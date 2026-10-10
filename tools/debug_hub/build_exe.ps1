# -*- coding: utf-8 -*-
"""build_exe.ps1 - 打包 debug_hub.exe（PyInstaller onefile + HTX 图标）。

用法：powershell -File tools\debug_hub\build_exe.ps1
产物：tools\debug_hub\debug_hub.exe（单文件，含 PySide6/pyserial/htx.ico）
依赖：pip install pyinstaller pyserial PySide6
"""
$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
python -m PyInstaller --noconfirm --onefile --windowed `
    --name debug_hub --icon htx.ico --add-data "htx.ico;." `
    --hidden-import serial.tools.list_ports debug_hub_qt.py
Copy-Item dist\debug_hub.exe . -Force
Remove-Item build, dist, debug_hub.spec -Recurse -Force -ErrorAction SilentlyContinue
Write-Host "built: $PSScriptRoot\debug_hub.exe"
