param(
    [ValidateSet('stub', 'model')]
    [string]$Mode = 'model',
    [string]$Python = 'X:\dev\awbw\.venv\Scripts\python.exe',
    [string]$Mgba = 'X:\dev\mgba-dev\current\mGBA-build-2026-09-19-win64-9139-3a5bc24629867576b0fb576a5d5a21d3b3d6b576\mGBA.exe',
    [string]$SaveState = '',
    [switch]$Trace
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$rom = Join-Path $root 'build-mod\aw2bhr-spann-local.gba'
$config = Join-Path $root 'spann-bridge.example.json'
$script = Join-Path $root 'tools\mgba\bootstrap.lua'
$runtime = Join-Path $root 'runtime'

foreach ($required in @($Python, $Mgba, $config, $script)) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "Required file is missing: $required"
    }
}
if (-not (Test-Path -LiteralPath $rom -PathType Leaf)) {
    & $Python (Join-Path $root 'tools\spann_bridge\build_mod.py')
    if ($LASTEXITCODE -ne 0) { throw 'Modified ROM build failed.' }
}
if ($SaveState -and -not (Test-Path -LiteralPath $SaveState -PathType Leaf)) {
    throw "Savestate is missing: $SaveState"
}

New-Item -ItemType Directory -Path $runtime -Force | Out-Null
$bridgeArgs = @('tools\spann_bridge\bridge.py', '--config', ('"' + $config + '"'), '--mode', $Mode)
$bridge = Start-Process -FilePath $Python -ArgumentList $bridgeArgs -WorkingDirectory $root `
    -RedirectStandardOutput (Join-Path $runtime 'bridge.stdout.log') `
    -RedirectStandardError (Join-Path $runtime 'bridge.stderr.log') `
    -WindowStyle Hidden -PassThru

$env:AW2_SPANN_ROOT = $root.Replace('\', '/')
$env:AW2_SPANN_IPC = $runtime.Replace('\', '/')
$env:AW2_SPANN_TRACE = if ($Trace) { '1' } else { '0' }
$mgbaArgs = @('--script', ('"' + $script + '"'))
if ($SaveState) {
    $mgbaArgs += @('-t', ('"' + (Resolve-Path -LiteralPath $SaveState).Path + '"'))
}
$mgbaArgs += ('"' + $rom + '"')

try {
    $emulator = Start-Process -FilePath $Mgba -ArgumentList $mgbaArgs -WorkingDirectory $root -PassThru
    $emulator.WaitForExit()
} finally {
    Stop-Process -Id $bridge.Id -Force -ErrorAction SilentlyContinue
}
