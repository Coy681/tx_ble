#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""wave_demo.py - 【已过时，仅存档】早期 wave_view.py 的离线演示脚本。

它配套的是已被取代的 matplotlib 原型（wave_view.py，已从仓库删除，
git 历史可找回）。当前平台的离线演示/回归请用：
    test_hub.py    （tkinter 版核心链路）
    test_fst.py    （Qt 版 VCD 导出链路）
保留本文件只是因为其合成"模拟 BLE 业务流"的思路可参考（广告/扫描/双连接
的假数据构造），新代码不要基于它开发。

---
原说明：offline demo for wave_view.py, no hardware needed.

Synthesizes a fake firmware wave stream (as if a BLE device was
advertising, scanning and running 2 connections), then feeds it through
the real WaveStream parser and TrackModel and renders with matplotlib.

This validates the whole host pipeline: block reassembly -> record
slicing -> id decode -> level/num routing -> track layout.

Usage:
  python wave_demo.py            # 10 s of simulated traffic, live view
  python wave_demo.py --dump demo.wave   # also write raw stream to file
"""

import struct
import random

import matplotlib.pyplot as plt
import matplotlib.animation as animation

import os
import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import wave_view as wv

TICK_US = 1        # demo clock: 1 tick = 1 us
DURATION_US = 10_000_000     # simulate 10 s


def id_of(mod, feat, inst, evt, level=False):
    typ = 1 << 31 if level else 0
    return typ | (mod << 24) | (feat << 16) | (inst << 8) | evt


def block(payload):
    return struct.pack('<I', len(payload)) + payload


def synth_stream():
    """Build fake UART bytes: alternating text-log blocks and wave blocks."""
    rng = random.Random(42)
    MOD_ADV, MOD_CONN, MOD_RF, MOD_SCHED = 3, 4, 9, 1
    F_EXT, F_CE, F_IRQ, F_TASK = 0x02, 0x01, 0x01, 0x01

    recs = []

    # --- adv events: periodic gates every 20 ms on 2 adv sets ----------
    t = 5_000
    while t < DURATION_US:
        for inst in (0, 1):
            dur = 1_500 + rng.randint(0, 400)
            recs.append((t, id_of(MOD_ADV, F_CE, inst, 0x05, True), 1))          # adv window begin
            recs.append((t + 200, id_of(MOD_RF, F_IRQ, inst, 0x01, True), 1))    # radio begin
            recs.append((t + dur, id_of(MOD_ADV, F_CE, inst, 0x05, True), 0))    # adv end
            recs.append((t + dur - 150, id_of(MOD_RF, F_IRQ, inst, 0x01, True), 0))  # radio end
            # rssi samples of received scan requests
            for k in range(3):
                recs.append((t + 300 + k * 400,
                             id_of(MOD_ADV, F_CE, inst, 0x01),
                             -40 - rng.randint(0, 50)))
        t += 20_000

    # --- conn events: two connections interleaved, shorter period -------
    for ce in range(2):
        t = 8_000 + ce * 7_000
        while t < DURATION_US:
            dur = 900 + rng.randint(0, 300)
            recs.append((t, id_of(MOD_CONN, F_CE, ce, 0x03, True), 1))
            recs.append((t + dur, id_of(MOD_CONN, F_CE, ce, 0x03, True), 0))
            recs.append((t + dur // 2, id_of(MOD_CONN, F_CE, ce, 0x07),
                         -55 - rng.randint(0, 30)))                     # rssi num
            t += 15_000

    # --- scheduler task gates ------------------------------------------
    t = 2_000
    while t < DURATION_US:
        dur = 300 + rng.randint(0, 200)
        recs.append((t, id_of(MOD_SCHED, F_TASK, 0, 0x01, True), 1))
        recs.append((t + dur, id_of(MOD_SCHED, F_TASK, 0, 0x01, True), 0))
        t += 10_000

    recs.sort(key=lambda r: r[0])

    # --- pack into blocks, interleaved with a text log block -----------
    out = bytearray()
    for i in range(0, len(recs), 10):
        chunk = recs[i:i + 10]
        payload = b''.join(
            struct.pack('<III', ts, wid, val & 0xFFFFFFFF)
            for (ts, wid, val) in chunk)
        out += block(payload)
        if i % 50 == 0:   # text log block in the same stream
            txt = b'adv evt done\r\n'
            out += struct.pack('<I', len(txt)) + txt
    return out


def main():
    import argparse
    ap = argparse.ArgumentParser(description='offline demo for wave_view')
    ap.add_argument('--dump', default=None, help='write raw stream to file')
    args = ap.parse_args()

    raw = synth_stream()
    if args.dump:
        with open(args.dump, 'wb') as f:
            f.write(raw)
        print(f'wrote {len(raw)} bytes to {args.dump}')

    modules, features = wv.load_map('.')
    print(f'scanned: {len(modules)} modules, {len(features)} features')

    model = None
    n = 0
    for ts, wid, val in wv.WaveStream().feed(raw):
        if model is None:
            model = wv.TrackModel(ts)
        model.add(ts, wid, val, modules, features)
        n += 1
    print(f'records parsed: {n}')

    fig, ax = plt.subplots(figsize=(13, 7))
    fig.suptitle('wave demo - simulated BLE adv + 2 conn (no hardware)')

    # static render of the whole capture
    viewer = type('V', (), {'ax': ax, 'model': model,
                            'window_us': DURATION_US})()
    # reuse draw logic but with full window: emulate Viewer.draw body
    wv.Viewer.draw = lambda self: None   # neutralize; draw manually below
    rows = {}
    for track, (xs, ys) in sorted(model.num.items()):
        pts = list(zip(xs, ys))
        if pts:
            row = rows.setdefault(track, len(rows))
            ax.step([p[0] for p in pts],
                    [row + 0.4 * min(p[1], 4) for p in pts],
                    where='post', linewidth=1, marker='.', markersize=2)
    for track, gates in sorted(model.gates.items()):
        for (b, e) in gates:
            row = rows.setdefault(track, len(rows))
            ax.plot([b, e], [row + 0.2, row + 0.2],
                    linewidth=8, alpha=0.5, solid_capstyle='butt')
    for track, ts_list in sorted(model.marks.items()):
        if ts_list:
            row = rows.setdefault(track, len(rows))
            ax.plot(ts_list, [row + 0.2] * len(ts_list), '|',
                    markersize=12, linestyle='')
    ax.set_yticks(range(len(rows)))
    ax.set_yticklabels(list(rows.keys()), fontsize=7)
    ax.grid(True, axis='x', alpha=0.3)
    ax.set_xlabel('time (us, simulated)')
    ax.set_xlim(0, DURATION_US)
    plt.tight_layout()
    plt.show()


if __name__ == '__main__':
    main()
