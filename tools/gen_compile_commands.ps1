# Generates compile_commands.json for clangd from the Eclipse .cproject settings.
# Usage: powershell -File tools\gen_compile_commands.ps1
$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $MyInvocation.MyCommand.Path | Split-Path -Parent

# Include paths mirrored from .cproject
$telinkTc = "C:\Users\tianxiang.huang\.Telink_Tools\V53xf_Windows\toolchains\nds32le-elf-mculib-v5f\riscv32-elf"
$includes = @(
    $root,
    (Join-Path $root "platform\telink\TLSR9528\drivers"),
    (Join-Path $root "platform\telink\common"),
    (Join-Path $telinkTc "include")
)

$incFlags = $includes | ForEach-Object { "-I$($_ -replace '\\','/')" }

# Target/flags from .cproject (d25f = rv32imc, dsp ext ignored by clangd)
$flags = @(
    "--target=riscv32-unknown-elf",
    "-march=rv32imc",
    "-std=c99",
    "-fno-strict-aliasing",
    "-D__D25F__",
    "-Wall"
) + $incFlags

$entries = @()
Get-ChildItem -Path $root -Recurse -Filter *.c | Where-Object { $_.FullName -notmatch '\\\.git\\' } | ForEach-Object {
    $file = $_.FullName -replace '\\','/'
    $entries += [ordered]@{
        directory = $root -replace '\\','/'
        arguments = @("clang") + $flags + @("-c", $file)
        file      = $file
    }
}

$out = Join-Path $root "compile_commands.json"
$entries | ConvertTo-Json -Depth 5 | Set-Content -Path $out -Encoding UTF8
Write-Host "Wrote $out with $($entries.Count) entries"
