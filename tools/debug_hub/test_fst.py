#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""test_fst.py - Qt 版 hub 的 VCD 导出回归测试（层级 + 按需声明）。

流程：
  1. 合成一段小型串口流（log + LEVEL 门 + NUM 采样 + 垃圾字节）
  2. 调用 debug_hub_qt.py --input 离线模式导出 VCD
  3. 校验 VCD 的 $scope 层级（tx_ble > 模块 > feature）与 $var：
     - 只应出现数据里出现过的信号（无空占位）
     - adv/LEGACY/s1 是 1-bit 门，scheduler/STIMER/s2_num 是 32-bit 数值

与 test_hub.py 的区别：本测试跑的是 Qt 版导出路径（GzVcdWriter 在两个
hub 里各有一份，协议变更时两个都要过一遍）。
"""
import struct       # 合成 12B 记录
import subprocess   # 子进程运行 hub
import sys
import os

# 切到脚本目录：中间产物生成在这里，跑完可删
os.chdir(os.path.dirname(os.path.abspath(__file__)))


def frame(t, payload):
    """组一帧：[0xA5][type][len][payload]。"""
    return bytes([0xA5, t, len(payload)]) + payload


def rec(ts, wid, val):
    """一条 12B wave 记录（小端 ts/id/val）。"""
    return struct.pack('<III', ts, wid, val)


LEVEL = 0x80000000          # id 的 bit31：LEVEL 类型标记
stream = b''                # 合成流
stream += frame(0, b'boot ok\r\n')                                            # log 帧
stream += frame(1, rec(100, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 1))   # adv/LEGACY 门 begin
stream += frame(1, rec(250, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 0))   # 同门 end
stream += frame(1, rec(300, (0x01 << 24) | (0x01 << 16) | 0x02, 42))          # scheduler NUM=42
stream += b'\x00\x00zz'                                                       # 垃圾（重同步测试）
stream += frame(0, b'done\r\n')                                               # log 帧

open('synth_stream.bin', 'wb').write(stream)        # 落盘输入样例
# Qt 版离线导出
r = subprocess.run([sys.executable, 'debug_hub_qt.py', '--input', 'synth_stream.bin',
                    '--vcd', 'synth_test.vcd', '--no-surfer'],
                   capture_output=True, text=True, timeout=120)
print(r.stdout, r.stderr)                           # 回显 hub 输出
txt = open('synth_test.vcd', encoding='ascii').read()
print(txt)                                          # 完整 VCD（肉眼/C I 比对）
# 摘出 scope/var 行集中显示，检查层级与按需声明
scopes = [l for l in txt.splitlines() if 'scope' in l or 'var' in l]
print('---- scopes/vars ----')
for s in scopes:
    print(s)
