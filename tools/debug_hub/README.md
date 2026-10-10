# debug_hub - tx_ble debug platform (v2.0.0)

Host tools for the wave software logic analyzer. Full docs:
`DEBUG_PLATFORM.md` (protocol spec, product definition, design decisions,
pitfalls).

## Files
- `debug_hub_qt.py` - the GUI (PySide6). v2.0.0 UI:
  - Top toolbar: `⚙ uart` (collapsed config: port/baud/parity/flow) +
    connect + status pill + pause / record / snapshot / clear all
  - Layout: **LOG full-height left column** | right column split
    vertically into **WAVE MONITOR** (top) and **TRACKS** (bottom)
  - **LOG**: local timestamp, coloring, substring/`re:` regex filter
    over history (backing store + 300ms debounce replay, n/m counter),
    save (from backing store, hours-level complete), send box at bottom
  - **LOG hours-level fidelity**: per-connect session file
    `logs/session_*.txt` (unlimited, timestamped, flush every 20 lines)
    + unlimited backing store (`--log-max N` to cap); display keeps
    last 20000 lines
  - **WAVE MONITOR**: 2-level tree (group = module/feature/instance,
    collapsed by default with lane count + aggregated ago; lanes show
    value/ago/gate sparkline, red `OPEN!` for dangling gates)
  - **WAVE header**: surfer export | import (replay a saved .bin) |
    save (12B raw, the "negative film") | clear
  - **TRACKS**: signal tree, checkboxes = surfer export scope, all/none
- `debug_hub.exe` - standalone build (44MB, double-click run; built via
  `build_exe.ps1`, not in git). HTX icon on exe + taskbar
- `htx.ico` / `htx_256.png` - app icon (HTX lettermark + gate waveform)
- `build_exe.ps1` - PyInstaller one-liner build script
- `surfer.exe` - MIT waveform viewer (opens the exported live.vcd)
- `analyze_gates.py` - gate sanity analysis over live.vcd
- `test_hub.py` / `test_fst.py` - offline end-to-end regressions
- `snapshots/` - snapshot archives (log/wave/raw/config)
- `logs/` - per-connect session logs (gitignored)

## Run
    # source (dev)
    python tools/debug_hub/debug_hub_qt.py --port COM7 --baud 1000000
    python tools/debug_hub/debug_hub_qt.py --input dump.bin   # offline dump -> VCD
    # standalone exe (no python needed): copy debug_hub.exe next to the
    # SDK root (for id mapping) + surfer.exe, then double-click
    python tools/debug_hub/test_fst.py    # regression

Product definition & roadmap: `DEBUG_PLATFORM.md` §1
(v2.0.0 shipped: uart-config collapse, LOG/WAVE save+import, session
logs, 2-level monitor tree, vertical layout, exe packaging, HTX icon).

Firmware side lives in `debug/` (wave.c, wave_id.h, debug_port).
