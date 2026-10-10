#!/usr/bin/env python3
"""offline hierarchy test: synth dump -> plain vcd (CRLF) with scopes, only seen tracks."""
import struct, subprocess, sys, os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

def frame(t, payload):
    return bytes([0xA5, t, len(payload)]) + payload

def rec(ts, wid, val):
    return struct.pack('<III', ts, wid, val)

LEVEL = 0x80000000
stream = b''
stream += frame(0, b'boot ok\r\n')
stream += frame(1, rec(100, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 1))   # adv/LEGACY s1 begin
stream += frame(1, rec(250, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 0))
stream += frame(1, rec(300, (0x01 << 24) | (0x01 << 16) | 0x02, 42))          # scheduler/TX num
# garbage
stream += b'\x00\x00zz'
stream += frame(0, b'done\r\n')

open('synth_stream.bin', 'wb').write(stream)
r = subprocess.run([sys.executable, 'debug_hub_qt.py', '--input', 'synth_stream.bin',
                    '--vcd', 'synth_test.vcd', '--no-surfer'],
                   capture_output=True, text=True, timeout=120)
print(r.stdout, r.stderr)
txt = open('synth_test.vcd', encoding='ascii').read()
print(txt)
scopes = [l for l in txt.splitlines() if 'scope' in l or 'var' in l]
print('---- scopes/vars ----')
for s in scopes:
    print(s)
