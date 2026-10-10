# debug_hub - tx_ble debug platform

Host tools for the wave software logic analyzer. Full docs:
`DEBUG_PLATFORM.md` (protocol spec, product definition, design decisions,
pitfalls).

## Files
- `debug_hub_qt.py` - main hub (PySide6), the only GUI. v1.1 UI:
  - Top toolbar (global only): port/baud/connect + status pill,
    pause (freeze display, data keeps buffering), record (raw stream
    to `record_*.bin`, replayable), snapshot, clear all
  - 3 panes, each with its own header bar (buttons belong to their pane):
    - **LOG**: local timestamp prefix, keyword coloring, substring /
      `re:` regex filter (filters history too, debounce 300ms, hit count
      n/m), send box at bottom (hex toggle, Enter=tx)
    - **WAVE MONITOR**: one row per track - value | ago (wall time since
      last record) | gate sparkline (last 8 LEVEL windows, red `OPEN!`
      for dangling gate) | track name; clear + surfer export in header
    - **TRACKS**: signal tree (module > feature/instance > lane),
      checkboxes define surfer export scope, all/none bulk toggles
  - Status bar: left = log stats (`log: N lines`), center = file events
    (live.vcd / rec / snap), right = wave stats (`wave: N` | KB/s |
    gate ok / open count)
- `surfer.exe` - MIT waveform viewer (opens the exported live.vcd)
- `analyze_gates.py` - gate sanity analysis over live.vcd
- `test_hub.py` / `test_fst.py` - offline end-to-end regressions
- `snapshots/` - timestamped session archives (log.txt / wave.bin /
  raw.bin / config.txt), rename to annotate bug cases

## Run
    python tools/debug_hub/debug_hub_qt.py
    python tools/debug_hub/debug_hub_qt.py --port COM7 --baud 1000000
    python tools/debug_hub/debug_hub_qt.py --input dump.bin   # offline dump -> VCD
    python tools/debug_hub/test_fst.py

Product definition & roadmap: see `DEBUG_PLATFORM.md` §1
(v1.1 shipped: pane header bars, history filter, log/wave status split,
record; next: replay page, coverage panel).

Firmware side lives in `debug/` (wave.c, wave_id.h, debug_port).
