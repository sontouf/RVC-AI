param(
  [string]$BuildDir = "build",
  [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot\..

$sim = Join-Path $BuildDir "$Config\rvc_sim.exe"
if (-not (Test-Path $sim)) {
  $sim = Join-Path $BuildDir "rvc_sim.exe"
}

python system_tests/run_all.py --sim $sim
