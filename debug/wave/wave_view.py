#!/usr/bin/env python3
"""wave_view.py - host-side waveform viewer for the tx_ble wave module.

Pipeline:
  1. grep WAVE_MODULE_LIST / WAVE_XXX_FEATS X-macros from firmware headers
     to build the id -> track-name mapping (same source of truth as firmware).
  2. open the debug UART, strip debug_port block headers [4B len][payload],
     cut wave payloads into 12-byte records [ts][id][value].
  3. decode id = [type(1)|module(7)|feature(8)|instance(8)|evt(8)],
     route LEVEL records to gate tracks, NUM records to curve lanes.

Usage:
  python wave_view.py --port COM5 --baud 1000000 --root ../..

Requires: pyserial, matplotlib
  pip install pyserial matplotlib

Known limitations (v0 skeleton):
  - u32 timestamp wraparound handled by difference, not absolute time.
  - if the stream exceeds plot refresh capability, records queue in the
    serial buffer; lower firmware wave traffic or raise --refresh.
"""

import argparse
import re
import struct
from collections import defaultdict

import matplotlib.pyplot as plt
import matplotlib.animation as animation

# --------------------------------------------------------------- header scan

MOD_LIST_RE = re.compile(r'#define\s+WAVE_MODULE_LIST\s*\(X\)(.*?)(?=\n#define)', re.S)
MOD_ITEM_RE = re.compile(r'X\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*"([^"]+)"\s*\)')
FEATS_BLOCK_RE = re.compile(
    r'#define\s+WAVE_\w+_FEATS\s*\(X\)(.*?)(?=\n#define|\nWAVE_FEAT_ENUM|\Z)', re.S)
FEAT_ITEM_RE = re.compile(
    r'X\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')

RECORD_SIZE = 12
BLOCK_HDR = 4
TYPE_NUM, TYPE_LEVEL = 0, 1
EVT_PROBE = 0xFF


def _num(tok):
    return int(tok, 0)


def _strip_comments(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)   # block comments
    text = re.sub(r'//[^\n]*', '', text)                # line comments
    return text


def load_map(root):
    """Grep wave id X-macros from all headers under root.

    Returns (modules, features):
      modules  = {mod_id: (short, label)}
      features = {(mod_id, feat_id): (mod_short, feat_short)}
    """
    import pathlib
    modules, features = {}, {}
    for h in pathlib.Path(root).rglob('*.h'):
        if h.name.startswith('wave_view'):
            continue
        try:
            text = _strip_comments(h.read_text(encoding='utf-8', errors='ignore'))
        except OSError:
            continue
        for m in MOD_LIST_RE.finditer(text):
            for short, val, label in MOD_ITEM_RE.findall(m.group(1)):
                modules[_num(val)] = (short, label)
        for m in FEATS_BLOCK_RE.finditer(text):
            for line in m.group(1).splitlines():
                fm = FEAT_ITEM_RE.search(line)
                if not fm:
                    continue
                mod_short, feat_short, feat_val = fm.groups()
                mod_id = next((k for k, (s, _l) in modules.items()
                               if s == mod_short), None)
                if mod_id is not None:
                    features[(mod_id, _num(feat_val))] = (mod_short, feat_short)
    return modules, features


# --------------------------------------------------------------- id decode

def decode_id(w):
    """id = [type(1)|module(7)|feature(8)|instance(8)|evt(8)] -> tuple."""
    return ((w >> 31) & 1, (w >> 24) & 0x7F, (w >> 16) & 0xFF,
            (w >> 8) & 0xFF, w & 0xFF)


def track_name(mod, feat, inst, evt, modules, features):
    m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))
    f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))
    parts = [m[1], f[1]]
    if inst:
        parts.append(f'i{inst}')
    parts.append('probe' if evt == EVT_PROBE else f'e{evt}')
    return '/'.join(parts)


# --------------------------------------------------------------- stream

class WaveStream:
    """Reassembles debug_port blocks, yields (ts, id, value) records.

    Block format: [u32 len][payload], len = payload bytes (<= 236).
    Wave payload: N x 12-byte records [u32 ts][u32 id][u32 value].
    Text-log blocks are skipped (payload length not a 12B multiple).
    """

    def __init__(self):
        self.buf = bytearray()

    def feed(self, data):
        self.buf += data
        while len(self.buf) >= BLOCK_HDR:
            (blen,) = struct.unpack_from('<I', self.buf, 0)
            if blen == 0 or blen > 236 or blen % RECORD_SIZE != 0:
                self.buf = self.buf[1:]          # resync on garbage
                continue
            if len(self.buf) < BLOCK_HDR + blen:
                break
            payload = self.buf[BLOCK_HDR:BLOCK_HDR + blen]
            del self.buf[:BLOCK_HDR + blen]
            for off in range(0, blen, RECORD_SIZE):
                yield struct.unpack_from('<III', payload, off)


# --------------------------------------------------------------- plot model

class TrackModel:
    """Accumulates records into per-track drawing data."""

    def __init__(self, t0):
        self.t0 = t0
        self.num = defaultdict(lambda: ([], []))   # track -> (times, values)
        self.gates = defaultdict(list)             # track/sN -> [(begin, end)]
        self.marks = defaultdict(list)             # track -> [times]
        self._open = {}                            # (track, state) -> t_begin

    def add(self, ts, wid, val, modules, features):
        typ, mod, feat, inst, evt = decode_id(wid)
        t = (ts - self.t0) & 0xFFFFFFFF
        base = track_name(mod, feat, inst, evt, modules, features)
        if typ == TYPE_LEVEL:
            key = (base, evt)
            if val:
                self._open[key] = t
            else:
                b = self._open.pop(key, None)
                if b is not None:
                    self.gates[base + f'/s{evt}'].append((b, t))
        else:
            if evt == EVT_PROBE and val == 0:
                self.marks[base].append(t)
            else:
                xs, ys = self.num[base]
                xs.append(t)
                ys.append(val)


# --------------------------------------------------------------- viewer

class Viewer:
    def __init__(self, args):
        import serial
        self.ser = serial.Serial(args.port, args.baud, timeout=0.05)
        self.modules, self.features = load_map(args.root)
        self.stream = WaveStream()
        self.model = None
        self.window_us = int(args.window * 1_000_000)

        print(f'scanned: {len(self.modules)} modules, '
              f'{len(self.features)} features')
        for k, (short, label) in sorted(self.modules.items()):
            print(f'  mod {k:#04x} {short:<8} {label}')

        self.fig, self.ax = plt.subplots(figsize=(13, 6))
        self.fig.suptitle('tx_ble wave viewer')

    def pump(self):
        data = self.ser.read(8192)
        if not data:
            return
        for ts, wid, val in self.stream.feed(data):
            if self.model is None:
                self.model = TrackModel(ts)
            self.model.add(ts, wid, val, self.modules, self.features)

    def draw(self):
        ax, model = self.ax, self.model
        ax.clear()
        if model is None:
            ax.text(0.5, 0.5, 'waiting for wave records...', ha='center')
            return
        # visible window: last --window seconds of activity
        ends = [max(xs) for xs, _ in model.num.values() if xs]
        ends += [e for gs in model.gates.values() for (_b, e) in gs]
        ends += [t for lst in model.marks.values() for t in lst]
        now = max(ends) if ends else 0
        t_lo = max(0, now - self.window_us)

        rows = {}                                    # track -> row index
        for track, (xs, ys) in sorted(model.num.items()):
            pts = [(x, y) for x, y in zip(xs, ys) if t_lo <= x <= now]
            if pts:
                row = rows.setdefault(track, len(rows))
                ax.step([p[0] for p in pts],
                        [row + 0.4 * min(p[1], 4) for p in pts],
                        where='post', linewidth=1, marker='.', markersize=2)
        for track, gates in sorted(model.gates.items()):
            for (b, e) in gates:
                if e < t_lo or b > now:
                    continue
                row = rows.setdefault(track, len(rows))
                ax.plot([b, e], [row + 0.2, row + 0.2],
                        linewidth=8, alpha=0.5, solid_capstyle='butt')
        for track, ts_list in sorted(model.marks.items()):
            pts = [t for t in ts_list if t_lo <= t <= now]
            if pts:
                row = rows.setdefault(track, len(rows))
                ax.plot(pts, [row + 0.2] * len(pts), '|',
                        markersize=12, linestyle='')

        ax.set_yticks(range(len(rows)))
        ax.set_yticklabels(list(rows.keys()), fontsize=7)
        ax.grid(True, axis='x', alpha=0.3)
        ax.set_xlabel('time (tick)')


def animation_ref(viewer, refresh_ms):
    # closure keeps matplotlib animation wired to the viewer
    def update(_frame):
        viewer.pump()
        viewer.draw()
    return animation.FuncAnimation(viewer.fig, update, interval=refresh_ms,
                                   cache_frame_data=False)


if __name__ == '__main__':
    main()
