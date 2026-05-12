#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

SIM="$ROOT/build/rvc_sim"
if [[ ! -x "$SIM" ]]; then
  echo "missing $SIM (build rvc_sim first)" >&2
  exit 2
fi

python3 "$ROOT/system_tests/run_all.py" --sim "$SIM"
