#!/usr/bin/env python3
"""
GUI helper for visualizing `rvc_sim --jsonl` traces.

This tool intentionally duplicates no control logic; it only renders what the C++ simulator prints.
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Tuple


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Animate RVC grid scenarios using rvc_sim JSONL output.")
    parser.add_argument("--sim", required=True, help="Path to rvc_sim executable")
    parser.add_argument("--scenario", required=True, help="Path to scenario JSON consumed by rvc_sim")
    parser.add_argument("--delay-ms", type=int, default=120, help="Milliseconds between frames")
    return parser.parse_args()


def load_spec(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def collect_trace(sim_exe: Path, scenario: Path) -> Tuple[List[dict], dict]:
    proc = subprocess.run(
        [str(sim_exe), "--jsonl", str(scenario)],
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        raise RuntimeError(proc.stderr.strip() or "rvc_sim failed")

    lines = [ln for ln in proc.stdout.splitlines() if ln.strip()]
    if not lines:
        raise RuntimeError("No stdout from rvc_sim")

    frames: List[dict] = []
    summary: Dict | None = None
    for line in lines:
        obj = json.loads(line)
        if "drive" in obj:
            frames.append(obj)
        else:
            summary = obj

    if summary is None:
        raise RuntimeError("Missing summary line from rvc_sim")

    return frames, summary


def run_headless_smoke() -> None:
    # Import lazily so CI machines without Tk still succeed on earlier steps.
    import tkinter as _tk  # noqa: F401


def main() -> int:
    if os.environ.get("RVC_HEADLESS") == "1":
        try:
            run_headless_smoke()
        except ImportError:
            print("tkinter not installed; skipping GUI import check.", file=sys.stderr)
        return 0

    args = parse_args()
    sim_exe = Path(args.sim)
    scenario = Path(args.scenario)

    spec = load_spec(scenario)
    grid_rows = spec["grid"]

    frames, summary = collect_trace(sim_exe, scenario)

    import tkinter as tk

    cell_px = 36
    rows = len(grid_rows)
    cols = max(len(row) for row in grid_rows) if rows else 0

    root = tk.Tk()
    root.title("RVC Grid GUI (trace replay)")

    canvas = tk.Canvas(root, width=cols * cell_px, height=rows * cell_px, highlightthickness=0)
    canvas.pack()

    status = tk.StringVar()
    status_bar = tk.Label(root, textvariable=status, anchor="w")
    status_bar.pack(fill="x")

    def draw_frame(frame: dict | None) -> None:
        canvas.delete("all")
        for r, row in enumerate(grid_rows):
            for c, ch in enumerate(row):
                x0 = c * cell_px
                y0 = r * cell_px
                x1 = x0 + cell_px
                y1 = y0 + cell_px
                fill = "#444444" if ch == "#" else "#f5f5f5"
                canvas.create_rectangle(x0, y0, x1, y1, fill=fill, outline="#bbbbbb")

        if frame is not None:
            rr = int(frame["row"])
            cc = int(frame["col"])
            x0 = cc * cell_px + 4
            y0 = rr * cell_px + 4
            x1 = x0 + cell_px - 8
            y1 = y0 + cell_px - 8
            canvas.create_oval(x0, y0, x1, y1, fill="#1e88e5", outline="#0d47a1")

        if frame is None:
            status.set(f"finished | summary={summary}")
        else:
            status.set(
                f"drive={frame.get('drive')} cleaner={frame.get('cleaner')} | summary={summary}"
            )

    idx = 0

    def tick() -> None:
        nonlocal idx
        if idx < len(frames):
            draw_frame(frames[idx])
            idx += 1
            root.after(args.delay_ms, tick)
        else:
            draw_frame(None)

    tick()
    root.mainloop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
