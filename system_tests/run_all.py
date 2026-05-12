#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path


def run_one(sim_exe: Path, scenario: Path) -> None:
    spec = json.loads(scenario.read_text(encoding="utf-8"))
    expect = spec.get("expect", {})

    proc = subprocess.run(
        [str(sim_exe), str(scenario)],
        capture_output=True,
        text=True,
    )

    if proc.returncode != 0:
        raise RuntimeError(f"{scenario.name} failed ({proc.returncode}): {proc.stderr.strip()}")

    lines = [ln for ln in proc.stdout.splitlines() if ln.strip()]
    if not lines:
        raise RuntimeError(f"{scenario.name} produced no stdout")

    summary = json.loads(lines[-1])

    expected_result = expect.get("result")
    if expected_result and summary.get("result") != expected_result:
        raise AssertionError(f"{scenario.name} result mismatch: {summary}")

    min_cleaned = expect.get("min_cleaned")
    if min_cleaned is not None and summary.get("cleaned", 0) < min_cleaned:
        raise AssertionError(
            f"{scenario.name} cleaned {summary.get('cleaned')} < required {min_cleaned}: {summary}"
        )


def main() -> int:
    parser = argparse.ArgumentParser(description="Run RVC system scenarios against rvc_sim.")
    parser.add_argument("--sim", required=True, help="Path to rvc_sim executable")
    args = parser.parse_args()

    sim = Path(args.sim)
    if not sim.exists():
        print(f"Simulator binary not found: {sim}", file=sys.stderr)
        return 2

    root = Path(__file__).resolve().parent
    maps = sorted((root / "maps").glob("*.json"))
    if not maps:
        print("No scenarios found under system_tests/maps", file=sys.stderr)
        return 2

    failures = 0
    for scenario in maps:
        try:
            run_one(sim, scenario)
            print(f"[PASS] {scenario.name}")
        except (AssertionError, RuntimeError, json.JSONDecodeError) as exc:
            failures += 1
            print(f"[FAIL] {scenario.name}: {exc}", file=sys.stderr)

    print(f"Finished {len(maps)} scenarios with {failures} failures.")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
