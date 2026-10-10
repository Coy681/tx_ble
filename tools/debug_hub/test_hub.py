#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""test_hub.py - tkinter 版 hub 的端到端离线回归测试。

流程：
  1. 在内存里合成一段"固件串口流"：log 帧 + wave 帧 + 故意混入的垃圾字节
  2. 调用 debug_hub.py --input 离线模式解析并导出 VCD
  3. 校验 stdout（log 文本回显）和 VCD 内容（scope 层级、信号、值变更）

覆盖点：
  - A5 帧解析、垃圾字节后的重同步
  - LEVEL 门（begin/end）与 NUM 采样两种记录
  - 未知 id 的降级命名
通过标准：脚本正常结束、stdout 无异常栈（内容人工/肉眼比对或 CI 断言）。
"""
import struct       # 合成 12B wave 记录
import subprocess   # 以子进程方式运行 hub（模拟真实命令行）
import sys
import os

# 切到本脚本所在目录：产物（synth_stream.bin / synth_test.vcd）都生成在这里
HUB_DIR = os.path.dirname(os.path.abspath(__file__))
os.chdir(HUB_DIR)


def frame(t, payload):
    """按线上协议组一帧：[0xA5][type][len][payload]。t=帧类型。"""
    return bytes([0xA5, t, len(payload)]) + payload


def rec(ts, wid, val):
    """合成一条 12B wave 记录：小端 [u32 ts][u32 id][u32 val]。"""
    return struct.pack('<III', ts, wid, val)


stream = b''     # 待喂给 hub 的完整字节流
# --- log 帧：验证文本通路 ---
stream += frame(0, b'boot ok\r\n')
# --- wave 帧：LEVEL 门 begin（id = LEVEL|adv<<24|LEGACY<<16|state1）---
LEVEL = 0x80000000
stream += frame(1, rec(100, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 1))
stream += frame(0, b'adv started\r\n')
# --- 同轨道 LEVEL end：与上面 begin 配对成门 ---
stream += frame(1, rec(250, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 0))
# --- NUM 采样：scheduler 模块 feature0x01 state2，值 42 ---
stream += frame(1, rec(300, (0x01 << 24) | (0x01 << 16) | 0x02, 42))
# --- 垃圾字节：测试解析器失步重同步能力（后面帧必须无损恢复）---
stream += b'\x00\x00garbage'
stream += frame(0, b'still alive\r\n')
# --- 未知 id（全 0）：验证降级命名 mod0x0/feat0x0 ---
stream += frame(1, rec(400, 0x00000000, 7))

open('synth_stream.bin', 'wb').write(stream)   # 落盘：既是输入也是可复现样例
# 以离线模式运行 hub（--no-surfer：不出波形窗口）
r = subprocess.run([sys.executable, 'debug_hub.py', '--input', 'synth_stream.bin',
                    '--vcd', 'synth_test.vcd', '--no-surfer'],
                   capture_output=True, text=True, timeout=60)
print(r.stdout, r.stderr)                      # 回显 hub 输出（含 log 文本）
print('---- vcd ----')
print(open('synth_test.vcd').read())           # 打印 VCD 供断言/肉眼比对
