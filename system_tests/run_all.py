#!/usr/bin/env python3
"""Run all system scenarios against rvc_sim; assert per-json requirements and global state coverage."""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys

ALL_DISPLAY_STATES = [
    "Idle",
    "Cleaning_Forward",
    "Cleaning_Forward_Boost",
    "Maneuver_Stop",
    "Maneuver_Turn",
    "Maneuver_Reverse",
    "Session_Stopping",
]


def find_rvc_sim(repo_root: str) -> str:
    cands = [
        os.path.join(repo_root, "build", "rvc_sim"),
        os.path.join(repo_root, "build", "Release", "rvc_sim.exe"),
        os.path.join(repo_root, "build", "Debug", "rvc_sim.exe"),
    ]
    for p in cands:
        if os.path.isfile(p):
            return p
    return ""


def run_one(sim_bin: str, scenario_path: str) -> tuple[int, list[dict]]:
    proc = subprocess.run(
        [sim_bin, "--scenario", scenario_path, "--jsonl"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    lines = [ln for ln in proc.stdout.splitlines() if ln.strip()]
    trace = []
    for ln in lines:
        try:
            trace.append(json.loads(ln))
        except json.JSONDecodeError:
            continue
    return proc.returncode, trace


def load_required(path: str) -> list[str]:
    with open(path, encoding="utf-8") as f:
        doc = json.load(f)
    return doc.get("required_states") or []


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    ap.add_argument("--sim", default="", help="Path to rvc_sim binary")
    args = ap.parse_args()
    root = args.root
    sim = args.sim or find_rvc_sim(root)
    if not sim:
        print("rvc_sim not found. Build with CMake first.", file=sys.stderr)
        return 2

    maps_dir = os.path.join(root, "system_tests", "maps")
    files = sorted(
        f for f in os.listdir(maps_dir) if f.endswith(".json")
    )
    if len(files) < 30:
        print(f"Expected >=30 scenario files, found {len(files)}", file=sys.stderr)
        return 2

    failed = False
    union: set[str] = set()
    for name in files:
        path = os.path.join(maps_dir, name)
        code, trace = run_one(sim, path)
        seen = {row.get("display", "") for row in trace}
        union |= seen
        req = load_required(path)
        if req:
            miss = [s for s in req if s not in seen]
            if miss:
                print(f"FAIL {name}: missing states in this run {miss}", file=sys.stderr)
                failed = True
        if code != 0:
            err = (proc.stderr or "").strip()
            if err:
                print(err, file=sys.stderr)
            print(f"FAIL {name}: rvc_sim exit {code}", file=sys.stderr)
            failed = True
        else:
            print(f"OK   {name}  (trace lines {len(trace)})")

    miss_global = [s for s in ALL_DISPLAY_STATES if s not in union]
    if miss_global:
        print(f"FAIL global coverage missing: {miss_global}", file=sys.stderr)
        failed = True
    else:
        print("Global display-state coverage: OK", file=sys.stderr)

    print(json.dumps({"display_states_seen": sorted(union), "scenario_count": len(files)}, indent=2))
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
