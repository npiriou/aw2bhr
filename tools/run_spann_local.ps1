param(
    [ValidateSet('stub', 'model')]
    [string]$Mode = 'model',
    [string]$Python = 'X:\dev\awbw\.venv\Scripts\python.exe',
    [string]$Mgba = 'X:\dev\mgba-dev\win32-current\mGBA-build-2026-09-19-win32-9139-3a5bc24629867576b0fb576a5d5a21d3b3d6b576\mGBA.exe',
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
$resolvedRuntime = (Resolve-Path -LiteralPath $runtime).Path
Get-ChildItem -LiteralPath $resolvedRuntime -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match '^(request|response)-[0-9a-f]{8}-[0-9a-f]{8}\.(bin|tmp)$' } |
    ForEach-Object { Remove-Item -LiteralPath $_.FullName -Force }
$bridgeStdout = Join-Path $resolvedRuntime 'bridge.stdout.log'
$bridgeStderr = Join-Path $resolvedRuntime 'bridge.stderr.log'
$bridgeArgs = @('tools\spann_bridge\bridge.py', '--config', ('"' + $config + '"'), '--mode', $Mode)
$bridge = Start-Process -FilePath $Python -ArgumentList $bridgeArgs -WorkingDirectory $root `
    -RedirectStandardOutput $bridgeStdout `
    -RedirectStandardError $bridgeStderr `
    -WindowStyle Hidden -PassThru

$ready = $false
for ($attempt = 0; $attempt -lt 300; $attempt++) {
    if ($bridge.HasExited) {
        throw "The local bridge exited during startup. See $bridgeStderr"
    }
    if ((Test-Path -LiteralPath $bridgeStderr) -and
        (Select-String -LiteralPath $bridgeStderr -SimpleMatch 'ready: mode=' -Quiet)) {
        $ready = $true
        break
    }
    Start-Sleep -Milliseconds 100
}
if (-not $ready) {
    Stop-Process -Id $bridge.Id -Force -ErrorAction SilentlyContinue
    throw "The local bridge did not become ready within 30 seconds. See $bridgeStderr"
}

$env:AW2_SPANN_ROOT = $root.Replace('\', '/')
$env:AW2_SPANN_IPC = $runtime.Replace('\', '/')
$env:AW2_SPANN_TRACE = if ($Trace) { '1' } else { '0' }
$env:AW2_AGENT_ROOT = $env:AW2_SPANN_ROOT
$env:AW2_AGENT_IPC = $env:AW2_SPANN_IPC
$env:AW2_AGENT_TRACE = $env:AW2_SPANN_TRACE
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
