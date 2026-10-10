#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""debug_hub_qt.py - tx_ble 调试平台主程序（PySide6 界面版）。

整体流程：
    固件 UART --> [A5][type][len][payload] 帧流
        type 0x00: log 文本   --> 左栏（HTML 着色、关键词过滤）
        type 0x01: wave 记录  --> 右栏原始流监视 + 缓冲区
                                 [surfer] 按钮 -> 全量导出 live.vcd -> Surfer.exe

wave 记录定长 12 字节（小端）：[u32 时间戳][u32 id][u32 值]
id 位布局 = [type(1)|module(7)|feature(8)|instance(8)|evt(8)]：
    LEVEL（bit31=1）: value 1=begin / 0=end，同轨道成对出现画成"门"
    NUM  （bit31=0）: value 就是采样数据，画成数值曲线
轨道名来自固件头文件里的 X-macro 列表（WAVE_MODULE_LIST / WAVE_*_FEATS），
本程序启动时 grep 源码树自动建立 id->名字 映射，与固件无需同步发版。

用法：
    python debug_hub_qt.py                              # 打开 GUI，工具栏选 COM/波特率
    python debug_hub_qt.py --port COM7 --baud 1000000   # 启动时直接连
    python debug_hub_qt.py --input dump.bin             # 离线模式：抓包文件 -> VCD

依赖：pyserial、PySide6（pip install pyserial PySide6）
"""

# ================= 标准库导入 =================
import argparse        # 命令行参数解析
import gzip            # （曾用于 gz-VCD，现保留备用）
import os              # 路径拼接、文件存在判断
import re              # 正则：grep 固件头文件里的 X-macro
import struct          # 二进制解包：12B wave 记录 -> (ts, id, value)
import subprocess      # 启动 Surfer.exe 子进程
import sys             # stdout/stderr 输出

# ================= 协议常量（与固件 debug_port.h/wave.h 对应）=================
SYNC = 0xA5            # 帧同步字节：每帧固定以 0xA5 开头，host 靠它定界
TYPE_LOG = 0x00        # 帧类型 0：log 文本（log.c 产出，自带 \r\n）
TYPE_WAVE = 0x01       # 帧类型 1：wave 记录流（12 字节对齐）
RECORD_SIZE = 12       # 单条 wave 记录字节数 = 3 × u32
MAX_PAYLOAD = 236      # 单帧 payload 上限 = 环形缓冲块 240 - 硬件头 4
EVT_PROBE = 0xFF       # evt=0xFF 表示"feature 级探针"（marker/心跳类打点）

# ================= id 映射（上位机与固件共享同一 X-macro 源）=================
# 注意：本段与 debug_hub.py 保持同步；将来可拆成 hub_core.py 统一 import。

# 匹配 "#define WAVE_MODULE_LIST(X) ... X(SCHED, 0x01, "scheduler") ..."
# (?=\n#define) 是前瞻断言：匹配到下一个 #define 之前停止（块边界）
MOD_LIST_RE = re.compile(r'#define\s+WAVE_MODULE_LIST\s*\(X\)(.*?)(?=\n#define)', re.S)
# 匹配模块表项 X(短名, 值, "显示名")，例如 X(SCHED, 0x01, "scheduler")
MOD_ITEM_RE = re.compile(r'X\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*"([^"]+)"\s*\)')
# 匹配各模块的 feature 子表 "#define WAVE_XXX_FEATS(X) ... X(ADV, LEGACY, 0x01)..."
# 边界：下一个 #define、紧跟的 WAVE_FEAT_ENUM、或文件结尾
FEATS_BLOCK_RE = re.compile(
    r'#define\s+WAVE_\w+_FEATS\s*\(X\)(.*?)(?=\n#define|\nWAVE_FEAT_ENUM|\Z)', re.S)
# 匹配 feature 表项 X(模块短名, feature 短名, 值)，例如 X(ADV, LEGACY, 0x01)
FEAT_ITEM_RE = re.compile(
    r'X\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')


def _num(tok):
    """把 '0x01' / '1' 之类的字面量转成 int（int(tok,0) 自动识别进制）。"""
    return int(tok, 0)


def _strip_comments(text):
    """剥掉 C 注释（/* 块注释 */ 和 // 行注释），避免注释里的示例干扰正则。"""
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)   # 块注释（跨行，re.S 让 . 匹配换行）
    text = re.sub(r'//[^\n]*', '', text)                # 行注释（到行尾）
    return text


def load_map(root):
    """扫描 root 下所有 .h 头文件，grep 出 wave id 的模块/feature 注册表。

    返回两个字典：
      modules  = {模块号: (短名, 显示名)}      例 {0x01: ('SCHED','scheduler')}
      features = {(模块号,feature号): (模块短名, feature短名)}
                 例 {(0x03,0x01): ('ADV','LEGACY')}
    设计要点：上位机与固件读同一份 X-macro 源 -> 改打点不用同步两边版本。
    """
    import pathlib                       # 延迟导入：只在需要扫描时加载
    modules, features = {}, {}
    for h in pathlib.Path(root).rglob('*.h'):            # 递归遍历所有头文件
        if h.name.startswith('debug_hub'):               # 跳过本工具自己的文件
            continue
        try:
            # 读取并剥注释（errors='ignore' 容忍非 UTF8 字节的 C 源文件）
            text = _strip_comments(h.read_text(encoding='utf-8', errors='ignore'))
        except OSError:
            continue                                     # 读不了（权限/被占用）就跳过
        for m in MOD_LIST_RE.finditer(text):             # 先收模块注册表
            for short, val, label in MOD_ITEM_RE.findall(m.group(1)):
                modules[_num(val)] = (short, label)      # {0x01: ('SCHED','scheduler')}
        for m in FEATS_BLOCK_RE.finditer(text):          # 再收各模块 feature 子表
            for line in m.group(1).splitlines():         # 逐行找 X(...) 表项
                fm = FEAT_ITEM_RE.search(line)
                if not fm:
                    continue                             # 该行不是 feature 表项
                mod_short, feat_short, feat_val = fm.groups()
                # 用模块短名反查模块号（X(...) 里只有短名，没有模块号）
                mod_id = next((k for k, (s, _l) in modules.items()
                               if s == mod_short), None)
                if mod_id is not None:
                    # 记录 (模块号, feature号) -> (模块短名, feature短名)
                    features[(mod_id, _num(feat_val))] = (mod_short, feat_short)
    return modules, features


def decode_id(w):
    """把 32 位 id 拆成五元组 (type, module, feature, instance, evt)。

    id = [type(1)|module(7)|feature(8)|instance(8)|evt(8)]
    注意 module 用 0x7F 掩码：bit31 是 type，不能混进模块号。
    """
    return ((w >> 31) & 1, (w >> 24) & 0x7F, (w >> 16) & 0xFF,
            (w >> 8) & 0xFF, w & 0xFF)


def track_name(mod, feat, inst, evt, modules, features):
    """把 id 五元组翻译成人类可读的轨道名字符串。

    例：adv/LEGACY/i2/s1（模块/feature/实例/状态道）
    未知 id 降级为数字命名（mod0x0/feat0x3），保证任何 id 都能显示。
    """
    # 查不到就用数字占位（m 元组 = (短名, 显示名)，这里取显示名）
    m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))
    f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))
    parts = [m[1], f[1]]                 # 第 1、2 级：模块名/feature 名
    if inst:
        parts.append(f'i{inst}')         # 第 3 级：运行时实例号（0 则省略）
    parts.append('probe' if evt == EVT_PROBE else f's{evt}')  # 第 4 级：事件道
    return '/'.join(parts)               # 斜杠连接成层级路径


# ================= 波形导出（VCD）=================

def collect_tracks(records, modules, features):
    """第一遍扫描：从记录集里收集"实际出现过的轨道"。

    返回集合 {(模块显示名, feature显示名, 实例号, 信号道名, 位宽)}：
      LEVEL -> (…, 's<evt>', 1)   1-bit 电平线
      NUM   -> (…, 's<evt>_num', 32)  32-bit 数值线
    用途：VCD 只声明出现过的信号，Surfer 面板无空占位（用户要求）。
    """
    tracks = set()                       # set 自动去重：同轨道只留一份
    for ts, wid, val in records:
        typ, mod, feat, inst, evt = decode_id(wid)
        m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))   # 模块查表（含降级）
        f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))  # feature 查表
        if typ == 1:                                              # LEVEL：电平门
            tracks.add((m[1], f[1], inst, f's{evt}', 1))
        else:                                                     # NUM：数值采样
            lane = 'probe' if evt == EVT_PROBE else f's{evt}_num'
            tracks.add((m[1], f[1], inst, lane, 32))
    return sorted(tracks)                # 排序保证每次导出的信号顺序稳定


class GzVcdWriter:
    """层级化 VCD 写出器（类名沿用 GzVcdWriter，实际输出裸 VCD）。

    结构：
        $scope module tx_ble > $scope module <模块> > $scope module <feature>
        > $var 信号道（s1 门 / s0_num 数值）
    两大坑（实测记录，勿改）：
      1. 本机 Surfer 不识别 gzip（合法 .vcd.gz 也报 UnknownFileFormat）-> 输出裸 VCD
      2. Surfer 的 VCD 解析器要求 CRLF 行尾，且 vector 值 b... 与信号 id
         之间必须有空格，否则 body 解析失败
    用法：一次性 export(records)，二遍写完（先声明后数据）。
    """

    def __init__(self, path, modules, features):
        self.path = path                 # 输出文件路径（通常 live.vcd）
        self.modules = modules           # 模块映射表（load_map 产出）
        self.features = features         # feature 映射表

    def export(self, records):
        """两遍写法：第一遍收集轨道并声明，第二遍按时间序写值变更。"""
        tracks = collect_tracks(records, self.modules, self.features)
        # newline='\r\n'：Surfer 要求 CRLF（LF 会报 "expected an id"）
        with open(self.path, 'w', encoding='ascii', newline='\r\n') as f:
            # --- VCD 文件头（定义段）---
            f.write('$date now $end\n$version debug_hub_qt $end\n')
            f.write('$timescale 1us $end\n')             # 名义刻度，实际用固件 tick 原值
            f.write('$scope module tx_ble $end\n')       # 根 scope（不闭合，直到文件尾）
            sym_of, width_of = {}, {}    # (模块,feature,实例,道) -> VCD 符号 / 位宽
            self._nsym = 0               # 符号计数器，用于生成 !、#、$ …
            cur_scope = None             # 当前打开的 scope (模块, feature)

            def enter_scope(scope):
                """进入一个新的 (模块, feature) scope；自动平衡 $upscope。

                关键：切换到同级 scope 时必须把之前开的 feature 层和
                module 层**各**弹出一层，否则 $scope/$upscope 不配对，
                Surfer 直接解析失败（踩过的坑，见 DEBUG_PLATFORM.md 9.7）。
                """
                nonlocal cur_scope       # 修改外层变量需要 nonlocal
                if scope == cur_scope:
                    return               # 已在该 scope 内，无需切换
                if cur_scope is not None:                    # 闭合旧嵌套
                    if cur_scope[1] is not None:             # 先弹 feature 层
                        f.write('$upscope $end\n')
                    f.write('$upscope $end\n')               # 再弹 module 层
                f.write(f'$scope module {scope[0]} $end\n')  # 开新的 module 层
                if scope[1] is not None:                     # 再开 feature 层
                    f.write(f'$scope module {scope[1]} $end\n')
                cur_scope = scope            # 记录当前 scope

            # --- 遍历轨道，逐个声明 $var ---
            for mod_l, feat_l, inst, lane, width in tracks:
                # feature scope 名带上实例号（同一 feature 的不同实例分家显示）
                scope = (mod_l, feat_l + (f'/i{inst}' if inst else ''))
                enter_scope(scope)           # 确保在正确层级内
                self._nsym += 1
                # 生成 VCD 符号：'!'、'#'、'$'…（94 个可见 ASCII 循环，>94 加前缀）
                sym = '!' * (1 + self._nsym // 94) + chr(33 + self._nsym % 94)
                sym_of[(mod_l, feat_l, inst, lane)] = sym    # 记录符号映射
                width_of[(mod_l, feat_l, inst, lane)] = width
                # 声明信号：$var wire <位宽> <符号> $<名字> $end（名字里 / 换 _）
                f.write(f'$var wire {width} {sym} ${lane.replace("/", "_")} $end\n')
            if cur_scope is not None:        # 声明结束，退回根 scope
                if cur_scope[1] is not None:
                    f.write('$upscope $end\n')               # 弹 feature 层
                f.write('$upscope $end\n')                   # 弹 module 层
            f.write('$enddefinitions $end\n')                # 定义段结束标记

            # --- 第二遍：按时间序写值变更（数据段）---
            last_ts = -1                     # 上一个时间戳（避免重复写 #ts）
            for ts, wid, val in records:
                typ, mod, feat, inst, evt = decode_id(wid)
                m = self.modules.get(mod, (f'm{mod}', f'mod{mod:#x}'))
                # feature 只取显示名（第二维），组成与声明一致的 key
                fl = self.features.get((mod, feat),
                                       (m[0], f'feat{feat:#x}'))[1]
                if typ == 1:                 # LEVEL：标量 0/1（无空格，紧贴 id）
                    key = (m[1], fl, inst, f's{evt}')
                    v = '1' if val else '0'
                else:                        # NUM：32bit 向量，b 与 id 间必须有空格
                    lane = 'probe' if evt == EVT_PROBE else f's{evt}_num'
                    key = (m[1], fl, inst, lane)
                    v = f'b{val & 0xFFFFFFFF:032b} '   # 空格是 VCD 规范要求
                if key not in sym_of:
                    continue                 # 不在声明集里（理论上不会发生）
                if ts != last_ts:            # 时间戳变化才写新 # 行（压缩体积）
                    f.write(f'#{ts}\n')
                    last_ts = ts
                f.write(f'{v}{sym_of[key]}\n')   # 值变更行：<值><符号>
            f.write('\n')                    # 文件尾空行（容错）
        return len(records)                  # 返回导出的记录数（UI 显示用）


# ================= 帧解析器 =================

class FrameParser:
    """[A5][type][len][payload] 帧流解析器（带失步重同步）。

    核心逻辑：
      1. 在缓冲里找 0xA5 同步字节
      2. 校验 type/len 合法性（type<=1，1<=len<=236）
      3. 凑齐整帧后取出 payload，回调 on_log / on_wave
      4. 任何一步不满足就丢弃到下一个 0xA5（自愈，防串口错位）
    状态零依赖：除了缓冲区没有任何状态，丢帧不会污染后续解析。
    """

    def __init__(self, on_log, on_wave):
        self.buf = bytearray()           # 未解析完的字节缓冲
        self.on_log = on_log             # log 帧回调（参数为 bytes payload）
        self.on_wave = on_wave           # wave 记录回调（参数为 (ts,id,val) 元组）

    def feed(self, data):
        """喂入新收到的原始字节；循环消化到缓冲不足一帧为止。"""
        self.buf += data                 # 追加到缓冲（串口分片到达）
        while True:
            i = self.buf.find(bytes([SYNC]))     # 从头找同步字节
            if i < 0:                            # 找不到 0xA5
                self.buf.clear()                 # 整段都是垃圾，全丢
                return
            if len(self.buf) < i + 3:            # 同步字节后不足 3 字节帧头
                del self.buf[:i]                 # 丢掉 0xA5 之前的垃圾，等下一片
                return
            typ = self.buf[i + 1]                # 帧类型字节
            ln = self.buf[i + 2]                 # 长度字节（payload 字节数）
            if ln == 0 or ln > MAX_PAYLOAD or typ > 1:
                del self.buf[:i + 1]             # 非法帧头：多半是假 0xA5，
                continue                         # 步进 1 字节继续找（重同步）
            end = i + 3 + ln                     # 整帧结束位置（帧头 3 + payload）
            if len(self.buf) < end:              # 帧不完整
                del self.buf[:i]                 # 丢 0xA5 前的垃圾，保留半帧
                return                           # 等下一片数据凑齐
            payload = bytes(self.buf[i + 3:end]) # 取出完整 payload
            del self.buf[:end]                   # 从缓冲移除已消费部分
            if typ == TYPE_LOG:                  # log 帧：整块交给回调
                self.on_log(payload)
            else:                                # wave 帧：按 12B 切成多条记录
                for off in range(0, ln - RECORD_SIZE + 1, RECORD_SIZE):
                    # '<III' = 3 个小端 u32：(时间戳, id, 值)
                    self.on_wave(struct.unpack_from('<III', payload, off))


# ================= 串口工作线程 =================

def make_worker(qt):
    """SerialWorker 类工厂。

    QThread 子类依赖 PySide6 运行时；放工厂函数里可以延迟到 GUI 阶段
    再 import Qt（离线模式不需要 Qt，加快启动）。
    """
    from PySide6.QtCore import QThread, Signal   # 线程类与信号机制

    class SerialWorker(QThread):
        """串口读写线程：读在子线程，UI 更新经信号投递回主线程。

        为什么要子线程：串口 1M 波特率持续流，若在主线程读会卡死 UI。
        Qt 规则：非主线程不能直接碰控件，所以用 Signal 跨线程发消息，
        Qt 自动把槽调用排队到主线程事件循环（线程安全）。
        """
        # 三个跨线程信号：log 文本行 / 一条 wave 记录 / 状态栏消息
        log_line = Signal(str)
        wave_rec = Signal(object)    # object 可携带任意 Python 对象（元组）
        status_line = Signal(str)

        def __init__(self, port, baud, raw_dump):
            super().__init__()
            self.port = port         # 串口号字符串（如 'COM7'）
            self.baud = baud         # 波特率 int
            self.raw_dump = raw_dump # 调试开关：是否把原始字节 dump 到 stderr
            self._stop = False       # 停止标志（GUI 断开时置 True）
            self.ser = None          # 串口对象（run() 里创建）
            # 解析器：回调直接 emit 信号，把解析结果搬运到主线程
            self.parser = FrameParser(
                on_log=lambda p: self.log_line.emit(
                    p.decode('ascii', errors='replace')),   # 非法字节替换成 '?'
                on_wave=lambda rec: self.wave_rec.emit(rec))

        def write(self, payload):
            """发送数据（GUI 线程直接调用；串口 write 很快，无需跨线程）。"""
            if self.ser is not None:
                try:
                    self.ser.write(payload)
                    return True      # 成功（调用方据此清空输入框）
                except Exception as e:
                    self.status_line.emit(f'send failed: {e}')   # 状态栏报错
            return False

        def run(self):
            """线程主体：开串口 -> 循环读 -> 喂解析器 -> 停止时关闭。"""
            import serial
            try:
                # timeout=0.05：read 最多阻塞 50ms，保证 _stop 置位后能及时退出
                self.ser = serial.Serial(self.port, self.baud, timeout=0.05)
            except Exception as e:
                self.status_line.emit(f'open failed: {e}')       # 开口失败上报
                return
            n = 0                            # 累计字节计数（raw dump 用）
            while not self._stop:
                try:
                    data = self.ser.read(8192)   # 批量读（减少系统调用次数）
                except Exception:
                    break                        # 串口出错（拔线等）直接退出
                if data:
                    if self.raw_dump:            # 调试：原始十六进制流
                        n += len(data)
                        sys.stderr.write(f'[raw {n:7d}B] {data[:64].hex(" ")}\n')
                    self.parser.feed(data)       # 喂帧解析器（同步回调 emit 信号）
            self.ser.close()                     # 释放串口（否则占用导致重连失败）
            self.ser = None

    return SerialWorker


# ================= GUI =================

# 深色主题 QSS（Qt 样式表，语法类似 CSS）
DARK = """
QWidget { background: #1e1f22; color: #d4d4d4; font-size: 13px; }   /* 全局底色 */
QPlainTextEdit { background: #141517; color: #d4d4d4;               /* 文本面板：深底等宽字 */
                 font-family: Consolas; font-size: 12px; border: none; }
QLineEdit, QComboBox { background: #2a2b2f; border: 1px solid #3c3d42;
                       border-radius: 4px; padding: 3px 6px; }      /* 输入框/下拉 */
QComboBox QAbstractItemView { background: #2a2b2f;                  /* 下拉弹出列表 */
                              selection-background-color: #094771; }
QPushButton { background: #2a2b2f; border: 1px solid #3c3d42;       /* 按钮常态 */
              border-radius: 4px; padding: 4px 14px; }
QPushButton:hover { background: #35363b; }                          /* 悬停变亮 */
QPushButton:pressed { background: #094771; }                        /* 按下变蓝 */
QPushButton:checked { background: #094771; color: white; }          /* 可勾选按钮(hex) */
QSplitter::handle { background: #33343a; width: 3px; }              /* 分屏拖动条 */
QStatusBar { background: #18191c; color: #888; }                    /* 状态栏 */
"""

LOG_MAX_BLOCKS = 20000      # log 面板最大行数（超出自动丢最旧的一半，防内存膨胀）
WAVE_MAX_BLOCKS = 20000     # wave 面板同上

# HTML 着色模板（appendHtml 用；%s 处填转义后的文本）
STYLE_ERR = '<span style="color:#e74c3c">%s</span>'    # 红：错误
STYLE_WARN = '<span style="color:#e67e22">%s</span>'   # 橙：警告
STYLE_OK = '<span style="color:#2ecc71">%s</span>'     # 绿：成功
STYLE_LVL = '<span style="color:#4fc3f7">%s</span>'    # 蓝：LEVEL 门记录
STYLE_NUM = '<span style="color:#ffd54f">%s</span>'    # 黄：NUM 数值记录


def run_gui(args, modules, features):
    """构建并运行 Qt 界面（阻塞直到窗口关闭）。"""
    from PySide6.QtCore import Qt
    from PySide6.QtWidgets import (
        QApplication, QMainWindow, QWidget, QPlainTextEdit, QHBoxLayout,
        QVBoxLayout, QPushButton, QComboBox, QLabel, QLineEdit, QSplitter)

    import serial.tools.list_ports   # COM 口枚举

    SerialWorker = make_worker(None) # 生成线程类（延迟 Qt 绑定）

    app = QApplication(sys.argv)     # Qt 应用对象（全局唯一）
    app.setStyleSheet(DARK)          # 应用深色主题
    win = QMainWindow()              # 主窗口
    win.setWindowTitle('tx_ble debug hub')
    win.resize(1400, 700)            # 默认窗口尺寸

    central = QWidget()              # 中央容器（装工具栏 + 分屏）
    win.setCentralWidget(central)
    vbox = QVBoxLayout(central)      # 垂直布局：上工具栏、下分屏
    vbox.setContentsMargins(6, 6, 6, 6)   # 四周留白 6px
    vbox.setSpacing(6)               # 控件间距 6px

    # ---------------- 工具栏：串口配置 / 发送 / 过滤 / 导出 ----------------
    bar = QHBoxLayout()              # 工具栏（水平排布）
    cb_port = QComboBox()            # 串口选择下拉
    cb_port.setEditable(True)        # 可手输（自动枚举之外的自定义口）
    cb_port.setFixedWidth(95)
    # 枚举系统当前所有 COM 口填入下拉
    cb_port.addItems([p.device for p in serial.tools.list_ports.comports()])
    if args.port:                    # 命令行指定了口就预选
        cb_port.setCurrentText(args.port)
    cb_baud = QComboBox()            # 波特率下拉
    cb_baud.setEditable(True)        # 允许输入预设外的波特率
    cb_baud.addItems(['9600', '115200', '460800', '921600',
                      '1000000', '2000000'])
    cb_baud.setCurrentText(str(args.baud))   # 默认 1M（与固件一致）

    btn_conn = QPushButton('connect')            # 连接/断开按钮
    btn_conn.setFixedWidth(96)
    lbl_conn = QLabel('disconnected')            # 连接状态文字

    ent_send = QLineEdit()           # 发送输入框
    ent_send.setPlaceholderText('send... (Enter=tx)')    # 灰色提示文字
    cb_hex = QPushButton('hex')      # hex 模式开关（可按下 = 十六进制发送）
    cb_hex.setCheckable(True)
    cb_hex.setFixedWidth(48)
    btn_tx = QPushButton('tx')       # 发送按钮
    btn_tx.setFixedWidth(44)

    edt_filter = QComboBox()         # 过滤框（可编辑下拉，留历史记录位）
    edt_filter.setEditable(True)
    edt_filter.setFixedWidth(150)
    edt_filter.lineEdit().setPlaceholderText('filter')

    btn_surfer = QPushButton('surfer')   # 导出 VCD 并拉起 Surfer
    btn_clear = QPushButton('clear')     # 清空 wave 缓冲
    lbl_wave = QLabel('wave: 0 records') # wave 计数标签

    # 按顺序摆进工具栏
    bar.addWidget(QLabel('port:'))
    bar.addWidget(cb_port)
    bar.addWidget(QLabel('baud:'))
    bar.addWidget(cb_baud)
    bar.addWidget(btn_conn)
    bar.addWidget(lbl_conn)
    bar.addSpacing(12)               # 组间距
    bar.addWidget(QLabel('send:'))
    bar.addWidget(ent_send, 1)       # stretch=1：发送框吃掉剩余宽度
    bar.addWidget(cb_hex)
    bar.addWidget(btn_tx)
    bar.addSpacing(12)
    bar.addWidget(QLabel('filter:'))
    bar.addWidget(edt_filter)
    bar.addSpacing(12)
    bar.addWidget(btn_surfer)
    bar.addWidget(btn_clear)
    bar.addWidget(lbl_wave)
    vbox.addLayout(bar)              # 工具栏挂进主垂直布局

    # ---------------- 主区：左右分屏 ----------------
    split = QSplitter()              # 可拖动的水平分屏容器
    txt_log = QPlainTextEdit()       # 左：log 面板
    txt_log.setReadOnly(True)
    txt_log.setMaximumBlockCount(LOG_MAX_BLOCKS)     # 超限自动裁剪旧内容
    txt_wave = QPlainTextEdit()      # 右：wave 原始流面板
    txt_wave.setReadOnly(True)
    txt_wave.setMaximumBlockCount(WAVE_MAX_BLOCKS)
    split.addWidget(txt_log)
    split.addWidget(txt_wave)
    split.setSizes([700, 700])       # 初始各占一半
    vbox.addWidget(split, 1)         # stretch=1：分屏区吃掉全部剩余高度

    # 底部状态栏：显示 id 映射规模
    win.statusBar().showMessage(
        f'id map: {len(modules)} modules, {len(features)} features')

    # ---------------- 状态与回调 ----------------
    records = []                     # 全部 wave 记录（surfer 导出用，只增不清）
    worker = [None]                  # 当前串口线程（列表模拟可变闭包变量）

    def esc(s):
        """HTML 转义（& < >），防止 log 文本里的特殊字符破坏着色标签。"""
        return (s.replace('&', '&amp;').replace('<', '&lt;')
                .replace('>', '&gt;'))

    def append_log(line):
        """往 log 面板插一行，按关键词着色（err 红 / warn 橙 / ok 绿）。"""
        low = line.lower()           # 统一小写再匹配（大小写不敏感）
        if 'err' in low or 'fail' in low:
            style = STYLE_ERR
        elif 'warn' in low:
            style = STYLE_WARN
        elif 'ok' in low or 'ready' in low or 'done' in low:
            style = STYLE_OK
        else:
            style = '%s'             # 无关键词：不加颜色标签
        # 去掉行尾换行再插入（appendHtml 自带换行）
        txt_log.appendHtml(style % esc(line.rstrip('\r\n')))

    def on_log_line(line):
        """串口线程 log_line 信号的槽：直接追加显示。"""
        append_log(line)

    def on_wave_record(rec):
        """串口线程 wave_rec 信号的槽：入缓冲 + 右栏实时显示。"""
        records.append(rec)          # 进总缓冲（surfer 导出用）
        ts, wid, val = rec           # 解包：(时间戳, id, 值)
        typ, mod, feat, inst, evt = decode_id(wid)
        base = track_name(mod, feat, inst, evt, modules, features)   # 轨道名
        if typ == 1:                 # LEVEL：显示 begin/end，蓝色
            line = f'{ts:>10}  {wid:08X}  {"begin" if val else "end  "}  {base}'
            style = STYLE_LVL
        else:                        # NUM：显示数值，黄色
            line = f'{ts:>10}  {wid:08X}  {val:<10}  {base}/num'
            style = STYLE_NUM
        txt_wave.appendHtml(style % esc(line))       # 右栏滚动显示
        lbl_wave.setText(f'wave: {len(records)} records')   # 更新计数

    def on_status(line):
        """状态消息槽：显示到连接状态标签（开口失败/发送失败等）。"""
        lbl_conn.setText(line)

    def toggle_connect():
        """连接/断开按钮槽：管理串口线程生命周期。"""
        if worker[0] is not None:    # 已连接 -> 断开
            worker[0]._stop = True   # 通知线程退出读循环
            worker[0].wait(1000)     # 最多等 1 秒线程结束
            worker[0] = None
            btn_conn.setText('connect')              # 按钮恢复绿色
            btn_conn.setStyleSheet('background:#27ae60;color:white;border:none;'
                                   'border-radius:4px;padding:4px 14px;')
            lbl_conn.setText('disconnected')
            return
        try:
            # 新建线程对象（参数来自工具栏下拉框当前值）
            w = SerialWorker(cb_port.currentText(),
                             int(cb_baud.currentText()), args.raw_dump)
        except Exception as e:
            lbl_conn.setText(f'bad config: {e}')     # 配置非法（如波特率非数字）
            return
        w.log_line.connect(on_log_line)      # 三个信号接到对应槽
        w.wave_rec.connect(on_wave_record)
        w.status_line.connect(on_status)
        w.start()                            # 启动线程（进入 run()）
        worker[0] = w
        btn_conn.setText('disconnect')       # 按钮变红色"断开"
        btn_conn.setStyleSheet('background:#c0392b;color:white;border:none;'
                               'border-radius:4px;padding:4px 14px;')
        lbl_conn.setText(f'{cb_port.currentText()} @ {cb_baud.currentText()}')

    def do_send():
        """发送槽：按 hex/ascii 模式组包并写入串口（自动补 \\r\\n）。"""
        if worker[0] is None or not ent_send.text():
            return                           # 未连接或输入为空，忽略
        text = ent_send.text()
        if cb_hex.isChecked():               # hex 模式：'01 03 AA' -> 字节
            try:
                # 允许空格/逗号分隔，也允许连写
                payload = bytes.fromhex(text.replace(' ', '').replace(',', ''))
                if not payload:
                    raise ValueError('empty')
            except ValueError as e:
                lbl_conn.setText(f'bad hex: {e}')    # 非法 hex 提示，不发送
                return
        else:                                # ascii 模式：按原文编码
            payload = text.encode('ascii', errors='replace')
        if worker[0].write(payload + b'\r\n'):       # 统一补回车换行
            ent_send.clear()                 # 发送成功才清空输入框

    def do_surfer():
        """surfer 按钮：全量记录 -> live.vcd -> 拉起 Surfer.exe。"""
        if not records:
            lbl_wave.setText('no wave records')      # 没数据不导出
            return
        w = GzVcdWriter(args.vcd, modules, features)
        w.export(records)                    # 覆盖写整个 VCD（全量重导）
        # 启动 Surfer 前校验文件完整（缺 $enddefinitions = 写坏了）
        try:
            with open(args.vcd, 'r', encoding='ascii') as f:
                if '$enddefinitions' not in f.read():
                    raise OSError('missing $enddefinitions')
        except OSError as e:
            lbl_wave.setText(f'export corrupted: {e}')
            return
        lbl_wave.setText(f'{len(records)} records -> {args.vcd}')
        if not args.no_surfer:
            try:
                # Popen 非阻塞：Surfer 独立运行，不阻塞本 GUI
                subprocess.Popen(
                    [args.surfer, os.path.abspath(args.vcd)],
                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            except OSError as e:
                lbl_wave.setText(f'surfer launch failed: {e}')

    def do_clear():
        """clear 按钮：清空记录缓冲与右栏显示（重新抓一段）。"""
        records.clear()
        txt_wave.clear()
        lbl_wave.setText('wave: 0 records')

    # 发送框内按回车 = 点 tx 按钮
    ent_send.returnPressed.connect(do_send)

    # 其余按钮接线
    btn_conn.clicked.connect(toggle_connect)
    btn_tx.clicked.connect(do_send)
    btn_surfer.clicked.connect(do_surfer)
    btn_clear.clicked.connect(do_clear)

    win.show()                       # 显示窗口
    return app                       # 返回 app 供 main() 进事件循环


def main():
    """命令行入口：解析参数 -> 建立 id 映射 -> 离线转换或启动 GUI。"""
    ap = argparse.ArgumentParser(description='tx_ble debug hub (Qt)')
    ap.add_argument('--port', help='串口预选，如 COM7')
    ap.add_argument('--input', help='离线模式：抓包文件转 VCD')
    ap.add_argument('--baud', type=int, default=1000000)      # 默认 1M
    ap.add_argument('--root', default=None)                   # 源码树根（扫描 id）
    ap.add_argument('--vcd', default=None)                    # VCD 输出路径
    ap.add_argument('--surfer', default=None)                 # Surfer 路径
    ap.add_argument('--no-surfer', action='store_true')       # 只导出不拉 Surfer
    ap.add_argument('--raw-dump', action='store_true')        # 原始字节调试开关
    args = ap.parse_args()

    # 默认路径全部相对脚本定位（任何 cwd 运行都一致）
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if args.vcd is None:
        args.vcd = os.path.join(script_dir, 'live.vcd')
    if args.surfer is None:
        args.surfer = os.path.abspath(os.path.join(script_dir, 'surfer.exe'))
    if args.root is None:
        # 脚本在 <sdk>/tools/debug_hub 下 -> SDK 根是上两级
        root = os.path.abspath(os.path.join(script_dir, '..', '..'))
    else:
        root = os.path.abspath(args.root)
    modules, features = load_map(root)       # 扫描固件头文件建映射
    print(f'[hub] scanned: {len(modules)} modules, {len(features)} features')

    if args.input:                           # ----- 离线模式 -----
        w = GzVcdWriter(args.vcd, modules, features)
        data = open(args.input, 'rb').read() # 读整个抓包文件
        lines = []                           # log 帧收集
        recs = []                            # wave 记录收集
        p2 = FrameParser(on_log=lines.append,
                         on_wave=lambda r: recs.append(r))
        p2.feed(data)                        # 一次性喂入解析
        for line in lines:                   # log 文本回显到 stdout
            sys.stdout.write(line.decode('ascii', errors='replace'))
        n = w.export(recs)                   # 全量导出 VCD
        print(f'[hub] offline done: {n} records -> {args.vcd}')
        return

    # ----- 实时模式：启动 GUI（阻塞到窗口关闭）-----
    app = run_gui(args, modules, features)
    sys.exit(app.exec())                     # 进入 Qt 事件循环


if __name__ == '__main__':
    main()
