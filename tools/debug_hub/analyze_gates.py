# -*- coding: utf-8 -*-
"""analyze_gates.py - 门形（gate）健康分析工具。

用途：解析 hub 导出的 live.vcd，统计每个 1-bit 信号的高电平窗口长度
与边沿间隔，用来定位**悬空门**（begin 打了 end 没打，导致空闲时段
高电平、窗口长度异常大——adv 门 bug 就是这么发现的）。

用法：
    python analyze_gates.py                    # 默认读 tools/debug_hub/live.vcd
    python analyze_gates.py 路径/xxx.vcd       # 指定文件

判定经验：
    正常门窗口长度应该集中在一个数量级（如 adv 发送 ~1.8ms）；
    若出现明显更大的窗口（如 22.8ms），大概率是某条退出路径漏打 end。
"""
import re        # VCD 文本解析
import sys       # 命令行参数
import os        # 路径定位

# 默认文件：hub 的 live.vcd（相对脚本目录，可被 argv[1] 覆盖）
DEFAULT = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'live.vcd')
path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT

# 读入整个 VCD（文本模式；hub 导出为 ASCII+CRLF，read_text 直接可读）
t = open(path, encoding='ascii').read()

# 以 $enddefinitions 分界：前面是定义段（scope/var 声明），后面是数据段（值变更）
i = t.find('$enddefinitions')
hdr, body = t[:i], t[i:]

# ---------- 第一部分：从定义段恢复每个符号的完整层级路径 ----------
path, sym_map = [], {}   # path=当前 scope 栈；sym_map={符号: '全路径名'}
# 一个正则同时匹配三种行：$scope 开层 / $upscope 关层 / $var 信号声明
pat = re.compile(r'\$scope module (\S+) \$end|\$upscope \$end|\$var wire (\d+) (\S+) \$(\S+) \$end')
for m in pat.finditer(hdr):
    if m.group(1):                       # $scope module <名字>：进层
        path.append(m.group(1))
    elif m.group(0).startswith('$ups'):  # $upscope：退层
        path.pop()
    else:                                # $var：登记 符号 -> 栈路径 + 信号名
        sym_map[m.group(3)] = '/'.join(path + [m.group(4)])

print('symbols:')                        # 打印符号表（核对层级是否符合预期）
for s, p in sym_map.items():
    print(' ', s, '->', p)

# ---------- 第二部分：扫描数据段，收集每个符号的 (时间戳, 值) 边沿序列 ----------
events, last_ts = {}, None               # events={符号:[(ts,值),...]}
for line in body.splitlines():           # 逐行扫数据段
    if line.startswith('#'):             # #<数字> = 时间戳行
        last_ts = int(line[1:])
    else:                                # 值变更行：<值><符号>（标量 0/1）
        mm = re.match(r'([01])(\S+)\r?$', line)
        if mm and last_ts is not None:   # 只关心 1-bit 标量
            events.setdefault(mm.group(2), []).append((last_ts, mm.group(1)))

# ---------- 第三部分：按符号统计高电平窗口与边沿间隔 ----------
for s, evs in events.items():
    highs, open_t = [], None
    for ts, v in evs:                    # 成对扫描：1 开窗，0 闭窗记长度
        if v == '1' and open_t is None:
            open_t = ts                  # begin：记开始时间
        elif v == '0' and open_t is not None:
            highs.append(ts - open_t)    # end：窗口长度 = end - begin
            open_t = None
    if open_t is not None:
        highs.append(-1)                 # 文件尾还有未闭合的门（悬空证据！）
    # 相邻边沿间隔（含 1->0），看节奏是否规律（周期性任务应近似均匀）
    gaps = [evs[k + 1][0] - evs[k][0] for k in range(len(evs) - 1)]
    print(f'{sym_map.get(s, s)}: {len(evs)} edges, {len(highs)} windows')
    print('   window lens[:10]:', highs[:10])                # 窗口长度分布
    print('   begin ts:', evs[0][0], ' edge-to-edge gaps[:8]:', gaps[:8])
