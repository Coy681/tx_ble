#!/usr/bin/env python3
"""synthesize a debug stream: log frames + wave frames, then run the hub offline."""
import struct, subprocess, sys, os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

def frame(t, payload):
    return bytes([0xA5, t, len(payload)]) + payload

def rec(ts, wid, val):
    return struct.pack('<III', ts, wid, val)

stream = b''
# log frames
stream += frame(0, b'boot ok\r\n')
# wave: WAVE_LEVEL(ADV, LEGACY, inst0, state1, begin) -> id 0x83010001
LEVEL = 0x80000000
stream += frame(1, rec(100, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 1))
stream += frame(0, b'adv started\r\n')
stream += frame(1, rec(250, LEVEL | (0x03 << 24) | (0x01 << 16) | 0x01, 0))
# NUM sample on scheduler/TASK feat
stream += frame(1, rec(300, (0x01 << 24) | (0x01 << 16) | 0x02, 42))
# garbage in the middle to test resync
stream += b'\x00\x00garbage'
stream += frame(0, b'still alive\r\n')
# wave on unknown id (fallback naming)
stream += frame(1, rec(400, 0x00000000, 7))

open('synth_stream.bin', 'wb').write(stream)
r = subprocess.run([sys.executable, 'debug_hub.py', '--input', 'synth_stream.bin',
                    '--vcd', 'synth_test.vcd', '--no-surfer'],
                   capture_output=True, text=True, timeout=60)
print(r.stdout, r.stderr)
print('---- vcd ----')
print(open('synth_test.vcd').read())
