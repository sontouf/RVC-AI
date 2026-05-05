#!/usr/bin/env python3
"""Fail if lcov summary for lines/branches is below required percent (default 100)."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys


def parse_summary(text: str) -> tuple[float, float]:
    line_pct = branch_pct = None
    for line in text.splitlines():
        m = re.match(r"\s*lines\.*:\s*([\d.]+)%", line, re.I)
        if m:
            line_pct = float(m.group(1))
        m = re.match(r"\s*branches\.*:\s*([\d.]+)%", line, re.I)
        if m:
            branch_pct = float(m.group(1))
    if line_pct is None or branch_pct is None:
        raise SystemExit(f"Could not parse lcov summary:\n{text}")
    return line_pct, branch_pct


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("info_file", help=".info file (e.g. coverage-src.info)")
    ap.add_argument("--lines", type=float, default=100.0)
    ap.add_argument("--branches", type=float, default=100.0)
    args = ap.parse_args()
    cmd = [
        "lcov",
        "--summary",
        args.info_file,
        "--rc",
        "branch_coverage=1",
    ]
    proc = subprocess.run(cmd, capture_output=True, text=True)
    sys.stdout.write(proc.stdout)
    sys.stderr.write(proc.stderr)
    if proc.returncode != 0:
        return proc.returncode
    line_pct, branch_pct = parse_summary(proc.stdout)
    ok = True
    if line_pct + 1e-6 < args.lines:
        print(f"FAIL: line coverage {line_pct}% < {args.lines}%", file=sys.stderr)
        ok = False
    if branch_pct + 1e-6 < args.branches:
        print(f"FAIL: branch coverage {branch_pct}% < {args.branches}%", file=sys.stderr)
        ok = False
    if ok:
        print(f"OK: lines={line_pct}% branches={branch_pct}% (thresholds lines>={args.lines}% branches>={args.branches}%)")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
