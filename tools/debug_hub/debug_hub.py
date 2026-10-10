#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""debug_hub.py - tx_ble 调试平台（tkinter 兜底版，功能与 Qt 版对齐）。

入口统一：
    UART --> [A5 同步][类型][长度][payload] 帧
        类型 0x00: log 文本   -> 左栏（过滤/着色）
        类型 0x01: wave 记录  -> 右栏原始流监视 + 缓冲
                                 [surfer] 按钮 -> 全量导出 live.vcd -> Surfer.exe

wave 记录 12 字节小端：[u32 时间戳][u32 id][u32 值]
id = [type(1)|module(7)|feature(8)|instance(8)|evt(8)]
    LEVEL（bit31=1）: value 1=begin / 0=end，成对画"门"
    NUM  （bit31=0）: value 为采样数据
轨道名 grep 固件源码树里的 X-macro（与固件共享同一份定义）。

用法：
    python debug_hub.py                          # GUI，工具栏选 COM/波特率
    python debug_hub.py --port COM7 --baud 1000000
    python debug_hub.py --input dump.bin         # 离线：抓包 -> vcd

依赖：pyserial；Surfer: tools/debug_hub/surfer.exe（surfer 按钮拉起）。
"""

import argparse        # 命令行解析
import gzip            # （曾用于 gz 导出，保留备用）
import os
import re              # grep X-macro
import struct          # 12B 记录解包
import subprocess      # 拉起 Surfer
import sys
import threading       # 串口读线程
import time            # （预留）

# ================= 协议常量（与固件对应）=================
SYNC = 0xA5            # 帧同步字节
TYPE_LOG = 0x00        # 帧：log 文本
TYPE_WAVE = 0x01       # 帧：wave 记录流
RECORD_SIZE = 12       # 单条 wave 记录 = 3 个 u32
MAX_PAYLOAD = 236      # payload 上限（rb 块 240 - 硬件头 4）
EVT_PROBE = 0xFF       # feature 级探针标记（evt=0xFF）

# ================= id 映射正则（与 Qt 版保持同步）=================
# 模块注册表块：#define WAVE_MODULE_LIST(X) ... （到下一个 #define 截止）
MOD_LIST_RE = re.compile(r'#define\s+WAVE_MODULE_LIST\s*\(X\)(.*?)(?=\n#define)', re.S)
# 模块表项 X(短名, 值, "显示名")
MOD_ITEM_RE = re.compile(r'X\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*"([^"]+)"\s*\)')
# feature 子表块：#define WAVE_XXX_FEATS(X) ...
FEATS_BLOCK_RE = re.compile(
    r'#define\s+WAVE_\w+_FEATS\s*\(X\)(.*?)(?=\n#define|\nWAVE_FEAT_ENUM|\Z)', re.S)
# feature 表项 X(模块短名, feature短名, 值)
FEAT_ITEM_RE = re.compile(
    r'X\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')


def _num(tok):
    """'0x01'/'1' -> int（int(tok,0) 自动识别进制）。"""
    return int(tok, 0)


def _strip_comments(text):
    """剥掉 C 注释，避免头文件注释里的示例触发正则误匹配。"""
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)   # /* 块注释 */
    text = re.sub(r'//[^\n]*', '', text)                # // 行注释
    return text


def load_map(root):
    """扫描 root 下所有 .h，grep 出模块/feature 注册表。

    返回：
      modules  = {模块号: (短名, 显示名)}
      features = {(模块号, feature号): (模块短名, feature短名)}
    上位机与固件读同一份 X-macro -> 改打点无需同步版本。
    """
    import pathlib                       # 延迟导入
    modules, features = {}, {}
    for h in pathlib.Path(root).rglob('*.h'):            # 递归所有头文件
        if h.name.startswith('debug_hub'):               # 跳过本工具文件
            continue
        try:
            text = _strip_comments(h.read_text(encoding='utf-8', errors='ignore'))
        except OSError:
            continue                                     # 读不了就跳过
        for m in MOD_LIST_RE.finditer(text):             # 模块注册表
            for short, val, label in MOD_ITEM_RE.findall(m.group(1)):
                modules[_num(val)] = (short, label)
        for m in FEATS_BLOCK_RE.finditer(text):          # 各模块 feature 表
            for line in m.group(1).splitlines():
                fm = FEAT_ITEM_RE.search(line)
                if not fm:
                    continue
                mod_short, feat_short, feat_val = fm.groups()
                # 由模块短名反查模块号
                mod_id = next((k for k, (s, _l) in modules.items()
                               if s == mod_short), None)
                if mod_id is not None:
                    features[(mod_id, _num(feat_val))] = (mod_short, feat_short)
    return modules, features


def decode_id(w):
    """32 位 id -> (type, module, feature, instance, evt) 五元组。"""
    return ((w >> 31) & 1, (w >> 24) & 0x7F, (w >> 16) & 0xFF,
            (w >> 8) & 0xFF, w & 0xFF)


def track_name(mod, feat, inst, evt, modules, features):
    """五元组 -> 人类可读轨道名（如 adv/LEGACY/i2/s1），未知 id 降级数字命名。"""
    m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))       # 模块查表
    f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))      # feature 查表
    parts = [m[1], f[1]]                 # 模块名/feature 名
    if inst:
        parts.append(f'i{inst}')         # 实例号（0 省略）
    parts.append('probe' if evt == EVT_PROBE else f's{evt}')     # 事件道
    return '/'.join(parts)


# ================= 波形导出（层级 VCD）=================

def collect_tracks(records, modules, features):
    """第一遍扫描：收集"实际出现过的轨道"（只声明出现过的信号，面板无空占位）。

    返回 {(模块名, feature名, 实例, 道名, 位宽)}：
      LEVEL -> 1-bit；NUM -> 32-bit（probe 特殊命名）
    """
    tracks = set()                       # 去重
    for ts, wid, val in records:
        typ, mod, feat, inst, evt = decode_id(wid)
        m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))
        f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))
        if typ == 1:                                              # LEVEL 门
            tracks.add((m[1], f[1], inst, f's{evt}', 1))
        else:                                                     # NUM 数值
            lane = 'probe' if evt == EVT_PROBE else f's{evt}_num'
            tracks.add((m[1], f[1], inst, lane, 32))
    return sorted(tracks)                # 排序保证导出顺序稳定


class GzVcdWriter:
    """层级化 VCD 写出器（名字沿用，实际输出裸 VCD）。

    结构：tx_ble scope > 模块 scope > feature scope > 信号道 $var。
    实测坑（勿改）：
      1. 本机 Surfer 不识别 gzip -> 只能输出裸 VCD
      2. Surfer 要求 CRLF 行尾；vector 值 b... 与信号 id 间必须有空格
    一次性 export(records)：两遍写（先声明信号，再按时间序写值变更）。
    """

    def __init__(self, path, modules, features):
        self.path = path                 # 输出路径
        self.modules = modules           # 模块映射
        self.features = features         # feature 映射

    def export(self, records):
        """两遍写出：collect_tracks -> $scope/$var 声明 -> #ts/值变更。"""
        tracks = collect_tracks(records, self.modules, self.features)
        # newline='\r\n'：Surfer 的 VCD 解析器要求 CRLF
        with open(self.path, 'w', encoding='ascii', newline='\r\n') as f:
            f.write('$date now $end\n$version debug_hub $end\n')   # 文件头
            f.write('$timescale 1us $end\n')     # 名义刻度（实际用固件 tick）
            f.write('$scope module tx_ble $end\n')     # 根 scope（文件尾才闭）
            sym_of = {}                          # (模块,feature,实例,道) -> 符号
            self._nsym = 0                       # 符号生成计数器
            cur_scope = None                     # 当前 scope (模块, feature)

            def enter_scope(scope):
                """进入新 scope；切换时必须完整退栈，保证 $scope/$upscope 配对。

                否则 Surfer 解析失败（曾因少弹一层报 wave_source 错）。
                """
                nonlocal cur_scope
                if scope == cur_scope:
                    return                       # 已在该 scope
                if cur_scope is not None:        # 退旧：先 feature 层再 module 层
                    if cur_scope[1] is not None:
                        f.write('$upscope $end\n')
                    f.write('$upscope $end\n')
                f.write(f'$scope module {scope[0]} $end\n')   # 进新 module 层
                if scope[1] is not None:                       # 进新 feature 层
                    f.write(f'$scope module {scope[1]} $end\n')
                cur_scope = scope

            for mod_l, feat_l, inst, lane, width in tracks:    # 逐轨道声明
                # feature scope 名带实例后缀（不同实例分家显示）
                scope = (mod_l, feat_l + (f'/i{inst}' if inst else ''))
                enter_scope(scope)
                self._nsym += 1
                # 生成 VCD 符号 '!' '#' '$'…（ASCII 33 起循环）
                sym = '!' * (1 + self._nsym // 94) + chr(33 + self._nsym % 94)
                sym_of[(mod_l, feat_l, inst, lane)] = sym
                # $var wire <位宽> <符号> $<名字> $end
                f.write(f'$var wire {width} {sym} ${lane.replace("/", "_")} $end\n')
            if cur_scope is not None:            # 声明完退回根 scope
                if cur_scope[1] is not None:
                    f.write('$upscope $end\n')
                f.write('$upscope $end\n')
            f.write('$enddefinitions $end\n')    # 定义段结束

            last_ts = -1                         # 数据段：按时间序写值变更
            for ts, wid, val in records:
                typ, mod, feat, inst, evt = decode_id(wid)
                m = self.modules.get(mod, (f'm{mod}', f'mod{mod:#x}'))
                fl = self.features.get((mod, feat),
                                       (m[0], f'feat{feat:#x}'))[1]
                if typ == 1:                     # LEVEL：标量 0/1（紧贴 id）
                    key = (m[1], fl, inst, f's{evt}')
                    v = '1' if val else '0'
                else:                            # NUM：向量（b 与 id 间必须空格）
                    lane = 'probe' if evt == EVT_PROBE else f's{evt}_num'
                    key = (m[1], fl, inst, lane)
                    v = f'b{val & 0xFFFFFFFF:032b} '   # 空格是 VCD 规范
                if key not in sym_of:
                    continue                     # 未声明的 key 跳过（防御）
                if ts != last_ts:                # 时间戳变了才写 # 行
                    f.write(f'#{ts}\n')
                    last_ts = ts
                f.write(f'{v}{sym_of[key]}\n')   # 值变更行
            f.write('\n')                        # 文件尾空行
        return len(records)


# ================= 帧解析器 =================

class FrameParser:
    """[A5][type][len][payload] 帧解析（带失步重同步，逻辑与 Qt 版一致）。"""

    def __init__(self, on_log, on_wave):
        self.buf = bytearray()           # 半帧缓冲
        self.on_log = on_log             # log 帧回调
        self.on_wave = on_wave           # wave 记录回调

    def feed(self, data):
        """喂原始字节；找 sync -> 校验帧头 -> 凑齐 -> 回调；异常则步进重同步。"""
        self.buf += data                 # 追加新数据
        while True:
            i = self.buf.find(bytes([SYNC]))     # 找同步字节
            if i < 0:                            # 没有 sync：全是垃圾
                self.buf.clear()
                return
            if len(self.buf) < i + 3:            # 帧头不完整
                del self.buf[:i]                 # 丢 sync 前垃圾，等下一片
                return
            typ = self.buf[i + 1]                # 类型
            ln = self.buf[i + 2]                 # 长度
            if ln == 0 or ln > MAX_PAYLOAD or typ > 1:
                del self.buf[:i + 1]             # 非法帧头 = 假 sync，步进重试
                continue
            end = i + 3 + ln                     # 整帧终点
            if len(self.buf) < end:              # 帧未到齐
                del self.buf[:i]
                return                           # 留半帧等后续
            payload = bytes(self.buf[i + 3:end]) # 完整 payload
            del self.buf[:end]                   # 消费掉
            if typ == TYPE_LOG:                  # log：整块回调
                self.on_log(payload)
            else:                                # wave：按 12B 切条回调
                for off in range(0, ln - RECORD_SIZE + 1, RECORD_SIZE):
                    self.on_wave(struct.unpack_from('<III', payload, off))


# ================= 共享状态 =================

# wave 缓冲：buffer 累积全部记录（surfer 按钮用）；
# shown 记录右栏监视器已显示到哪条（显示与缓冲分离，互不影响）
wave_hub = {'buffer': [], 'shown': 0, 'lock': threading.Lock()}


def open_surfer(exe, vcd_path):
    """拉起 Surfer.exe 打开 VCD（非阻塞；失败只打印不抛出）。"""
    try:
        subprocess.Popen([exe, os.path.abspath(vcd_path)],
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print(f'[hub] surfer launched: {vcd_path}')
    except OSError as e:
        print(f'[hub] failed to launch surfer: {e}')


# ================= wave 监视器（右栏）=================

class WaveMonitor:
    """右栏原始 wave 流：一条记录一行，自动滚动。

    目的只是"肉眼确认 wave 还在打印"；精细分析交给 Surfer。
    显示行数超上限时删掉最旧的一半，防内存膨胀。
    """

    MAX_LINES = 2000                 # 显示区最大行数

    def __init__(self, text, modules, features):
        self.txt = text              # tkinter Text 控件
        self.modules = modules       # 模块映射
        self.features = features     # feature 映射
        self.count = 0               # 累计已显示记录数（顶栏计数用）
        # 颜色标签：LEVEL 蓝 / NUM 黄 / 未知灰
        text.tag_configure('lvl', foreground='#4fc3f7')
        text.tag_configure('num', foreground='#ffd54f')
        text.tag_configure('unk', foreground='#777')

    def drain(self):
        """把缓冲里"尚未显示"的记录刷到屏幕（显示游标推进，缓冲不删）。"""
        with wave_hub['lock']:       # 串口线程在写 buffer，必须加锁
            recs = wave_hub['buffer'][wave_hub['shown']:]   # 只取新记录
            wave_hub['shown'] = len(wave_hub['buffer'])     # 推进游标
        for ts, wid, val in recs:
            typ, mod, feat, inst, evt = decode_id(wid)
            base = track_name(mod, feat, inst, evt, self.modules, self.features)
            if typ == 1:             # LEVEL 行：begin/end 蓝色
                self.txt.insert('end',
                                f'{ts:>10}  {wid:08X}  {"begin" if val else "end  "}  {base}\n',
                                'lvl')
            else:                    # NUM 行：数值黄色
                self.txt.insert('end',
                                f'{ts:>10}  {wid:08X}  {val:<10}  {base}/num\n',
                                'num')
        self.count += len(recs)
        # 行数超限：删掉前一半（简单防膨胀策略）
        if int(self.txt.index('end-1c').split('.')[0]) > self.MAX_LINES:
            self.txt.delete('1.0', f'{self.MAX_LINES // 2}.0')
        self.txt.see('end')          # 自动滚到底


# ================= GUI =================

def run_gui(log_queue, args, modules, features):
    """构建 tkinter 界面并进入主循环（阻塞到窗口关闭）。"""
    import tkinter as tk
    from tkinter import ttk
    from tkinter.scrolledtext import ScrolledText

    root = tk.Tk()                   # 主窗口
    root.title('tx_ble debug hub')
    root.geometry('1280x640')        # 默认尺寸

    # ---------------- 工具栏 ----------------
    bar = tk.Frame(root)             # 顶部工具栏容器
    bar.pack(fill='x')               # 横向撑满

    import serial.tools.list_ports   # COM 口枚举

    tk.Label(bar, text='port:').pack(side='left')    # 串口选择
    var_port = tk.StringVar(value=args.port or '')
    cb_port = ttk.Combobox(bar, textvariable=var_port, width=8,
                           values=[p.device for p in
                                   serial.tools.list_ports.comports()])
    cb_port.pack(side='left', padx=(2, 8))

    tk.Label(bar, text='baud:').pack(side='left')    # 波特率选择
    var_baud = tk.StringVar(value=str(args.baud))
    cb_baud = ttk.Combobox(bar, textvariable=var_baud, width=9,
                           values=['9600', '115200', '460800',
                                   '921600', '1000000', '2000000'])
    cb_baud.pack(side='left', padx=(2, 8))

    ser = [None]                     # 当前串口对象（列表模拟可变闭包变量）
    reader_thr = [None]              # 读线程句柄

    def reader_loop(port):
        """串口读线程主体：循环读 -> 喂解析器；串口对象被换掉时退出。"""
        n = [0]                      # 字节计数（raw dump 用）
        while ser[0] is port and port.is_open:       # ser[0] 变了 = 已断开
            try:
                data = port.read(8192)               # 批量读
            except Exception:
                break                                # 串口异常（拔线等）
            if data:
                if args.raw_dump:                    # 调试原始流开关
                    n[0] += len(data)
                    sys.stderr.write(f'[raw {n[0]:7d}B] {data[:64].hex(" ")}\n')
                hub_parser.feed(data)                # 喂帧解析器

    def toggle_connect():
        """连接/断开按钮：管理串口对象与读线程。"""
        if ser[0] is not None:                       # 已连接 -> 断开
            p = ser[0]
            ser[0] = None                            # 置 None 让读线程退出
            try:
                p.close()                            # 关串口（释放占用）
            except Exception:
                pass
            btn_conn.configure(text='connect', bg='#27ae60')
            lbl_conn.configure(text='disconnected', fg='#888')
            return
        try:
            # 打开串口（参数取自下拉框）
            p = serial.Serial(var_port.get(), int(var_baud.get()), timeout=0.05)
        except Exception as e:
            lbl_conn.configure(text=f'open failed: {e}', fg='#e74c3c')
            return
        ser[0] = p
        btn_conn.configure(text='disconnect', bg='#c0392b')   # 按钮变"断开"
        lbl_conn.configure(text=f'{var_port.get()} @ {var_baud.get()}',
                           fg='#2ecc71')
        # 启动读线程（daemon=True：主窗口关闭自动死）
        reader_thr[0] = threading.Thread(target=reader_loop, args=(p,),
                                         daemon=True)
        reader_thr[0].start()

    btn_conn = tk.Button(bar, text='connect', command=toggle_connect,
                         bg='#27ae60', fg='white', width=10)
    btn_conn.pack(side='left', padx=(0, 4))
    lbl_conn = tk.Label(bar, text='disconnected', fg='#888')   # 状态文字
    lbl_conn.pack(side='left', padx=(0, 12))

    tk.Label(bar, text='send:').pack(side='left')    # 发送区
    var_send = tk.StringVar()
    ent_send = tk.Entry(bar, textvariable=var_send, width=22)
    ent_send.pack(side='left', padx=(2, 4))

    def do_send(_event=None):
        """发送：hex 勾选时按十六进制解析，否则按 ASCII；统一补 \\r\\n。"""
        if ser[0] is None or not var_send.get():
            return                                   # 未连接/空输入
        text = var_send.get()
        if var_hex.get():                            # hex 模式
            try:
                payload = bytes.fromhex(
                    text.replace(' ', '').replace(',', ''))   # 忽略空格逗号
                if not payload:
                    raise ValueError('empty hex')
            except ValueError as e:
                lbl_conn.configure(text=f'bad hex: {e}', fg='#e74c3c')
                return                               # 非法 hex 不发送
        else:                                        # ASCII 模式
            payload = text.encode('ascii', errors='replace')
        try:
            ser[0].write(payload + b'\r\n')          # 补回车换行发送
            var_send.set('')                         # 成功才清输入框
        except Exception as e:
            lbl_conn.configure(text=f'send failed: {e}', fg='#e74c3c')
    var_hex = tk.BooleanVar(value=False)             # hex 开关变量
    tk.Checkbutton(bar, text='hex', variable=var_hex).pack(side='left')
    btn_send = tk.Button(bar, text='tx', command=do_send)
    btn_send.pack(side='left')
    ent_send.bind('<Return>', do_send)               # 回车 = 发送

    tk.Label(bar, text='    filter:').pack(side='left')   # 过滤框（子串匹配）
    var_filter = tk.StringVar()
    tk.Entry(bar, textvariable=var_filter, width=18).pack(side='left', padx=4)

    def refresh_wave():
        """surfer 按钮：全量缓冲 -> live.vcd -> 拉起 Surfer。"""
        with wave_hub['lock']:
            recs = list(wave_hub['buffer'])          # 拷贝快照，避免边写边导
        if not recs:
            lbl_wave.configure(text='no wave records buffered', fg='#e67e22')
            return
        w = GzVcdWriter(args.vcd, modules, features)
        w.export(recs)                               # 覆盖写全量 VCD
        # 拉起前校验文件完整（写坏会导致 Surfer 报错，用户会以为平台坏了）
        try:
            with open(args.vcd, 'r', encoding='ascii') as f:
                if '$enddefinitions' not in f.read():
                    raise OSError('missing $enddefinitions')
        except OSError as e:
            lbl_wave.configure(text=f'export corrupted: {e}', fg='#e74c3c')
            return
        lbl_wave.configure(text=f'{len(recs)} records -> {args.vcd}')
        if not args.no_surfer:
            open_surfer(args.surfer, args.vcd)

    def clear_wave():
        """clear 按钮：清空缓冲与右栏，重新抓一段。"""
        with wave_hub['lock']:
            wave_hub['buffer'].clear()
            wave_hub['shown'] = 0                    # 显示游标归零
        wave_monitor.txt.delete('1.0', 'end')        # 清右栏显示
        wave_monitor.count = 0
        lbl_wave.configure(text='buffer cleared', fg='#888')

    btn_wave = tk.Button(bar, text='surfer', command=refresh_wave,
                         bg='#2980b9', fg='white')
    btn_wave.pack(side='left', padx=(12, 2))
    tk.Button(bar, text='clear', command=clear_wave).pack(side='left')
    lbl_wave = tk.Label(bar, text='wave: buffering (0 records)', fg='#888')
    lbl_wave.pack(side='left', padx=8)

    paused = [False]                 # 暂停标志（保留的 UI 功能位）

    def toggle_pause():
        """pause 按钮：冻结/恢复界面刷新（逻辑预留）。"""
        paused[0] = not paused[0]
        btn_pause.configure(text='resume' if paused[0] else 'pause')
    btn_pause = tk.Button(bar, text='pause', command=toggle_pause)
    btn_pause.pack(side='left', padx=4)

    # ---------------- 主区分屏：左 log | 右 wave ----------------
    paned = tk.PanedWindow(root, orient='horizontal', sashwidth=4)
    paned.pack(fill='both', expand=True)         # 占满剩余空间

    left = tk.Frame(paned, bg='#111')            # 左：log 面板
    paned.add(left, width=640)
    txt = ScrolledText(left, font=('Consolas', 9), bg='#111', fg='#ddd')
    txt.pack(fill='both', expand=True)
    # 关键词着色标签
    txt.tag_configure('warn', foreground='#e67e22')
    txt.tag_configure('err', foreground='#e74c3c')
    txt.tag_configure('ok', foreground='#2ecc71')

    right = tk.Frame(paned, bg='#181818')        # 右：wave 监视面板
    paned.add(right, width=620)
    wtxt = ScrolledText(right, font=('Consolas', 8), bg='#181818', fg='#bbb')
    wtxt.pack(fill='both', expand=True)
    wave_monitor = WaveMonitor(wtxt, modules, features)   # 监视器绑定右栏

    def pump():
        """主循环定时器：刷 log 队列 + wave 监视，150ms 一次。"""
        try:
            while True:                          # 一次搬空 log 队列
                line = log_queue.get_nowait()
                flt = var_filter.get()           # 过滤词（子串匹配）
                if flt and flt not in line:
                    continue                     # 不匹配则丢弃不显示
                low = line.lower()
                tag = None                       # 关键词着色
                if 'err' in low or 'fail' in low:
                    tag = 'err'
                elif 'warn' in low:
                    tag = 'warn'
                elif 'ok' in low or 'ready' in low or 'done' in low:
                    tag = 'ok'
                txt.insert('end', line, tag or ())
                txt.see('end')                   # 自动滚动到底
        except Exception:
            pass                                 # 队列空了就退出本次循环
        wave_monitor.drain()                     # 刷 wave 监视器
        n = wave_monitor.count
        if lbl_wave.cget('text').startswith('wave:'):
            lbl_wave.configure(text=f'wave: {n} records')   # 顶栏计数
        root.after(150, pump)                    # 150ms 后再来一轮

    # ---------------- 解析器接线 ----------------
    def on_log(payload):
        """log 帧回调：解码文本放进队列（由主线程 pump 显示）。"""
        log_queue.put(payload.decode('ascii', errors='replace'))

    def on_wave(rec):
        """wave 记录回调：加锁进缓冲（显示由 pump 定期刷）。"""
        with wave_hub['lock']:
            wave_hub['buffer'].append(rec)

    hub_parser = FrameParser(on_log=on_log, on_wave=on_wave)

    # 命令行指定了口：自动预选并连接
    if args.port:
        var_port.set(args.port)
        toggle_connect()

    root.after(150, pump)            # 启动定时刷新
    root.mainloop()                  # 进入 tkinter 主循环（阻塞）


# ================= 入口 =================

def main():
    """命令行入口：参数 -> id 映射 -> 离线转换或 GUI。"""
    ap = argparse.ArgumentParser(description='tx_ble debug hub')
    ap.add_argument('--port', help='启动即连接的串口，如 COM7')
    ap.add_argument('--input', help='离线模式：抓包文件')
    ap.add_argument('--baud', type=int, default=1000000)      # 默认 1M
    ap.add_argument('--root', default=None, help='源码树根（默认脚本上两级）')
    ap.add_argument('--vcd', default=None, help='VCD 输出路径')
    ap.add_argument('--surfer', default=None, help='surfer.exe 路径')
    ap.add_argument('--no-surfer', action='store_true')       # 只导出不拉起
    ap.add_argument('--raw-dump', action='store_true')        # 原始字节调试
    args = ap.parse_args()

    # 默认路径相对脚本定位（任何 cwd 运行一致）
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if args.vcd is None:
        args.vcd = os.path.join(script_dir, 'live.vcd')
    if args.surfer is None:
        args.surfer = os.path.abspath(os.path.join(script_dir, 'surfer.exe'))
    if args.root is None:
        # 脚本在 <sdk>/tools/debug_hub -> SDK 根上两级
        root = os.path.abspath(os.path.join(script_dir, '..', '..'))
    else:
        root = os.path.abspath(args.root)
    modules, features = load_map(root)           # 建 id 映射
    print(f'[hub] scanned: {len(modules)} modules, {len(features)} features')
    for k, (short, label) in sorted(modules.items()):    # 打印映射表
        print(f'  mod {k:#04x} {short:<8} {label}')

    if args.input:                               # ----- 离线模式 -----
        data = open(args.input, 'rb').read()     # 读抓包文件
        lines = []                               # log 收集
        recs = []                                # wave 收集
        p2 = FrameParser(on_log=lines.append,
                         on_wave=lambda r: recs.append(r))
        p2.feed(data)                            # 一次性解析
        for line in lines:                       # log 回显 stdout
            sys.stdout.write(line.decode('ascii', errors='replace'))
        w = GzVcdWriter(args.vcd, modules, features)
        w.export(recs)                           # 全量导出 VCD
        print(f'[hub] offline done: {len(recs)} records -> {args.vcd}')
        return

    # ----- 实时模式：GUI -----
    import queue
    log_queue = queue.Queue()        # 串口线程 -> UI 线程的 log 队列
    try:
        run_gui(log_queue, args, modules, features)
    finally:
        print('[hub] closed')


if __name__ == '__main__':
    main()
