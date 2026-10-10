#!/usr/bin/env python3
"""debug_hub.py - unified debug platform for the tx_ble firmware.

One entry point for the debug UART:

    UART --> [A5 sync][type][len][payload] frames
               type 0x00: log text  -> left pane (tkinter) with filter/colors
               type 0x01: wave recs -> right pane live canvas + live.vcd (Surfer)

Wave records are 12 bytes: [u32 ts][u32 id][u32 value].
id = [type(1)|module(7)|feature(8)|instance(8)|evt(8)].
  LEVEL (bit31 set): value 1=begin / 0=end -> gate on track
  NUM: value as data -> numeric sample
Track names come from the same X-macro lists the firmware uses
(WAVE_MODULE_LIST / WAVE_*_FEATS), grepped from the source tree.

Usage:
  python debug_hub.py                          # GUI, pick COM/baud in toolbar
  python debug_hub.py --port COM7 --baud 1000000
  python debug_hub.py --input dump.bin         # offline: dump -> vcd

Requires: pyserial   (pip install pyserial)
Surfer:   surfer_win/surfer.exe (launched by the "surfer" button).
"""

import argparse
import gzip
import os
import re
import struct
import subprocess
import sys
import threading
import time

SYNC = 0xA5
TYPE_LOG = 0x00
TYPE_WAVE = 0x01
RECORD_SIZE = 12
MAX_PAYLOAD = 236
EVT_PROBE = 0xFF

# --------------------------------------------------------------- id mapping

MOD_LIST_RE = re.compile(r'#define\s+WAVE_MODULE_LIST\s*\(X\)(.*?)(?=\n#define)', re.S)
MOD_ITEM_RE = re.compile(r'X\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*"([^"]+)"\s*\)')
FEATS_BLOCK_RE = re.compile(
    r'#define\s+WAVE_\w+_FEATS\s*\(X\)(.*?)(?=\n#define|\nWAVE_FEAT_ENUM|\Z)', re.S)
FEAT_ITEM_RE = re.compile(
    r'X\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')


def _num(tok):
    return int(tok, 0)


def _strip_comments(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    text = re.sub(r'//[^\n]*', '', text)
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
        if h.name.startswith('debug_hub'):
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


def decode_id(w):
    """id = [type(1)|module(7)|feature(8)|instance(8)|evt(8)]."""
    return ((w >> 31) & 1, (w >> 24) & 0x7F, (w >> 16) & 0xFF,
            (w >> 8) & 0xFF, w & 0xFF)


def track_name(mod, feat, inst, evt, modules, features):
    m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))
    f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))
    parts = [m[1], f[1]]
    if inst:
        parts.append(f'i{inst}')
    parts.append('probe' if evt == EVT_PROBE else f's{evt}')
    return '/'.join(parts)


# --------------------------------------------------------------- writer

def collect_tracks(records, modules, features):
    """Scan records and return {(mod_label, feat_label, inst, lane, width)}.

    Only tracks that actually appear in the data are returned, so the
    viewer shows exactly what was captured (no empty placeholders).
    """
    tracks = set()
    for ts, wid, val in records:
        typ, mod, feat, inst, evt = decode_id(wid)
        m = modules.get(mod, (f'mod{mod:#x}', f'mod{mod:#x}'))
        f = features.get((mod, feat), (m[0], f'feat{feat:#x}'))
        if typ == 1:
            tracks.add((m[1], f[1], inst, f's{evt}', 1))
        else:
            lane = 'probe' if evt == EVT_PROBE else f's{evt}_num'
            tracks.add((m[1], f[1], inst, lane, 32))
    return sorted(tracks)


class GzVcdWriter:
    """VCD writer with hierarchy and on-demand signal set.

    Name kept for call-site stability. Hierarchy: $scope module
    <module> > $scope module <feature> > signal lanes. Signals are
    declared only for tracks present in the data, inside
    $enddefinitions. Call export(records) once.
    """

    def __init__(self, path, modules, features):
        self.path = path
        self.modules = modules
        self.features = features

    def export(self, records):
        tracks = collect_tracks(records, self.modules, self.features)
        # NOTE: this Surfer build does not support .vcd.gz (fails with
        # UnknownFileFormat even though the gzip stream is valid), so we
        # write plain VCD despite the larger file size.
        # NOTE: this Surfer build's VCD parser requires CRLF line endings
        # (LF-only files fail with "expected an id for a value change").
        with open(self.path, 'w', encoding='ascii', newline='\r\n') as f:
            f.write('$date now $end\n$version debug_hub $end\n')
            f.write('$timescale 1us $end\n$scope module tx_ble $end\n')
            sym_of = {}
            self._nsym = 0
            cur_scope = None

            def enter_scope(scope):
                """scope = (module, feature) scope-name pair; feat may be None."""
                nonlocal cur_scope
                if scope == cur_scope:
                    return
                # close previous nesting fully: feature scope then module
                if cur_scope is not None:
                    if cur_scope[1] is not None:
                        f.write('$upscope $end\n')
                    f.write('$upscope $end\n')
                f.write(f'$scope module {scope[0]} $end\n')
                if scope[1] is not None:
                    f.write(f'$scope module {scope[1]} $end\n')
                cur_scope = scope

            for mod_l, feat_l, inst, lane, width in tracks:
                scope = (mod_l, feat_l + (f'/i{inst}' if inst else ''))
                enter_scope(scope)
                self._nsym += 1
                sym = '!' * (1 + self._nsym // 94) + chr(33 + self._nsym % 94)
                sym_of[(mod_l, feat_l, inst, lane)] = sym
                f.write(f'$var wire {width} {sym} ${lane.replace("/", "_")} $end\n')
            if cur_scope is not None:
                if cur_scope[1] is not None:
                    f.write('$upscope $end\n')
                f.write('$upscope $end\n')
            f.write('$enddefinitions $end\n')

            last_ts = -1
            for ts, wid, val in records:
                typ, mod, feat, inst, evt = decode_id(wid)
                m = self.modules.get(mod, (f'm{mod}', f'mod{mod:#x}'))
                fl = self.features.get((mod, feat),
                                       (m[0], f'feat{feat:#x}'))[1]
                if typ == 1:
                    key = (m[1], fl, inst, f's{evt}')
                    v = '1' if val else '0'
                else:
                    lane = 'probe' if evt == EVT_PROBE else f's{evt}_num'
                    key = (m[1], fl, inst, lane)
                    v = f'b{val & 0xFFFFFFFF:032b} '   # space before id per VCD spec
                if key not in sym_of:
                    continue
                if ts != last_ts:
                    f.write(f'#{ts}\n')
                    last_ts = ts
                f.write(f'{v}{sym_of[key]}\n')
            f.write('\n')
        return len(records)


# --------------------------------------------------------------- frame parser

class FrameParser:
    """Resync-capable parser for [A5][type][len][payload] frames."""

    def __init__(self, on_log, on_wave):
        self.buf = bytearray()
        self.on_log = on_log
        self.on_wave = on_wave

    def feed(self, data):
        self.buf += data
        while True:
            i = self.buf.find(bytes([SYNC]))
            if i < 0:
                self.buf.clear()
                return
            if len(self.buf) < i + 3:
                del self.buf[:i]
                return
            typ = self.buf[i + 1]
            ln = self.buf[i + 2]
            if ln == 0 or ln > MAX_PAYLOAD or typ > 1:
                del self.buf[:i + 1]        # false sync, step past
                continue
            end = i + 3 + ln
            if len(self.buf) < end:
                del self.buf[:i]
                return
            payload = bytes(self.buf[i + 3:end])
            del self.buf[:end]
            if typ == TYPE_LOG:
                self.on_log(payload)
            else:
                for off in range(0, ln - RECORD_SIZE + 1, RECORD_SIZE):
                    self.on_wave(struct.unpack_from('<III', payload, off))


# --------------------------------------------------------------- hub state

# wave records accumulate in 'all' (for the surfer button); 'shown'
# counts how many the right-pane monitor has already displayed.
wave_hub = {'buffer': [], 'shown': 0, 'lock': threading.Lock()}


def open_surfer(exe, vcd_path):
    try:
        subprocess.Popen([exe, os.path.abspath(vcd_path)],
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print(f'[hub] surfer launched: {vcd_path}')
    except OSError as e:
        print(f'[hub] failed to launch surfer: {e}')


# --------------------------------------------------------------- wave monitor

class WaveMonitor:
    """Raw wave feed in the right pane: one line per record, newest at
    bottom. Purpose is to see that wave data keeps arriving - real
    analysis happens in Surfer (button)."""

    MAX_LINES = 2000

    def __init__(self, text, modules, features):
        self.txt = text
        self.modules = modules
        self.features = features
        self.count = 0
        text.tag_configure('lvl', foreground='#4fc3f7')
        text.tag_configure('num', foreground='#ffd54f')
        text.tag_configure('unk', foreground='#777')

    def drain(self):
        with wave_hub['lock']:
            recs = wave_hub['buffer'][wave_hub['shown']:]
            wave_hub['shown'] = len(wave_hub['buffer'])
        for ts, wid, val in recs:
            typ, mod, feat, inst, evt = decode_id(wid)
            base = track_name(mod, feat, inst, evt, self.modules, self.features)
            if typ == 1:
                self.txt.insert('end',
                                f'{ts:>10}  {wid:08X}  {"begin" if val else "end  "}  {base}\n',
                                'lvl')
            else:
                self.txt.insert('end',
                                f'{ts:>10}  {wid:08X}  {val:<10}  {base}/num\n',
                                'num')
        self.count += len(recs)
        # trim history
        if int(self.txt.index('end-1c').split('.')[0]) > self.MAX_LINES:
            self.txt.delete('1.0', f'{self.MAX_LINES // 2}.0')
        self.txt.see('end')


# --------------------------------------------------------------- GUI

def run_gui(log_queue, args, modules, features):
    import tkinter as tk
    from tkinter import ttk
    from tkinter.scrolledtext import ScrolledText

    root = tk.Tk()
    root.title('tx_ble debug hub')
    root.geometry('1280x640')

    # ---------------- toolbar: serial config ----------------
    bar = tk.Frame(root)
    bar.pack(fill='x')

    import serial.tools.list_ports

    tk.Label(bar, text='port:').pack(side='left')
    var_port = tk.StringVar(value=args.port or '')
    cb_port = ttk.Combobox(bar, textvariable=var_port, width=8,
                           values=[p.device for p in
                                   serial.tools.list_ports.comports()])
    cb_port.pack(side='left', padx=(2, 8))

    tk.Label(bar, text='baud:').pack(side='left')
    var_baud = tk.StringVar(value=str(args.baud))
    cb_baud = ttk.Combobox(bar, textvariable=var_baud, width=9,
                           values=['9600', '115200', '460800',
                                   '921600', '1000000', '2000000'])
    cb_baud.pack(side='left', padx=(2, 8))

    ser = [None]
    reader_thr = [None]

    def reader_loop(port):
        n = [0]
        while ser[0] is port and port.is_open:
            try:
                data = port.read(8192)
            except Exception:
                break
            if data:
                if args.raw_dump:
                    n[0] += len(data)
                    sys.stderr.write(f'[raw {n[0]:7d}B] {data[:64].hex(" ")}\n')
                hub_parser.feed(data)

    def toggle_connect():
        if ser[0] is not None:                    # disconnect
            p = ser[0]
            ser[0] = None
            try:
                p.close()
            except Exception:
                pass
            btn_conn.configure(text='connect', bg='#27ae60')
            lbl_conn.configure(text='disconnected', fg='#888')
            return
        try:
            p = serial.Serial(var_port.get(), int(var_baud.get()), timeout=0.05)
        except Exception as e:
            lbl_conn.configure(text=f'open failed: {e}', fg='#e74c3c')
            return
        ser[0] = p
        btn_conn.configure(text='disconnect', bg='#c0392b')
        lbl_conn.configure(text=f'{var_port.get()} @ {var_baud.get()}',
                           fg='#2ecc71')
        reader_thr[0] = threading.Thread(target=reader_loop, args=(p,),
                                         daemon=True)
        reader_thr[0].start()

    btn_conn = tk.Button(bar, text='connect', command=toggle_connect,
                         bg='#27ae60', fg='white', width=10)
    btn_conn.pack(side='left', padx=(0, 4))
    lbl_conn = tk.Label(bar, text='disconnected', fg='#888')
    lbl_conn.pack(side='left', padx=(0, 12))

    tk.Label(bar, text='send:').pack(side='left')
    var_send = tk.StringVar()
    ent_send = tk.Entry(bar, textvariable=var_send, width=22)
    ent_send.pack(side='left', padx=(2, 4))

    def do_send(_event=None):
        if ser[0] is None or not var_send.get():
            return
        text = var_send.get()
        if var_hex.get():
            try:
                payload = bytes.fromhex(
                    text.replace(' ', '').replace(',', ''))
                if not payload:
                    raise ValueError('empty hex')
            except ValueError as e:
                lbl_conn.configure(text=f'bad hex: {e}', fg='#e74c3c')
                return
        else:
            payload = text.encode('ascii', errors='replace')
        try:
            ser[0].write(payload + b'\r\n')
            var_send.set('')
        except Exception as e:
            lbl_conn.configure(text=f'send failed: {e}', fg='#e74c3c')
    var_hex = tk.BooleanVar(value=False)
    tk.Checkbutton(bar, text='hex', variable=var_hex).pack(side='left')
    btn_send = tk.Button(bar, text='tx', command=do_send)
    btn_send.pack(side='left')
    ent_send.bind('<Return>', do_send)

    tk.Label(bar, text='    filter:').pack(side='left')
    var_filter = tk.StringVar()
    tk.Entry(bar, textvariable=var_filter, width=18).pack(side='left', padx=4)

    def refresh_wave():
        """Convert all buffered wave records into the VCD and open Surfer."""
        with wave_hub['lock']:
            recs = list(wave_hub['buffer'])
        if not recs:
            lbl_wave.configure(text='no wave records buffered', fg='#e67e22')
            return
        w = GzVcdWriter(args.vcd, modules, features)
        w.export(recs)
        # verify file completeness before launching the viewer
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
        with wave_hub['lock']:
            wave_hub['buffer'].clear()
            wave_hub['shown'] = 0
        wave_monitor.txt.delete('1.0', 'end')
        wave_monitor.count = 0
        lbl_wave.configure(text='buffer cleared', fg='#888')

    btn_wave = tk.Button(bar, text='surfer', command=refresh_wave,
                         bg='#2980b9', fg='white')
    btn_wave.pack(side='left', padx=(12, 2))
    tk.Button(bar, text='clear', command=clear_wave).pack(side='left')
    lbl_wave = tk.Label(bar, text='wave: buffering (0 records)', fg='#888')
    lbl_wave.pack(side='left', padx=8)

    paused = [False]

    def toggle_pause():
        paused[0] = not paused[0]
        btn_pause.configure(text='resume' if paused[0] else 'pause')
    btn_pause = tk.Button(bar, text='pause', command=toggle_pause)
    btn_pause.pack(side='left', padx=4)

    # ---------------- main split: log | wave ----------------
    paned = tk.PanedWindow(root, orient='horizontal', sashwidth=4)
    paned.pack(fill='both', expand=True)

    left = tk.Frame(paned, bg='#111')
    paned.add(left, width=640)
    txt = ScrolledText(left, font=('Consolas', 9), bg='#111', fg='#ddd')
    txt.pack(fill='both', expand=True)
    txt.tag_configure('warn', foreground='#e67e22')
    txt.tag_configure('err', foreground='#e74c3c')
    txt.tag_configure('ok', foreground='#2ecc71')

    right = tk.Frame(paned, bg='#181818')
    paned.add(right, width=620)
    wtxt = ScrolledText(right, font=('Consolas', 8), bg='#181818', fg='#bbb')
    wtxt.pack(fill='both', expand=True)
    wave_monitor = WaveMonitor(wtxt, modules, features)

    def pump():
        try:
            while True:
                line = log_queue.get_nowait()
                flt = var_filter.get()
                if flt and flt not in line:
                    continue
                low = line.lower()
                tag = None
                if 'err' in low or 'fail' in low:
                    tag = 'err'
                elif 'warn' in low:
                    tag = 'warn'
                elif 'ok' in low or 'ready' in low or 'done' in low:
                    tag = 'ok'
                txt.insert('end', line, tag or ())
                txt.see('end')
        except Exception:
            pass
        wave_monitor.drain()
        n = wave_monitor.count
        if lbl_wave.cget('text').startswith('wave:'):
            lbl_wave.configure(text=f'wave: {n} records')
        root.after(150, pump)

    # ---------------- parser wiring ----------------
    def on_log(payload):
        log_queue.put(payload.decode('ascii', errors='replace'))

    def on_wave(rec):
        with wave_hub['lock']:
            wave_hub['buffer'].append(rec)

    hub_parser = FrameParser(on_log=on_log, on_wave=on_wave)

    # pre-select the requested port (if any)
    if args.port:
        var_port.set(args.port)
        toggle_connect()

    root.after(150, pump)
    root.mainloop()


# --------------------------------------------------------------- entry

def main():
    ap = argparse.ArgumentParser(description='tx_ble debug hub')
    ap.add_argument('--port', help='serial port to open at startup, e.g. COM7')
    ap.add_argument('--input', help='raw capture file for offline conversion')
    ap.add_argument('--baud', type=int, default=1000000)
    ap.add_argument('--root', default=None, help='source tree root for id scan '
                    '(default: SDK root, two levels above this script)')
    ap.add_argument('--vcd', default=None, help='VCD output path '
                    '(default: <script dir>/live.vcd)')
    ap.add_argument('--surfer', default=None, help='surfer.exe path '
                    '(default: <sdk root>/surfer_win/surfer.exe)')
    ap.add_argument('--no-surfer', action='store_true')
    ap.add_argument('--raw-dump', action='store_true',
                    help='hex-dump all received bytes to stderr for debugging')
    args = ap.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    if args.vcd is None:
        args.vcd = os.path.join(script_dir, 'live.vcd')
    if args.surfer is None:
        args.surfer = os.path.abspath(os.path.join(script_dir, '..', '..',
                                                   'surfer_win', 'surfer.exe'))
    if args.root is None:
        root = os.path.abspath(os.path.join(script_dir, '..', '..'))
    else:
        root = os.path.abspath(args.root)
    modules, features = load_map(root)
    print(f'[hub] scanned: {len(modules)} modules, {len(features)} features')
    for k, (short, label) in sorted(modules.items()):
        print(f'  mod {k:#04x} {short:<8} {label}')

    if args.input:
        data = open(args.input, 'rb').read()
        lines = []
        recs = []
        p2 = FrameParser(on_log=lines.append,
                         on_wave=lambda r: recs.append(r))
        p2.feed(data)
        for line in lines:
            sys.stdout.write(line.decode('ascii', errors='replace'))
        w = GzVcdWriter(args.vcd, modules, features)
        w.export(recs)
        print(f'[hub] offline done: {len(recs)} records -> {args.vcd}')
        return

    import queue
    log_queue = queue.Queue()
    try:
        run_gui(log_queue, args, modules, features)
    finally:
        print('[hub] closed')


if __name__ == '__main__':
    main()
