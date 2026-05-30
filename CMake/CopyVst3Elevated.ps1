# Copy VST3 bundle to system folder (e.g. C:\Program Files\Common Files\VST3).
# Run elevated (UAC) so the build can write to Program Files.
# Usage: .\CopyVst3Elevated.ps1 -ArgsFile "C:\path\to\vst3_copy_args.txt"
#   Args file has two lines: line 1 = source .vst3 path, line 2 = dest parent path.

param(
    [Parameter(Mandatory = $true)]
    [string]$ArgsFile
)

$lines = Get-Content -LiteralPath $ArgsFile -Encoding UTF8
if ($lines.Count -lt 2) {
    Write-Error "Args file must contain 2 lines (Source, DestParent)."
    exit 1
}
$Source = $lines[0].Trim() -replace '^\uFEFF', ''
$DestParent = $lines[1].Trim()

$ErrorActionPreference = "Stop"
$name = [System.IO.Path]::GetFileName($Source)
$dest = Join-Path $DestParent $name

if (-not (Test-Path $Source)) {
    Write-Error "Source not found: $Source"
    exit 1
}

if (Test-Path $dest) {
    Remove-Item $dest -Recurse -Force
}
Copy-Item -Path $Source -Destination $dest -Recurse -Force
exit 0
