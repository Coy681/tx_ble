#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""debug_hub_qt.py - tx_ble 调试平台主程序（PySide6 界面版）。

整体流程：
    固件 UART --> [A5][type][len][payload] 帧流
        type 0x00: log 文本   --> 左栏（HTML 着色、关键词过滤）
        type 0x01: wave 记录  --> 右栏原始流监视 + 缓冲区
                                 [surfer] 按钮 -> 全量导出 live.vcd -> Surfer.exe

wave 记录定长 12 字节（小端）：[u32 时间戳][u32 id][u32 值]
id 位布局 = [type(1)|module(7)|feature(8)|evt(8)|instance(4)|rsvd(4)]：
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
# 匹配各 feature 的 evt 子表 "#define WAVE_XXX_EVTS(X) ...
# X(SCHED, STIMER, WINDOW, 0x01)"
# 边界：下一个 #define、紧跟的 WAVE_EVT_ENUM、或文件结尾
EVT_LIST_RE = re.compile(
    r'#define\s+WAVE_\w+_EVTS\s*\(X\)(.*?)(?=\n#define|\nWAVE_EVT_ENUM|\Z)', re.S)
# 匹配 evt 表项 X(模块短名, feature短名, evt短名token, 值)；
# token 小写后即 host 显示名（固件/host 单列双用）
EVT_ITEM_RE = re.compile(
    r'X\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')


def _num(tok):
    """把 '0x01' / '1' 之类的字面量转成 int（int(tok,0) 自动识别进制）。"""
    return int(tok, 0)


def _strip_comments(text):
    """剥掉 C 注释（/* 块注释 */ 和 // 行注释），避免注释里的示例干扰正则。"""
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)   # 块注释（跨行，re.S 让 . 匹配换行）
    text = re.sub(r'//[^\n]*', '', text)                # 行注释（到行尾）
    return text


def load_map(root):
    """扫描 root 下所有 .h 头文件，grep 出 wave id 的模块/feature/evt 注册表。

    返回三个字典：
      modules  = {模块号: (短名, 显示名)}      例 {0x01: ('SCHED','scheduler')}
      features = {(模块号,feature号): (模块短名, feature短名)}
                 例 {(0x03,0x01): ('ADV','LEGACY')}
      events   = {(模块号,feature号,evt号): 显示名(=token小写)}
                 例 {(0x01,0x01,0x01): 'window'}
    设计要点：上位机与固件读同一份 X-macro 源 -> 改打点不用同步两边版本。
    """
    import pathlib                       # 延迟导入：只在需要扫描时加载
    modules, features, events = {}, {}, {}
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
                    # 四列 X(mod, feat, TOKEN, 值) 是 evt 表项特征：
                    # 出现在 _FEATS 块里说明表名后缀写错（应为 _EVTS），
                    # 显式告警而非静默漏收（用户配置错要可见）
                    if re.search(
                            r'X\(\s*\w+\s*,\s*\w+\s*,\s*\w+\s*,'
                            r'\s*(0x[0-9A-Fa-f]+|\d+)\s*\)', line):
                        print(f'[hub] WARN: 4-col evt entry inside _FEATS '
                              f'block (name should end _EVTS): '
                              f'{line.strip()[:60]}', flush=True)
                    continue
                mod_short, feat_short, feat_val = fm.groups()
                # 用模块短名反查模块号（X(...) 里只有短名，没有模块号）
                mod_id = next((k for k, (s, _l) in modules.items()
                               if s == mod_short), None)
                if mod_id is not None:
                    # 记录 (模块号, feature号) -> (模块短名, feature短名)
                    features[(mod_id, _num(feat_val))] = (mod_short, feat_short)
        for m in EVT_LIST_RE.finditer(text):             # 最后收 evt 子表
            for line in m.group(1).splitlines():
                em = EVT_ITEM_RE.search(line)
                if not em:
                    continue                             # 该行不是 evt 表项
                mod_short, feat_short, evt_short, evt_val = em.groups()
                mod_id = next((k for k, (s, _l) in modules.items()
                               if s == mod_short), None)
                if mod_id is None:
                    continue
                # feature 短名反查 feature 号（复用已收的 features 表）
                feat_id = next((fv for (mi, fv), (ms, fs) in features.items()
                                if mi == mod_id and fs == feat_short), None)
                if feat_id is not None:
                    # token 即显示名（小写），如 WINDOW -> window
                    events[(mod_id, feat_id, _num(evt_val))] = \
                        vcd_safe(evt_short.lower())
    return modules, features, events


def lane_name(typ, mod, feat, evt, modules, features, events):
    """evt 值 -> 信号道名：查 EVTS 表得语义名，查不到降级 s<evt>。

    LEVEL -> 裸语义名（如 'window'）；NUM -> 语义名 + '_num'；
    evt=0xFF 恒为 probe（feature 级探针，保留语义不进表）。
    结果经 vcd_safe() 消毒：label 是自由文本，可能含非法字符。
    """
    if evt == EVT_PROBE:
        return vcd_safe('probe')
    label = events.get((mod, feat, evt))
    if label is None:
        raw = f's{evt}' if typ == 1 else f's{evt}_num'
    else:
        raw = label if typ == 1 else f'{label}_num'
    return vcd_safe(raw).lower()


def vcd_safe(name):
    """把自由文本名字消毒成 VCD/Surfer 合法标识符。

    VCD 标识符要求可打印 ASCII 且不能含空白；Surfer 实测对空格、
    制表等直接拒绝（wave_source 解析错误）。规则：空白->下划线，
    其余保留（token 来的名字是合法标识符，本函数只是防御层）。
    """
    name = name.replace('"', "'")
    return '_'.join(name.split()) or 'x'


def decode_id(w):
    """把 32 位 id 拆成五元组 (type, module, feature, instance, evt)。

    id = [type(1)|module(7)|feature(8)|evt(8)|instance(4)|rsvd(4)]
      bit31   bit30-24    bit23-16     bit15-8    bit7-4   bit3-0
    注意 module 用 0x7F 掩码：bit31 是 type，不能混进模块号。
    rsvd（bit3-0）目前忽略。
    """
    return ((w >> 31) & 1, (w >> 24) & 0x7F, (w >> 16) & 0xFF,
            (w >> 4) & 0x0F, (w >> 8) & 0xFF)


def track_name(mod, feat, inst, evt, modules, features, events=None,
               typ=None):
    """把 id 五元组翻译成人类可读的轨道名字符串。

    例：scheduler/STIMER/i2/window（模块/feature/实例/事件道）
    evt 查 EVTS 表得语义名（typ=1 用裸名，typ=0 加 _num）；查不到降级
    s<evt>；未知 id 降级为数字命名，保证任何 id 都能显示。
    """
    # 查不到就用数字占位（m 元组 = (短名, 显示名)，这里取显示名）
    m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))
    f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))
    parts = [m[1], f[1]]                 # 第 1、2 级：模块名/feature 名
    if inst:
        parts.append(f'i{inst}')         # 第 3 级：运行时实例号（0 则省略）
    if events is not None and typ is not None:
        parts.append(lane_name(typ, mod, feat, evt, modules, features,
                               events))  # 第 4 级：语义道名（含降级）
    else:
        parts.append('probe' if evt == EVT_PROBE else f's{evt}')
    return '/'.join(parts)               # 斜杠连接成层级路径


# ================= 波形导出（VCD）=================

def collect_tracks(records, modules, features, events):
    """第一遍扫描：从记录集里收集"实际出现过的轨道"。

    返回集合 {(模块显示名, feature显示名, 实例号, 信号道名, 位宽)}：
      LEVEL -> (…, evt语义名/'s<evt>', 1)   1-bit 电平线
      NUM   -> (…, evt语义名_num/'s<evt>_num', 32)  32-bit 数值线
    用途：VCD 只声明出现过的信号，Surfer 面板无空占位（用户要求）。
    """
    tracks = set()                       # set 自动去重：同轨道只留一份
    for ts, wid, val in records:
        typ, mod, feat, inst, evt = decode_id(wid)
        m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))   # 模块查表（含降级）
        f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))  # feature 查表
        lane = lane_name(typ, mod, feat, evt, modules, features, events)
        tracks.add((m[1], f[1].lower(), inst, lane, 1 if typ == 1 else 32))
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

    def __init__(self, path, modules, features, events):
        self.path = path                 # 输出文件路径（通常 live.vcd）
        self.modules = modules           # 模块映射表（load_map 产出）
        self.features = features         # feature 映射表
        self.events = events             # evt 映射表（load_map 产出）

    def export(self, records):
        """两遍写法：第一遍收集轨道并声明，第二遍按时间序写值变更。"""
        tracks = collect_tracks(records, self.modules, self.features,
                                self.events)
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
                # 拖入波形面板后只显示信号名，scope 层级会丢 -> 信号名
                # 带全路径（module_feature_lane），多模块对比不用回树里看
                full = f'{mod_l}_{feat_l}_{lane.replace("/", "_")}'
                # 声明信号：$var wire <位宽> <符号> $<名字> $end
                f.write(f'$var wire {width} {sym} ${full} $end\n')
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
                                       (m[0], f'feat{feat:#x}'))[1].lower()
                key = (m[1], fl, inst,
                       lane_name(typ, mod, feat, evt, self.modules,
                                 self.features, self.events))
                if typ == 1:                 # LEVEL：标量 0/1（无空格，紧贴 id）
                    v = '1' if val else '0'
                else:                        # NUM：32bit 向量，b 与 id 间必须有空格
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

        def __init__(self, port, baud, raw_dump, parity='N', flow=''):
            super().__init__()
            self.port = port         # 串口号字符串（如 'COM7'）
            self.baud = baud         # 波特率 int
            self.raw_dump = raw_dump # 调试开关：是否把原始字节 dump 到 stderr
            self.parity = parity     # 校验位 'N'/'E'/'O'（setup 面板可配）
            self.flow = flow         # 流控 ''/'rtscts'/'xonxoff'/'dsrdtr'
            self._stop = False       # 停止标志（GUI 断开时置 True）
            self.ser = None          # 串口对象（run() 里创建）
            self.raw = bytearray()   # 本次连接的原始字节流（快照归档用）
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
                self.ser = serial.Serial(
                    self.port, self.baud, timeout=0.05,
                    parity=self.parity,
                    xonxoff=(self.flow == 'xonxoff'),
                    rtscts=(self.flow == 'rtscts'),
                    dsrdtr=(self.flow == 'dsrdtr'))
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
                    self.raw += data               # 原始流缓存（快照用）
                    if self.raw_dump:            # 调试：原始十六进制流
                        n += len(data)
                        sys.stderr.write(f'[raw {n:7d}B] {data[:64].hex(" ")}\n')
                    self.parser.feed(data)       # 喂帧解析器（同步回调 emit 信号）
            self.ser.close()                     # 释放串口（否则占用导致重连失败）
            self.ser = None

    return SerialWorker


# ================= GUI =================

# 浅色主题 QSS（VS Code Light+ 风格：#f3f3f3 底 / #e8e8e8 面板 / #007acc 强调蓝）
LIGHT = """
QWidget { background: #f3f3f3; color: #333333; font-size: 13px; }   /* 全局底色 */
QPlainTextEdit, QTreeWidget { background: #ffffff; color: #333333;  /* 文本/树面板 */
                 font-family: Consolas; font-size: 12px; border: 1px solid #e0e0e0; }
QTreeWidget::item { height: 22px; }                                  /* 树行加高可读 */
QTreeWidget::item:selected { background: #cce5ff; color: #1e1e1e; }
QHeaderView::section { background: #f0f0f0; color: #616161;          /* 表头 */
                       border: none; border-bottom: 1px solid #e0e0e0;
                       padding: 3px 6px; }
QLineEdit, QComboBox { background: #ffffff; border: 1px solid #cecece;
                       border-radius: 4px; padding: 3px 6px;
                       selection-background-color: #add6ff; }       /* 输入框/下拉 */
QComboBox QAbstractItemView { background: #ffffff; color: #333;
                              selection-background-color: #cce5ff; }
QPushButton { background: #e4e6e9; border: 1px solid #c9c9c9;        /* 按钮常态：浅灰
              （白面板上白按钮无层次，统一压灰一档） */
              border-radius: 4px; padding: 4px 14px; color: #333; }
QPushButton:hover { background: #d5d7db; border-color: #9a9a9a; }   /* 悬停变深 */
QPushButton:pressed { background: #007acc; color: white; }          /* 按下强调蓝 */
QPushButton:checked { background: #007acc; color: white; }          /* 可勾选按钮(hex) */
QPushButton:disabled { color: #aaaaaa; background: #ededed; }       /* 禁用态更浅 */
QSplitter::handle { background: #e0e0e0; width: 3px; }              /* 分屏拖动条 */
QStatusBar { background: #f0f0f0; color: #616161;                   /* VS Code 蓝状态栏底改浅 */
             border-top: 1px solid #e0e0e0; }
QStatusBar QLabel { color: #616161; padding: 0 8px; }               /* 状态指标分段 */
QLabel#pill_conn { border-radius: 8px; padding: 1px 10px;           /* 连接状态药丸 */
                   font-weight: bold; }
QToolTip { background: #ffffe8; color: #333; border: 1px solid #cecece; }
"""

LOG_MAX_BLOCKS = 20000      # log **显示层**最大行数（QPlainTextEdit 上限，
                            # 保证 HTML 渲染性能；更早历史在留底/会话文件）
LOG_STORE_MAX = 0           # log 留底上限（0=无限）。默认无限：约 120B/行，
                            # 50 行/s 跑 8 小时 ≈ 170MB，可接受；内存紧张时
                            # 用 --log-max 设上限（显示/落盘不受影响）

# HTML 着色模板（appendHtml 用；浅底深色系，对齐 VS Code Light 语义色）
STYLE_ERR = '<span style="color:#c93030">%s</span>'    # 红：错误（cd3131 变体）
STYLE_WARN = '<span style="color:#b8800a">%s</span>'   # 橙/黄：警告
STYLE_OK = '<span style="color:#1a9a4b">%s</span>'     # 绿：成功
STYLE_LVL = '<span style="color:#005cb8">%s</span>'    # 蓝：LEVEL 门记录
STYLE_NUM = '<span style="color:#8a5200">%s</span>'    # 深琥珀：NUM 数值记录
STYLE_TS = '<span style="color:#9b9b9b">%s</span>'     # 浅灰：本机时间戳

# sparkline：LEVEL 门最近窗口相对宽度字符画（8 格条形图）
SPARK_BLOCKS = 8
SPARK_FULL = '\u2593'          # ▓
SPARK_EMPTY = '\u2591'         # ░
GATE_OPEN_LIMIT = 50_000       # 预留：门开启超过 50ms tick 视为悬空阈值

# WAVE MONITOR 表格列
COL_VAL, COL_AGO, COL_SPARK, COL_TRACK = range(4)


def run_gui(args, modules, features, events):
    """构建并运行 Qt 界面（阻塞直到窗口关闭）。

    页面形态（v1.0）：
        工具栏两行：连接/暂停/录制/快照 | 发送/过滤/导出
        三栏：LOG(40%) | WAVE MONITOR(35%, 每轨一行+sparkline) | TRACKS(25%)
        状态栏：recs | KB/s | gate 异常 | 快照提示
    """
    from PySide6.QtCore import Qt, QTimer, QDateTime
    from PySide6.QtGui import QColor, QFont
    from PySide6.QtWidgets import (
        QApplication, QMainWindow, QWidget, QPlainTextEdit, QHBoxLayout,
        QVBoxLayout, QPushButton, QComboBox, QLabel, QLineEdit, QSplitter,
        QTreeWidget, QTreeWidgetItem, QAbstractItemView)

    from PySide6.QtGui import QColor, QFont, QIcon

    import serial.tools.list_ports   # COM 口枚举

    SerialWorker = make_worker(None) # 生成线程类（延迟 Qt 绑定）

    # 运行时窗口图标（任务栏/Alt-Tab）：exe 内嵌图标只管文件资源管理器，
    # Qt 窗口必须显式 setWindowIcon，否则下边栏显示默认灰图标
    # 图标查找顺序：PyInstaller 解包目录(--add-data 嵌入) -> exe 同目录 ->
    # 脚本目录（源码布局）
    if getattr(sys, 'frozen', False):
        _icon_cands = [os.path.join(getattr(sys, '_MEIPASS', ''), 'htx.ico'),
                       os.path.join(os.path.dirname(sys.executable),
                                    'htx.ico')]
    else:
        _icon_cands = [os.path.join(
            os.path.dirname(os.path.abspath(__file__)), 'htx.ico')]
    _icon_path = next((p for p in _icon_cands if os.path.isfile(p)), '')
    # AppUserModelID：Windows 任务栏按进程归属显示图标，python.exe 启动时
    # 下边栏显示 Python 图标。设置独立应用身份后任务栏改用窗口图标
    try:
        import ctypes
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(
            'amlogic.tx_ble.debug_hub')
    except Exception:
        pass
    app = QApplication([sys.argv[0]] if getattr(sys, 'frozen', False)
                       else sys.argv)  # QApplication(sys.argv) 对 exe argv 敏感
    if _icon_path:
        app.setWindowIcon(QIcon(_icon_path))

    app.setStyleSheet(LIGHT)         # 应用浅色主题（VS Code Light+ 风格）
    win = QMainWindow()              # 主窗口
    win.setWindowTitle('tx_ble debug hub')
    if _icon_path:
        win.setWindowIcon(QIcon(_icon_path))
    win.resize(1200, 680)            # 默认窗口尺寸（适配 1280x800 主屏）

    central = QWidget()              # 中央容器（装工具栏 + 分屏）
    win.setCentralWidget(central)
    vbox = QVBoxLayout(central)      # 垂直布局：两行工具栏、下分屏
    vbox.setContentsMargins(6, 6, 6, 4)
    vbox.setSpacing(4)

    # ---------------- 顶部工具栏：仅全局操作（连接/暂停/录制/快照/全清） ----------------
    def fit_btn(b, pad=12):
        """按钮宽度按内容自适应（固定像素会在不同 DPI/字体下裁字）。"""
        b.setMinimumWidth(b.minimumSizeHint().width() + pad)

    bar1 = QHBoxLayout()
    bar1.setSpacing(6)
    # UART 配置折叠：默认只显示一个 ⚙ 按钮，点开弹出配置面板
    # （port/baud/parity/flow 全收进面板，顶部留白给其他按钮）
    btn_setup = QPushButton('\u2699 uart')        # ⚙ 折叠的串口配置入口
    btn_setup.setCheckable(True)
    btn_setup.setToolTip('UART settings (port/baud/parity/flow)')
    fit_btn(btn_setup, 10)
    cb_port = QComboBox()            # 串口选择下拉
    cb_port.setEditable(True)
    cb_port.setFixedWidth(95)
    cb_port.addItems([p.device for p in serial.tools.list_ports.comports()])
    if args.port:
        cb_port.setCurrentText(args.port)
    cb_baud = QComboBox()
    cb_baud.setEditable(True)
    cb_baud.addItems(['9600', '115200', '460800', '921600',
                      '1000000', '2000000'])
    cb_baud.setCurrentText(str(args.baud))
    cb_parity = QComboBox()          # 校验位（默认 N，与固件一致）
    cb_parity.addItems(['N', 'E', 'O'])
    cb_flow = QComboBox()            # 流控（默认无）
    cb_flow.addItems(['none', 'rtscts', 'xonxoff', 'dsrdtr'])
    cfg_row = QWidget()              # 折叠配置面板（默认隐藏）
    cfg_h = QHBoxLayout(cfg_row)
    cfg_h.setContentsMargins(24, 0, 0, 0)
    cfg_h.setSpacing(6)
    for lbl, wdg in (('port:', cb_port), ('baud:', cb_baud),
                     ('parity:', cb_parity), ('flow:', cb_flow)):
        cfg_h.addWidget(QLabel(lbl))
        cfg_h.addWidget(wdg)
    cfg_h.addStretch(1)
    cfg_row.setVisible(False)        # 折叠态

    btn_conn = QPushButton('connect')
    fit_btn(btn_conn)
    lbl_conn = QLabel('disconnected')
    lbl_conn.setObjectName('pill_conn')           # 药丸样式（QSS #pill_conn）
    lbl_conn.setStyleSheet('background:#c8c8c8;color:#666666;')

    btn_pause = QPushButton('\u23f8')             # ⏸ 冻结滚动显示（数据不丢）
    btn_pause.setCheckable(True)
    btn_pause.setToolTip('pause display (data keeps buffering)')
    btn_pause.setEnabled(False)                   # 未连接前禁用
    btn_rec = QPushButton('\u2b55 record')        # ⏺ 原始流录制
    btn_rec.setCheckable(True)
    btn_rec.setToolTip('record raw stream to file (replayable)')
    btn_snap = QPushButton('\U0001F4F8 snapshot') # 📸 会话快照归档
    btn_clear = QPushButton('clear all')          # 全局清空（log+wave）

    # 按钮宽度一律按内容自适应（固定像素会在不同 DPI/字体下裁字），
    # 统一给 12px 余量；窄按钮（⏸/tx 等）单独收紧为 8px
    for b in (btn_pause, btn_rec):
        b.setMinimumWidth(b.minimumSizeHint().width() + 8)
    for b in (btn_snap, btn_clear):
        b.setMinimumWidth(b.minimumSizeHint().width() + 12)

    bar1.addWidget(btn_setup)
    bar1.addWidget(btn_conn)
    bar1.addWidget(lbl_conn)
    bar1.addSpacing(12)
    bar1.addWidget(btn_pause)
    bar1.addWidget(btn_rec)
    bar1.addWidget(btn_snap)
    bar1.addWidget(btn_clear)
    bar1.addStretch(1)               # 右侧弹性：其余控件靠左成组
    vbox.addLayout(bar1)
    vbox.addWidget(cfg_row)          # 配置面板（⌂ 展开时可见）

    # ---------------- 面板头部条（操作归属各自面板，不再挤顶部） ----------------
    def panel_bar(*items, stretch=None):
        """构造一个面板头部条：浅底圆角横条，放该面板专属操作。

        stretch=某个 item 时该项吃满剩余宽度（如过滤框/发送框）。
        """
        w_ = QWidget()
        h = QHBoxLayout(w_)
        h.setContentsMargins(4, 2, 4, 2)
        h.setSpacing(6)
        for it in items:
            h.addWidget(it, 1 if it is stretch else 0)
        if stretch is None:
            h.addStretch(1)
        # 选择器限定：裸 background 声明会被子控件继承（冲淡按钮
        # hover/checked 态），必须用 #paneBar 选择器只作用容器自身
        w_.setObjectName('paneBar')
        w_.setStyleSheet(
            'QWidget#paneBar { background:#eaeaea;'
            'border:1px solid #e0e0e0; border-radius:4px; }')
        return w_

    # LOG 面板：过滤框（头部）+ 发送框（底部，聊天式语境）
    edt_filter = QComboBox()         # 过滤框：re: 前缀 = 正则，否则子串
    edt_filter.setEditable(True)
    edt_filter.setMinimumWidth(180)
    edt_filter.lineEdit().setPlaceholderText("filter ('re:' = regex)")
    lbl_filter = QLabel('')          # 过滤命中计数（n/m = 命中/留底总量）
    btn_clear_log = QPushButton('clear')
    fit_btn(btn_clear_log, 14)
    btn_clear_log.setToolTip('clear log pane')
    btn_save_log = QPushButton('save')
    fit_btn(btn_save_log, 14)
    btn_save_log.setToolTip('save full log (from backing store) to file')
    log_head = panel_bar(QLabel('LOG'), edt_filter, lbl_filter,
                         btn_save_log, btn_clear_log, stretch=edt_filter)
    ent_send = QLineEdit()
    ent_send.setPlaceholderText('send to board... (Enter=tx)')
    cb_hex = QPushButton('hex')
    cb_hex.setCheckable(True)
    fit_btn(cb_hex, 14)
    # 选中态高对比：强调蓝底白字 + 深蓝边框（与常态浅灰明显区分）
    cb_hex.setStyleSheet(
        'QPushButton:checked { background:#007acc; color:white;'
        ' border:1px solid #005a9e; font-weight:bold; }'
        'QPushButton:checked:hover { background:#0086c8; }')
    btn_tx = QPushButton('tx')
    fit_btn(btn_tx, 14)
    log_foot = panel_bar(ent_send, cb_hex, btn_tx, stretch=ent_send)

    # WAVE MONITOR 面板：surfer 导出 + 自己的 clear（统计在底部状态栏右段）
    btn_surfer = QPushButton('surfer \u25b6')     # 导出勾选轨道 -> Surfer
    fit_btn(btn_surfer)
    btn_clear_wave = QPushButton('clear')
    fit_btn(btn_clear_wave, 14)
    btn_clear_wave.setToolTip('clear wave records + monitor rows')
    lbl_wave = QLabel('wave: 0')
    btn_save_wave = QPushButton('save')
    fit_btn(btn_save_wave, 14)
    btn_save_wave.setToolTip('save wave records (12B binary) to file')
    btn_load_wave = QPushButton('import')
    fit_btn(btn_load_wave, 14)
    btn_load_wave.setToolTip(
        'load a saved wave_*.bin (or snapshot wave.bin) for replay;\n'
        'records feed MONITOR/TRACKS, then check tracks + surfer as usual')
    btn_rescan = QPushButton('rescan')
    fit_btn(btn_rescan, 14)
    btn_rescan.setToolTip(
        're-grep firmware headers for module/feature/evt names after\n'
        'editing X-macro tables; MONITOR/TRACKS rebuild with new names,\n'
        'existing records are replayed (no data loss)')
    mon_head = panel_bar(QLabel('WAVE'), btn_surfer, btn_load_wave,
                         btn_save_wave, btn_rescan,
                         btn_clear_wave, lbl_wave)

    # TRACKS 面板：全选/全不选（勾选 = surfer 导出范围）
    btn_sel_all = QPushButton('all')
    fit_btn(btn_sel_all, 14)
    btn_sel_all.setToolTip('check all tracks')
    btn_sel_none = QPushButton('none')
    fit_btn(btn_sel_none, 14)
    btn_sel_none.setToolTip('uncheck all tracks')
    trk_head = panel_bar(QLabel('TRACKS'), btn_sel_all, btn_sel_none)

    # ---------------- 主区：左 LOG 通高 | 右上 WAVE / 右下 TRACKS ----------------
    # LOG 行长（时间戳+文本）横向空间最值钱 -> 单独一列通高；
    # WAVE 4 列表与 TRACKS 树都是"宽扁"内容 -> 上下分各拿完整宽度，
    # 且导出选择（下）与数据流（上）视线近：勾选完马上 surfer。
    split_h = QSplitter(Qt.Horizontal)   # 一级：左 | 右
    split_v = QSplitter(Qt.Vertical)     # 二级：右上 WAVE / 右下 TRACKS
    txt_log = QPlainTextEdit()       # 左：log（带本机时间戳 + 过滤）
    txt_log.setReadOnly(True)
    txt_log.setMaximumBlockCount(LOG_MAX_BLOCKS)
    txt_log.setFont(QFont('Consolas', 9))

    mon = QTreeWidget()              # 右上：WAVE MONITOR 每轨一行实时摘要
    mon.setHeaderLabels(['value', 'ago', 'gate', 'track'])
    mon.setRootIsDecorated(True)     # 两层树：组行可折叠（防轨道多时爆屏）
    mon.setUniformRowHeights(True)
    mon.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)
    mon.setVerticalScrollMode(QAbstractItemView.ScrollPerItem)  # 平滑滚动
    mon.setColumnWidth(COL_VAL, 90)
    mon.setColumnWidth(COL_AGO, 80)
    mon.setColumnWidth(COL_SPARK, 110)
    mon.setFont(QFont('Consolas', 9))

    tree_tracks = QTreeWidget()      # 右下：TRACKS 信号树（勾选决定导出范围）
    tree_tracks.setHeaderLabels(['track', ''])
    tree_tracks.setColumnWidth(0, 260)

    # 每栏 = 头部条 + 内容（+ 底部条），用容器包住再进 splitter
    def pane(head, body, foot=None):
        w_ = QWidget()
        v = QVBoxLayout(w_)
        v.setContentsMargins(0, 0, 0, 0)
        v.setSpacing(2)
        v.addWidget(head)
        v.addWidget(body, 1)
        if foot is not None:
            v.addWidget(foot)
        return w_
    pane_log = pane(log_head, txt_log, log_foot)
    pane_mon = pane(mon_head, mon)
    pane_trk = pane(trk_head, tree_tracks)
    split_v.addWidget(pane_mon)      # 右上：wave
    split_v.addWidget(pane_trk)      # 右下：tracks
    split_v.setSizes([400, 260])     # 右侧上下约 6/4（wave 是主观察区）
    split_h.addWidget(pane_log)      # 左：log 通高
    split_h.addWidget(split_v)       # 右：上下两窗
    split_h.setSizes([560, 640])     # 左右约 45/55（右列要放两窗）
    vbox.addWidget(split_h, 1)

    # ---------------- 状态栏（信息带：左 log 统计 | 中 全局提示 | 右 wave 统计） ----------------
    lbl_stat_log = QLabel('log: 0 lines')        # 左段：log 留底统计
    lbl_stat_vcd = QLabel('live.vcd -')          # 中段：文件操作提示
    lbl_wave_stat = QLabel('wave: 0')            # 右段：wave 统计（recs/KB/s/gate）
    lbl_stat_bw = QLabel('0 KB/s')
    lbl_stat_gate = QLabel('gate ok')
    win.statusBar().addWidget(lbl_stat_log)      # 左
    win.statusBar().addWidget(lbl_stat_vcd)      # 中
    win.statusBar().addWidget(QLabel(), 1)       # 弹性区把 wave 组推到右侧
    win.statusBar().addWidget(lbl_wave_stat)     # 右
    win.statusBar().addWidget(lbl_stat_bw)
    win.statusBar().addWidget(lbl_stat_gate)
    win.statusBar().addWidget(
        QLabel(f'id map: {len(modules)} modules, {len(features)} features, '
               f'{len(events)} evts'))

    # ---------------- 状态与回调 ----------------
    records = []                     # 全部 wave 记录（surfer 导出用）
    worker = [None]                  # 当前串口线程（列表模拟可变闭包变量）
    track_items = {}                 # TRACKS 树叶子：key -> 叶节点
    mon_rows = {}                    # MONITOR 道：key -> (道行节点, 状态dict)
    mon_groups = {}                  # MONITOR 组行：(模块,feat,实例) -> 组行
    track_state = {}                 # 门摘要：key -> {open_ts,win,wins[]}
    pending_log = []                 # 暂停期间积压的 log 行
    log_store = []                   # log 留底（过滤重放 + 快照完整导出用）
    log_sess = [None, 0]             # [会话文件句柄, 行计数]：小时级保真
    filter_re = [None]               # 当前过滤正则（None = 不过滤）
    filter_sub = ['']                # 当前过滤子串
    gate_bad = [0]                   # 悬空门计数（状态栏）
    byte_count = [0, 0]              # KB/s 统计：[上次字节, 上次时刻]
    rec_file = [None]                # 录制文件句柄

    def now_hms():
        """本机时间戳 HH:MM:SS.mmm（log 行前缀）。"""
        return QDateTime.currentDateTime().toString('HH:mm:ss.zzz')

    def track_key(wid):
        """32 位 id -> VCD 声明用的轨道 key（与 GzVcdWriter key 一致）。"""
        typ, mod, feat, inst, evt = decode_id(wid)
        m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))
        f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))
        lane = lane_name(typ, mod, feat, evt, modules, features, events)
        return (m[1], f[1].lower(), inst, lane)

    def child_of(parent, text):
        """在 parent 下找文本为 text 的子节点，没有就新建。"""
        for i in range(parent.childCount()):
            c = parent.child(i)
            if c.text(0) == text:
                return c
        c = QTreeWidgetItem([text])
        parent.addChild(c)
        return c

    def ensure_track_item(key):
        """TRACKS 树懒构建：模块 > feature/实例 > 道名，叶子带勾选框。

        默认全部折叠（几百 feature 时列表不爆屏）：模块行可见可点，
        新轨道挂入不打断当前浏览位置；想看哪个组手动展开。
        """
        if key in track_items:
            return track_items[key]
        mod_l, feat_l, inst, lane = key
        mi = child_of(tree_tracks.invisibleRootItem(), mod_l)
        fi = child_of(mi, feat_l + (f'/i{inst}' if inst else ''))
        leaf = QTreeWidgetItem([lane])
        leaf.setCheckState(0, Qt.Checked)     # 默认全选 = 与旧版全量导出一致
        fi.addChild(leaf)
        fi.setText(1, f'({fi.childCount()})')  # feature 行显示道数
        track_items[key] = leaf
        return leaf

    def ensure_mon_row(key, is_level):
        """WAVE MONITOR 两层树懒构建（防几百轨道时平铺爆屏）：

        第 1 层 = 组行 `module/feature/iN`（汇总：最近值/ago/活跃道数）
        第 2 层 = 各信号道 lane（value/ago/spark/全名）
        组行默认折叠，点击展开只看关心的组。
        """
        mod_l, feat_l, inst, lane = key
        gname = '/'.join([mod_l, feat_l] + ([f'i{inst}'] if inst else []))
        group_key = (mod_l, feat_l, inst)
        if group_key not in mon_groups:
            g = QTreeWidgetItem(['', '', '', gname])
            g.setForeground(COL_VAL, QColor('#616161'))
            g.setFont(COL_TRACK, QFont('Consolas', 9, QFont.Bold))
            mon.addTopLevelItem(g)
            mon_groups[group_key] = g
        g = mon_groups[group_key]
        if key in mon_rows:
            return mon_rows[key]
        row = QTreeWidgetItem(['-', '-', '', f'{gname}/{lane}'])
        if is_level:
            row.setForeground(COL_VAL, QColor('#005cb8'))   # LEVEL：深蓝
        else:
            row.setForeground(COL_VAL, QColor('#8a5200'))   # NUM：深琥珀
        g.addChild(row)
        g.setText(COL_TRACK, f'{gname} ({g.childCount()})')  # 组内道数
        mon_rows[key] = (row, {'last_ts': None, 'open_ts': None,
                               'wins': []})
        return mon_rows[key]

    def spark(wins):
        """最近 8 个门窗口的相对宽度字符画。输入为窗口长度列表。"""
        if not wins:
            return ''
        recent = wins[-SPARK_BLOCKS:]
        hi = max(recent) or 1
        return ''.join(
            SPARK_FULL if w * 2 > hi else
            (SPARK_EMPTY if w * 8 <= hi else '\u2592')   # 中档用 ▒
            for w in recent)

    def esc(s):
        """HTML 转义（& < >），防止 log 文本破坏着色标签。"""
        return (s.replace('&', '&amp;').replace('<', '&lt;')
                .replace('>', '&gt;'))

    def match_filter(line):
        """过滤匹配：filter_re 优先（re: 前缀），否则子串。空 = 全通过。"""
        if filter_re[0] is not None:
            return filter_re[0].search(line) is not None
        if filter_sub[0]:
            return filter_sub[0].lower() in line.lower()
        return True

    def append_log(line):
        """log 行渲染流水线：着色 + 本机时间戳前缀 + 过滤。

        纯渲染函数（不写入 log_store），实时追加与历史重放共用。
        wall 为该行的本机时间戳字符串（留底时生成，重放时原样复用）。
        """
        text = line.rstrip('\r\n')
        if not match_filter(text):
            return False
        low = text.lower()
        if 'err' in low or 'fail' in low:
            style = STYLE_ERR
        elif 'warn' in low:
            style = STYLE_WARN
        elif 'ok' in low or 'ready' in low or 'done' in low:
            style = STYLE_OK
        else:
            style = '%s'
        ts_html = STYLE_TS % f'[{now_hms()}]'
        txt_log.appendHtml(f'{ts_html} ' + style % esc(text))
        return True

    def on_log_line(line):
        """log 信号槽：落盘 -> 入留底 -> 渲染（暂停时积压不渲染）。

        三层存储，容量策略不同：
          session 文件 = 无限（小时级保真，每条带本机时间戳落盘）
          log_store    = 留底（默认无限；--log-max 可设上限省内存）
          txt_log 面板 = 最近 LOG_MAX_BLOCKS 行（显示层，重放也只画这段）
        """
        text = line.rstrip('\r\n')
        if log_sess[0] is not None:              # 会话文件：小时级保真
            try:
                log_sess[0].write(f'[{now_hms()}] {text}\n')
                log_sess[1] += 1
                if log_sess[1] % 20 == 0:        # 每 20 行 flush 一次
                    log_sess[0].flush()
            except OSError:
                pass                             # 磁盘故障不打断采集
        log_store.append(text)
        if LOG_STORE_MAX and len(log_store) > LOG_STORE_MAX:
            del log_store[:len(log_store) - LOG_STORE_MAX]
        if btn_pause.isChecked():
            pending_log.append(line)
            return
        if append_log(text) and (filter_re[0] is not None or filter_sub[0]):
            update_log_counter()     # 过滤生效时命中数随新行实时更新

    def replay_log():
        """按当前过滤条件全量重建 log 面板（过滤历史行）。

        只重画**显示窗口**（最近 LOG_MAX_BLOCKS 条）：留底可能是几小时
        几十万行，全量画 HTML 会卡死 UI；更早的历史走 session 文件/save。
        """
        txt_log.clear()
        for text in log_store[-LOG_MAX_BLOCKS:]:
            append_log(text)
        update_log_counter()

    def update_log_counter():
        """log 统计双写：底部状态栏（留底总量）+ LOG 头部（过滤命中 n/m）。

        无过滤时头部只显 'ok'，总量看底部（避免两处显示同一数字）。
        """
        lbl_stat_log.setText(f'log: {len(log_store)} lines')
        if filter_re[0] is not None or filter_sub[0]:
            n_hit = sum(1 for t in log_store if match_filter(t))
            lbl_filter.setText(f'{n_hit}/{len(log_store)}')
        else:
            lbl_filter.setText('')

    def on_filter_changed(text):
        """过滤框变化：解析 re: 前缀，防抖后全量重放（历史也过滤）。"""
        t = text.strip()
        if t.startswith('re:'):
            try:
                filter_re[0] = re.compile(t[3:])
                filter_sub[0] = ''
                lbl_filter.setText('regex ok')
            except re.error as e:
                filter_re[0] = None
                lbl_filter.setText(f're bad: {e}')
                return
        else:
            filter_re[0] = None
            filter_sub[0] = t
        filter_timer.start(300)      # 停手 300ms 才重放，打字不卡

    def _apply_wave(rec, is_new):
        """wave 记录处理核心：MONITOR 行更新 + 悬空门检测。

        is_new=False 时（rescan 重放）不重复入 records 缓冲、
        不刷新 wave 计数（计数没变）。
        """
        if is_new:
            records.append(rec)
        ts, wid, val = rec
        typ, mod, feat, inst, evt = decode_id(wid)
        key = track_key(wid)
        row, st = ensure_mon_row(key, typ == 1)
        ensure_track_item(key)
        if typ == 1:                 # LEVEL 门：开/闭窗 + 窗宽序列
            if val and st['open_ts'] is None:
                st['open_ts'] = ts
            elif not val and st['open_ts'] is not None:
                st['wins'].append(ts - st['open_ts'])
                if len(st['wins']) > 64:      # 窗宽序列限长（sparkline 用）
                    del st['wins'][:32]
                st['open_ts'] = None
            if st['open_ts'] is not None:
                # 门开着：OPEN! 红色（悬空现场证据）
                row.setText(COL_SPARK, 'OPEN!')
                row.setForeground(COL_SPARK, QColor('#c93030'))
            else:
                row.setText(COL_SPARK, spark(st['wins']))
                row.setForeground(COL_SPARK, QColor('#005cb8'))
            row.setText(COL_VAL, str(val))
        else:                        # NUM：当前值即摘要
            row.setText(COL_VAL, str(val))
            row.setText(COL_SPARK, '')
        st['last_ts'] = ts
        st['last_wall'] = QDateTime.currentMSecsSinceEpoch()  # 本机到达时刻
        if is_new:
            lbl_wave.setText(f'wave: {len(records)}')
            lbl_wave_stat.setText(f'wave: {len(records)}')

    def on_wave_record(rec):
        """wave 信号槽：入缓冲 + MONITOR 行更新 + 悬空门检测。"""
        _apply_wave(rec, True)

    def on_status(line):
        """状态消息槽：连接药丸变色提示（open failed 等）。"""
        lbl_conn.setText(line)
        lbl_conn.setStyleSheet('background:#c93030;color:white;')

    def set_conn_pill(ok, text):
        """连接状态药丸：绿=连接，灰=断开，红=错误。"""
        lbl_conn.setText(text)
        color = '#1a9a4b' if ok else '#c8c8c8'
        fg = 'white' if ok else '#666666'
        lbl_conn.setStyleSheet(f'background:{color};color:{fg};')

    def toggle_pause(checked):
        """暂停槽：冻结显示；恢复时全量重放（积压行已在留底里）。"""
        btn_pause.setText('\u25b6' if checked else '\u23f8')
        if not checked:
            replay_log()
            pending_log.clear()

    def toggle_record(checked):
        """录制槽：原始字节流落盘 raw_rec_YYYYMMDD-HHMMSS.bin。"""
        import datetime
        if checked:
            if worker[0] is None:            # 未连接不许录
                btn_rec.setChecked(False)
                return
            path = os.path.join(
                os.path.dirname(args.vcd), 'record_' +
                datetime.datetime.now().strftime('%Y%m%d-%H%M%S') + '.bin')
            rec_file[0] = open(path, 'wb')
            btn_rec.setText('\u25cf rec')    # 录制中样式（红色由定时器闪）
            btn_rec.setStyleSheet('background:#c93030;color:white;')
            lbl_stat_vcd.setText(f'rec {os.path.basename(path)}')
        else:
            if rec_file[0] is not None:
                rec_file[0].close()
                rec_file[0] = None
            btn_rec.setText('\u2b55 record')
            btn_rec.setStyleSheet('')

    def toggle_connect():
        """连接/断开按钮槽：管理串口线程生命周期 + 会话日志文件。"""
        if worker[0] is not None:    # 已连接 -> 断开
            worker[0]._stop = True
            worker[0].wait(1000)
            worker[0] = None
            if log_sess[0] is not None:      # 会话日志收尾落盘
                log_sess[0].close()
                log_sess[0] = None
            btn_conn.setText('connect')
            btn_pause.setEnabled(False)
            set_conn_pill(False, 'disconnected')
            return
        try:
            w = SerialWorker(cb_port.currentText(),
                             int(cb_baud.currentText()), args.raw_dump,
                             parity=cb_parity.currentText(),
                             flow=cb_flow.currentText()
                             if cb_flow.currentText() != 'none' else '')
        except Exception as e:
            set_conn_pill(False, f'bad config: {e}')
            return
        # 会话日志文件：每次连接一个，小时级保真（append 模式随时可断电）
        import datetime
        sess_path = os.path.join(
            os.path.dirname(args.vcd), 'logs',
            'session_' + datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
            + '_' + cb_port.currentText().replace(':', '') + '.txt')
        os.makedirs(os.path.dirname(sess_path), exist_ok=True)
        try:
            log_sess[0] = open(sess_path, 'a', encoding='utf-8',
                               errors='replace')
            log_sess[1] = 0
        except OSError as e:
            lbl_stat_vcd.setText(f'session log failed: {e}')
        # 录制模式：把落盘钩子挂进 worker 读循环（write hook）
        if rec_file[0] is not None:
            f_ref = rec_file[0]
            orig_feed = w.parser.feed

            def feed_and_record(data):
                f_ref.write(data)        # 原始字节落盘（回放=完整现场）
                orig_feed(data)
            w.parser.feed = feed_and_record
        w.log_line.connect(on_log_line)
        w.wave_rec.connect(on_wave_record)
        w.status_line.connect(on_status)
        w.start()
        worker[0] = w
        btn_conn.setText('disconnect')
        btn_pause.setEnabled(True)
        set_conn_pill(True, f'{cb_port.currentText()} @ {cb_baud.currentText()}')
        lbl_stat_vcd.setText(f'session log -> {os.path.basename(sess_path)}')

    def do_send():
        """发送槽：hex/ascii 组包写入串口（自动补 \\r\\n）。"""
        if worker[0] is None or not ent_send.text():
            return
        text = ent_send.text()
        if cb_hex.isChecked():
            try:
                payload = bytes.fromhex(text.replace(' ', '').replace(',', ''))
                if not payload:
                    raise ValueError('empty')
            except ValueError as e:
                set_conn_pill(False, f'bad hex: {e}')
                return
        else:
            payload = text.encode('ascii', errors='replace')
        if worker[0].write(payload + b'\r\n'):
            ent_send.clear()

    def selected_keys():
        """信号树勾选的轨道 key 集合（决定 surfer 导出谁）。"""
        return {k for k, leaf in track_items.items()
                if leaf.checkState(0) == Qt.Checked}

    def do_surfer():
        """surfer 按钮：勾选轨道 -> live.vcd -> 拉起 Surfer.exe。

        导出结果提示放底部状态栏（全局文件事件），头部 lbl_wave 只显计数。
        三种"导不出"情况分别提示（曾把"没勾选"误报成"没记录"，像 bug）：
          1. records 本身为空 -> 'no wave records'
          2. 勾选集为空（点过 none）-> 'all tracks unchecked'
          3. 勾选与现有记录无交集（勾后 clear 过）-> '0/N match selection'
        """
        sel = selected_keys()
        if not records:
            lbl_stat_vcd.setText('no wave records')
            return
        if not sel:
            lbl_stat_vcd.setText('all tracks unchecked (TRACKS > all?)')
            return
        recs = [r for r in records if track_key(r[1]) in sel]
        if not recs:
            lbl_stat_vcd.setText(f'0/{len(records)} match selection')
            return
        w = GzVcdWriter(args.vcd, modules, features, events)
        w.export(recs)
        try:
            with open(args.vcd, 'r', encoding='ascii') as f:
                if '$enddefinitions' not in f.read():
                    raise OSError('missing $enddefinitions')
        except OSError as e:
            lbl_stat_vcd.setText(f'export corrupted: {e}')
            return
        import datetime
        lbl_stat_vcd.setText(
            f'live.vcd {len(recs)}/{len(records)} '
            f'{datetime.datetime.now().strftime("%H:%M")}')
        if not args.no_surfer:
            try:
                subprocess.Popen(
                    [args.surfer, os.path.abspath(args.vcd)],
                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            except OSError as e:
                lbl_stat_vcd.setText(f'surfer launch failed: {e}')

    def do_save_log():
        """LOG save：留底全量另存（文件对话框选目录/文件名，可选跳过）。"""
        import datetime
        from PySide6.QtWidgets import QFileDialog
        default = os.path.join(
            os.path.dirname(args.vcd),
            'log_' + datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
            + '.txt')
        path, _ = QFileDialog.getSaveFileName(
            win, 'save log', default, 'Text (*.txt);;All (*)')
        if not path:                 # 用户取消 = 可选保存，静默退出
            return
        with open(path, 'w', encoding='utf-8', errors='replace') as f:
            f.write('\n'.join(log_store))
        lbl_stat_vcd.setText(f'saved {len(log_store)} log lines')

    def do_save_wave():
        """WAVE save：12B 原始记录另存（可 --input 回放/转 VCD）。"""
        import datetime
        from PySide6.QtWidgets import QFileDialog
        if not records:
            lbl_stat_vcd.setText('no wave records to save')
            return
        default = os.path.join(
            os.path.dirname(args.vcd),
            'wave_' + datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
            + '.bin')
        path, _ = QFileDialog.getSaveFileName(
            win, 'save wave records', default, 'Binary (*.bin);;All (*)')
        if not path:
            return
        with open(path, 'wb') as f:
            for r in records:
                f.write(struct.pack('<III', *r))
        lbl_stat_vcd.setText(f'saved {len(records)} wave records')

    def do_load_wave():
        """WAVE import：加载 wave_*.bin（save/snapshot 产物）回放。

        读 12B 记录逐条走 on_wave_record 现有管线 -> MONITOR/TRACKS/
        门摘要/状态栏全部按回放数据重建；之后照常勾选 + surfer。
        导入会清空当前记录（避免与新数据混流），会话日志不受影响。
        """
        from PySide6.QtWidgets import QFileDialog
        path, _ = QFileDialog.getOpenFileName(
            win, 'import wave records', os.path.dirname(args.vcd),
            'Wave records (*.bin);;All (*)')
        if not path:
            return
        data = open(path, 'rb').read()
        if len(data) % RECORD_SIZE:
            lbl_stat_vcd.setText(
                f'bad bin: {len(data)}B not multiple of {RECORD_SIZE}')
            return
        do_clear_wave()              # 先清当前记录（回放 = 替换现场）
        n = 0
        for off in range(0, len(data), RECORD_SIZE):
            on_wave_record(struct.unpack_from('<III', data, off))
            n += 1
        lbl_stat_vcd.setText(
            f'imported {n} records <- {os.path.basename(path)}')

    def toggle_setup(checked):
        """⚙ uart 配置折叠切换：展开显示 port/baud/parity/flow 面板。"""
        cfg_row.setVisible(checked)

    def do_rescan():
        """rescan：重 grep 固件头文件刷新 id 映射，MONITOR/TRACKS 重建。

        改 X-macro 表（模块/feature/evt 名）后无需重启上位机：重跑
        load_map() -> 清掉按旧名字建的树（行/组/勾选缓存全失效）->
        用新映射重放现有 records 重建树。数据不丢、串口不断开。
        """
        nonlocal modules, features, events
        modules, features, events = load_map(args.root)
        mon.clear()
        mon_rows.clear()
        mon_groups.clear()
        track_items.clear()
        tree_tracks.clear()
        track_state.clear()
        for r in records:            # 现有记录按新名字重放（只重建显示）
            _apply_wave(r, False)
        lbl_stat_vcd.setText(
            f'rescanned: {len(modules)} modules, {len(features)} features, '
            f'{len(events)} evts, {len(records)} records replayed')

    def do_snapshot():
        """snapshot 按钮：log + wave + raw + config 归档到快照目录。"""
        import datetime
        ts_dir = os.path.join(
            os.path.dirname(args.vcd),
            'snapshots', datetime.datetime.now().strftime('%Y%m%d-%H%M%S'))
        os.makedirs(ts_dir, exist_ok=True)
        with open(os.path.join(ts_dir, 'log.txt'), 'w',
                  encoding='utf-8', errors='replace') as f:
            f.write('\n'.join(log_store))    # 留底导出：完整、不受过滤影响
        with open(os.path.join(ts_dir, 'wave.bin'), 'wb') as f:
            for r in records:
                f.write(struct.pack('<III', *r))
        if worker[0] is not None and worker[0].raw:
            with open(os.path.join(ts_dir, 'raw.bin'), 'wb') as f:
                f.write(bytes(worker[0].raw))
        with open(os.path.join(ts_dir, 'config.txt'), 'w',
                  encoding='ascii') as f:
            f.write(f'port={cb_port.currentText()}\n'
                    f'baud={cb_baud.currentText()}\n'
                    f'records={len(records)}\n')
        lbl_stat_vcd.setText(f'snap {os.path.basename(ts_dir)}')

    def do_clear():
        """clear all：清空 log（含留底）+ wave 记录缓冲/MONITOR/门摘要。"""
        log_store.clear()
        pending_log.clear()
        txt_log.clear()
        lbl_filter.setText('')
        do_clear_wave()

    def do_clear_log():
        """LOG 面板 clear：清面板 + 留底（快照从留底出，必须同步）。"""
        log_store.clear()
        pending_log.clear()
        txt_log.clear()
        lbl_filter.setText('')
        lbl_stat_log.setText('log: 0 lines')

    def do_clear_wave():
        """WAVE 面板 clear：清空记录缓冲/MONITOR 行/门摘要（勾选保留）。"""
        records.clear()
        mon.clear()
        mon_rows.clear()
        mon_groups.clear()
        track_state.clear()
        gate_bad[0] = 0
        lbl_wave.setText('wave: 0')
        lbl_wave_stat.setText('wave: 0')

    def do_sel_all():
        """TRACKS 全选：surfer 导出全部轨道。"""
        for leaf in track_items.values():
            leaf.setCheckState(0, Qt.Checked)

    def do_sel_none():
        """TRACKS 全不选：surfer 导出被清空（提示无记录可导）。"""
        for leaf in track_items.values():
            leaf.setCheckState(0, Qt.Unchecked)

    def tick_1s():
        """1 秒定时：KB/s 统计 + ago 列刷新 + 悬空门计数 + 录制闪烁。"""
        now = QDateTime.currentMSecsSinceEpoch()
        if worker[0] is not None:
            cur = len(worker[0].raw)
            if byte_count[1] and now > byte_count[1]:
                kb = (cur - byte_count[0]) / 1024.0 * 1000.0 / (
                    now - byte_count[1])
                lbl_stat_bw.setText(f'{kb:.1f} KB/s')
            byte_count[0] = cur
            byte_count[1] = now
        else:
            byte_count[1] = 0
            lbl_stat_bw.setText('0 KB/s')
        # ago 列：距最后一条记录的本机时间差；组行聚合组内最近事件。
        # 显示精度自适应：<1s 用 ms（高频流下 0.0s 恒定像卡死，实测坑），
        # <60s 用 s，<1h 用分，更长用小时
        def fmt_ago(ms):
            if ms < 1000:
                return f'{ms:.0f}ms'
            if ms < 60000:
                return f'{ms/1000:.1f}s'
            if ms < 3600000:
                return f'{ms/60000:.0f}m'
            return f'{ms/3600000:.1f}h'
        for key, g in mon_groups.items():
            walls = [st.get('last_wall') for (_r, st) in mon_rows.values()
                     if _r.parent() is g and st.get('last_wall')]
            if walls:
                g.setText(COL_AGO, fmt_ago(now - max(walls)))
        for key, (row, st) in mon_rows.items():
            if st.get('last_wall') is not None:
                row.setText(COL_AGO, fmt_ago(now - st['last_wall']))
        # 悬空门计数：仍处于 begin 未 end 的门（超时靠颜色已高亮）
        bad = sum(1 for _row, st in mon_rows.values()
                  if st['open_ts'] is not None)
        if bad:
            lbl_stat_gate.setText(f'\u26a0 gate open {bad}')
            lbl_stat_gate.setStyleSheet('color:#c93030;')
        else:
            lbl_stat_gate.setText('gate ok')
            lbl_stat_gate.setStyleSheet('')
        if rec_file[0] is not None:   # 录制中按钮闪烁
            on = btn_rec.styleSheet() == ''
            btn_rec.setStyleSheet('' if on else 'background:#c93030;color:white;')

    # 定时器：1s 状态刷新 + 过滤重放防抖（300ms 停手才重建面板）
    filter_timer = QTimer(win)
    filter_timer.setSingleShot(True)
    filter_timer.timeout.connect(replay_log)
    tmr = QTimer(win)
    tmr.timeout.connect(tick_1s)
    tmr.start(1000)

    # 控件接线
    ent_send.returnPressed.connect(do_send)
    btn_conn.clicked.connect(toggle_connect)
    btn_tx.clicked.connect(do_send)
    btn_pause.toggled.connect(toggle_pause)
    btn_rec.toggled.connect(toggle_record)
    btn_snap.clicked.connect(do_snapshot)
    btn_clear.clicked.connect(do_clear)
    btn_clear_log.clicked.connect(do_clear_log)
    btn_clear_wave.clicked.connect(do_clear_wave)
    btn_save_log.clicked.connect(do_save_log)
    btn_save_wave.clicked.connect(do_save_wave)
    btn_load_wave.clicked.connect(do_load_wave)
    btn_rescan.clicked.connect(do_rescan)
    btn_setup.toggled.connect(toggle_setup)
    btn_sel_all.clicked.connect(do_sel_all)
    btn_sel_none.clicked.connect(do_sel_none)
    btn_surfer.clicked.connect(do_surfer)
    edt_filter.lineEdit().textChanged.connect(on_filter_changed)

    win.show()
    # 多显示器修复：窗口强制放主屏中心并抢焦点（双屏下可能落在负坐标区）
    try:
        pri = app.primaryScreen().availableGeometry()
        fg = win.frameGeometry()
        win.move(pri.center() - fg.center())
        win.raise_()
        win.activateWindow()
    except Exception:
        pass
    # winId() 强制 Qt 立即创建原生窗口（防"进程活着但屏幕无窗口"）
    try:
        print(f'[hub] native hwnd={int(win.winId()):#x} '
              f'visible={win.isVisible()}', flush=True)
    except Exception as e:
        print(f'[hub] winId failed: {e}', flush=True)
    print('[hub] GUI window shown', flush=True)
    # 事件循环必须在 run_gui 内跑：win/控件都是本函数局部变量，
    # 若 return 后再 exec，Python 引用被回收 -> 窗口被 GC 销毁。
    rc = app.exec()
    print(f'[hub] GUI exited, code={rc}', flush=True)
    # 退出前收尾：录制文件落盘
    if rec_file[0] is not None:
        rec_file[0].close()
        rec_file[0] = None
    return rc


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
    ap.add_argument('--log-max', type=int, default=0,
                    help='log 留底上限行数（0=无限，默认无限）')
    args = ap.parse_args()

    # 默认路径全部相对脚本定位（任何 cwd 运行都一致）
    # PyInstaller 打包后 __file__ 指向临时解包目录，改用 exe 所在目录
    if getattr(sys, 'frozen', False):
        script_dir = os.path.dirname(sys.executable)
    else:
        script_dir = os.path.dirname(os.path.abspath(__file__))
    if args.vcd is None:
        args.vcd = os.path.join(script_dir, 'live.vcd')
    if args.surfer is None:
        cand = os.path.join(script_dir, 'surfer.exe')
        # exe 同目录没有 surfer.exe 时退回仓库内位置（开发机场景）
        if os.path.isfile(cand):
            args.surfer = os.path.abspath(cand)
        elif os.path.isdir(os.path.join(script_dir, '..', '..', 'tools')):
            args.surfer = os.path.abspath(
                os.path.join(script_dir, '..', 'tools', 'debug_hub',
                             'surfer.exe'))
        else:
            args.surfer = os.path.abspath(cand)
    if args.root is None:
        if os.path.isdir(os.path.join(script_dir, 'tools')):
            # exe 放在 SDK 根旁 -> 扫描 SDK 树建 id 映射
            root = os.path.abspath(script_dir)
        elif os.path.isdir(os.path.join(script_dir, '..', '..', 'tools')):
            # 脚本在 <sdk>/tools/debug_hub 下 -> SDK 根是上两级
            root = os.path.abspath(os.path.join(script_dir, '..', '..'))
        else:
            root = script_dir      # 独立 exe（无源码树）：无映射，id 降级显示
    else:
        root = os.path.abspath(args.root)
    args.root = root                         # 回写：do_rescan 复用同一根目录
    modules, features, events = load_map(root)   # 扫描固件头文件建映射
    LOG_STORE_MAX = args.log_max             # 留底上限（0=无限）
    print(f'[hub] scanned: {len(modules)} modules, {len(features)} features, '
          f'{len(events)} evts')

    if args.input:                           # ----- 离线模式 -----
        w = GzVcdWriter(args.vcd, modules, features, events)
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
    sys.exit(run_gui(args, modules, features, events))


if __name__ == '__main__':
    main()
