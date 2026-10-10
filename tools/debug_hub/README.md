# debug_hub - tx_ble debug platform

Host tools for the wave software logic analyzer. Full docs:
`DEBUG_PLATFORM.md` (protocol spec, design decisions, pitfalls).

## Files
- `debug_hub_qt.py` - main hub (PySide6): log pane | wave monitor, serial
  toolbar, hex/ascii send, hierarchical VCD export + Surfer launcher
- `debug_hub.py` - tkinter fallback, same core
- `surfer.exe` - MIT waveform viewer (opens the exported live.vcd)
- `analyze_gates.py` - gate sanity analysis over live.vcd
- `test_hub.py` / `test_fst.py` - offline end-to-end regressions

## Run
    python tools/debug_hub/debug_hub_qt.py
    python tools/debug_hub/debug_hub_qt.py --port COM7 --baud 1000000
    python tools/debug_hub/test_hub.py

Firmware side lives in `debug/` (wave.c, wave_id.h, debug_port).
