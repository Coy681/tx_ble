#!/usr/bin/env python3
"""debug_hub_qt.py - tx_ble debug platform, Qt frontend.

Same core as debug_hub.py (A5 frame parser, X-macro id mapping, VCD
writer, Surfer launcher) with a PySide6 UI: dark theme, split log/wave
panes, serial toolbar, hex/ascii send.

Usage:
  python debug_hub_qt.py
  python debug_hub_qt.py --port COM7 --baud 1000000
  python debug_hub_qt.py --input dump.bin             # offline -> vcd

Requires: pyserial, PySide6
"""

import argparse
import gzip
import os
import re
import struct
import subprocess
import sys

SYNC = 0xA5
TYPE_LOG = 0x00
TYPE_WAVE = 0x01
RECORD_SIZE = 12
MAX_PAYLOAD = 236
EVT_PROBE = 0xFF

# --------------------------------------------------------------- id mapping
# (identical to debug_hub.py - keep both in sync or import when packaged)

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
    """Gzip-compressed VCD with hierarchy and lazy-but-headered signals.

    Surfer opens .vcd.gz natively (FST-class file size, no C deps).
    Hierarchy: $scope module <module> > $scope module <feature> >
    signal lanes. Call export(records) once; signals are declared only
    for tracks present in the data, inside $enddefinitions.
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
            f.write('$date now $end\n$version debug_hub_qt $end\n')
            f.write('$timescale 1us $end\n$scope module tx_ble $end\n')
            sym_of, width_of = {}, {}
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
                width_of[(mod_l, feat_l, inst, lane)] = width
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
                del self.buf[:i + 1]
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


# --------------------------------------------------------------- serial worker

def make_worker(qt):
    """SerialWorker class factory (Qt-bound classes need a live import)."""
    from PySide6.QtCore import QThread, Signal

    class SerialWorker(QThread):
        log_line = Signal(str)
        wave_rec = Signal(object)
        status_line = Signal(str)

        def __init__(self, port, baud, raw_dump):
            super().__init__()
            self.port = port
            self.baud = baud
            self.raw_dump = raw_dump
            self._stop = False
            self.ser = None
            self.parser = FrameParser(
                on_log=lambda p: self.log_line.emit(
                    p.decode('ascii', errors='replace')),
                on_wave=lambda rec: self.wave_rec.emit(rec))

        def write(self, payload):
            """Called from the GUI thread; serial write is quick."""
            if self.ser is not None:
                try:
                    self.ser.write(payload)
                    return True
                except Exception as e:
                    self.status_line.emit(f'send failed: {e}')
            return False

        def run(self):
            import serial
            try:
                self.ser = serial.Serial(self.port, self.baud, timeout=0.05)
            except Exception as e:
                self.status_line.emit(f'open failed: {e}')
                return
            n = 0
            while not self._stop:
                try:
                    data = self.ser.read(8192)
                except Exception:
                    break
                if data:
                    if self.raw_dump:
                        n += len(data)
                        sys.stderr.write(f'[raw {n:7d}B] {data[:64].hex(" ")}\n')
                    self.parser.feed(data)
            self.ser.close()
            self.ser = None

    return SerialWorker


# --------------------------------------------------------------- GUI

DARK = """
QWidget { background: #1e1f22; color: #d4d4d4; font-size: 13px; }
QPlainTextEdit { background: #141517; color: #d4d4d4;
                 font-family: Consolas; font-size: 12px; border: none; }
QLineEdit, QComboBox { background: #2a2b2f; border: 1px solid #3c3d42;
                       border-radius: 4px; padding: 3px 6px; }
QComboBox QAbstractItemView { background: #2a2b2f;
                              selection-background-color: #094771; }
QPushButton { background: #2a2b2f; border: 1px solid #3c3d42;
              border-radius: 4px; padding: 4px 14px; }
QPushButton:hover { background: #35363b; }
QPushButton:pressed { background: #094771; }
QPushButton:checked { background: #094771; color: white; }
QSplitter::handle { background: #33343a; width: 3px; }
QStatusBar { background: #18191c; color: #888; }
"""

LOG_MAX_BLOCKS = 20000
WAVE_MAX_BLOCKS = 20000

STYLE_ERR = '<span style="color:#e74c3c">%s</span>'
STYLE_WARN = '<span style="color:#e67e22">%s</span>'
STYLE_OK = '<span style="color:#2ecc71">%s</span>'
STYLE_LVL = '<span style="color:#4fc3f7">%s</span>'
STYLE_NUM = '<span style="color:#ffd54f">%s</span>'


def run_gui(args, modules, features):
    from PySide6.QtCore import Qt
    from PySide6.QtWidgets import (
        QApplication, QMainWindow, QWidget, QPlainTextEdit, QHBoxLayout,
        QVBoxLayout, QPushButton, QComboBox, QLabel, QLineEdit, QSplitter)

    import serial.tools.list_ports

    SerialWorker = make_worker(None)

    app = QApplication(sys.argv)
    app.setStyleSheet(DARK)
    win = QMainWindow()
    win.setWindowTitle('tx_ble debug hub')
    win.resize(1400, 700)

    central = QWidget()
    win.setCentralWidget(central)
    vbox = QVBoxLayout(central)
    vbox.setContentsMargins(6, 6, 6, 6)
    vbox.setSpacing(6)

    # ---------------- toolbar ----------------
    bar = QHBoxLayout()
    cb_port = QComboBox()
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

    btn_conn = QPushButton('connect')
    btn_conn.setFixedWidth(96)
    lbl_conn = QLabel('disconnected')

    ent_send = QLineEdit()
    ent_send.setPlaceholderText('send... (Enter=tx)')
    cb_hex = QPushButton('hex')
    cb_hex.setCheckable(True)
    cb_hex.setFixedWidth(48)
    btn_tx = QPushButton('tx')
    btn_tx.setFixedWidth(44)

    edt_filter = QComboBox()
    edt_filter.setEditable(True)
    edt_filter.setFixedWidth(150)
    edt_filter.lineEdit().setPlaceholderText('filter')

    btn_surfer = QPushButton('surfer')
    btn_clear = QPushButton('clear')
    lbl_wave = QLabel('wave: 0 records')

    bar.addWidget(QLabel('port:'))
    bar.addWidget(cb_port)
    bar.addWidget(QLabel('baud:'))
    bar.addWidget(cb_baud)
    bar.addWidget(btn_conn)
    bar.addWidget(lbl_conn)
    bar.addSpacing(12)
    bar.addWidget(QLabel('send:'))
    bar.addWidget(ent_send, 1)
    bar.addWidget(cb_hex)
    bar.addWidget(btn_tx)
    bar.addSpacing(12)
    bar.addWidget(QLabel('filter:'))
    bar.addWidget(edt_filter)
    bar.addSpacing(12)
    bar.addWidget(btn_surfer)
    bar.addWidget(btn_clear)
    bar.addWidget(lbl_wave)
    vbox.addLayout(bar)

    # ---------------- splitter: log | wave ----------------
    split = QSplitter()
    txt_log = QPlainTextEdit()
    txt_log.setReadOnly(True)
    txt_log.setMaximumBlockCount(LOG_MAX_BLOCKS)
    txt_wave = QPlainTextEdit()
    txt_wave.setReadOnly(True)
    txt_wave.setMaximumBlockCount(WAVE_MAX_BLOCKS)
    split.addWidget(txt_log)
    split.addWidget(txt_wave)
    split.setSizes([700, 700])
    vbox.addWidget(split, 1)

    win.statusBar().showMessage(
        f'id map: {len(modules)} modules, {len(features)} features')

    # ---------------- state + wiring ----------------
    records = []
    worker = [None]

    def esc(s):
        return (s.replace('&', '&amp;').replace('<', '&lt;')
                .replace('>', '&gt;'))

    def append_log(line):
        low = line.lower()
        if 'err' in low or 'fail' in low:
            style = STYLE_ERR
        elif 'warn' in low:
            style = STYLE_WARN
        elif 'ok' in low or 'ready' in low or 'done' in low:
            style = STYLE_OK
        else:
            style = '%s'
        txt_log.appendHtml(style % esc(line.rstrip('\r\n')))

    def on_log_line(line):
        append_log(line)

    def on_wave_record(rec):
        records.append(rec)
        ts, wid, val = rec
        typ, mod, feat, inst, evt = decode_id(wid)
        base = track_name(mod, feat, inst, evt, modules, features)
        if typ == 1:
            line = f'{ts:>10}  {wid:08X}  {"begin" if val else "end  "}  {base}'
            style = STYLE_LVL
        else:
            line = f'{ts:>10}  {wid:08X}  {val:<10}  {base}/num'
            style = STYLE_NUM
        txt_wave.appendHtml(style % esc(line))
        lbl_wave.setText(f'wave: {len(records)} records')

    def on_status(line):
        lbl_conn.setText(line)

    def toggle_connect():
        if worker[0] is not None:
            worker[0]._stop = True
            worker[0].wait(1000)
            worker[0] = None
            btn_conn.setText('connect')
            btn_conn.setStyleSheet('background:#27ae60;color:white;border:none;'
                                   'border-radius:4px;padding:4px 14px;')
            lbl_conn.setText('disconnected')
            return
        try:
            w = SerialWorker(cb_port.currentText(),
                             int(cb_baud.currentText()), args.raw_dump)
        except Exception as e:
            lbl_conn.setText(f'bad config: {e}')
            return
        w.log_line.connect(on_log_line)
        w.wave_rec.connect(on_wave_record)
        w.status_line.connect(on_status)
        w.start()
        worker[0] = w
        btn_conn.setText('disconnect')
        btn_conn.setStyleSheet('background:#c0392b;color:white;border:none;'
                               'border-radius:4px;padding:4px 14px;')
        lbl_conn.setText(f'{cb_port.currentText()} @ {cb_baud.currentText()}')

    def do_send():
        if worker[0] is None or not ent_send.text():
            return
        text = ent_send.text()
        if cb_hex.isChecked():
            try:
                payload = bytes.fromhex(text.replace(' ', '').replace(',', ''))
                if not payload:
                    raise ValueError('empty')
            except ValueError as e:
                lbl_conn.setText(f'bad hex: {e}')
                return
        else:
            payload = text.encode('ascii', errors='replace')
        if worker[0].write(payload + b'\r\n'):
            ent_send.clear()

    def do_surfer():
        if not records:
            lbl_wave.setText('no wave records')
            return
        w = GzVcdWriter(args.vcd, modules, features)
        w.export(records)
        # verify file completeness before launching the viewer
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
                subprocess.Popen(
                    [args.surfer, os.path.abspath(args.vcd)],
                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            except OSError as e:
                lbl_wave.setText(f'surfer launch failed: {e}')

    def do_clear():
        records.clear()
        txt_wave.clear()
        lbl_wave.setText('wave: 0 records')

    ent_send.returnPressed.connect(do_send)

    btn_conn.clicked.connect(toggle_connect)
    btn_tx.clicked.connect(do_send)
    btn_surfer.clicked.connect(do_surfer)
    btn_clear.clicked.connect(do_clear)

    win.show()
    return app


def main():
    ap = argparse.ArgumentParser(description='tx_ble debug hub (Qt)')
    ap.add_argument('--port', help='serial port to preselect, e.g. COM7')
    ap.add_argument('--input', help='raw capture file for offline conversion')
    ap.add_argument('--baud', type=int, default=1000000)
    ap.add_argument('--root', default=None)
    ap.add_argument('--vcd', default=None)
    ap.add_argument('--surfer', default=None)
    ap.add_argument('--no-surfer', action='store_true')
    ap.add_argument('--raw-dump', action='store_true')
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

    if args.input:
        w = GzVcdWriter(args.vcd, modules, features)
        data = open(args.input, 'rb').read()
        lines = []
        recs = []
        p2 = FrameParser(on_log=lines.append,
                         on_wave=lambda r: recs.append(r))
        p2.feed(data)
        for line in lines:
            sys.stdout.write(line.decode('ascii', errors='replace'))
        n = w.export(recs)
        print(f'[hub] offline done: {n} records -> {args.vcd}')
        return

    app = run_gui(args, modules, features)
    sys.exit(app.exec())


if __name__ == '__main__':
    main()
