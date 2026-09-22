@echo off
setlocal
cd /d "%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\run_spann_local.ps1" -Mode model
if errorlevel 1 (
  echo.
  echo AW2 ML failed to start. See runtime\bridge.stderr.log for details.
  pause
)
