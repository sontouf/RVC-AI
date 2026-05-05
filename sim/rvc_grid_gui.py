#!/usr/bin/env python3
"""
Grid simulator GUI: loads JSON scenarios, runs `rvc_sim --jsonl`, animates the robot,
and shows coverage checklist for RVC display states.
"""
from __future__ import annotations

import json
import os
import subprocess
import sys
import tkinter as tk
from tkinter import filedialog, messagebox, ttk

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAPS = os.path.join(REPO, "system_tests", "maps")

ALL_STATES = [
    "Idle",
    "Cleaning_Forward",
    "Cleaning_Forward_Boost",
    "Maneuver_Stop",
    "Maneuver_Turn",
    "Maneuver_Reverse",
    "Session_Stopping",
]


def find_sim() -> str:
    for p in (
        os.path.join(REPO, "build", "rvc_sim"),
        os.path.join(REPO, "build", "Release", "rvc_sim.exe"),
        os.path.join(REPO, "build", "Debug", "rvc_sim.exe"),
    ):
        if os.path.isfile(p):
            return p
    return ""


class App:
    def __init__(self) -> None:
        self.root = tk.Tk()
        self.root.title("RVC Grid Simulator")
        self.sim_bin = find_sim()
        self.scenario_path: str | None = None
        self.scenario_doc: dict | None = None
        self.trace: list[dict] = []
        self.frame_idx = 0
        self.cell = 36
        self._build()

    def _build(self) -> None:
        top = ttk.Frame(self.root, padding=6)
        top.pack(fill=tk.X)
        ttk.Button(top, text="Open scenario…", command=self._open).pack(side=tk.LEFT, padx=2)
        ttk.Button(top, text="Run selected", command=self._run).pack(side=tk.LEFT, padx=2)
        ttk.Button(top, text="Run ALL system maps", command=self._run_all).pack(side=tk.LEFT, padx=2)
        ttk.Button(top, text="Step ▸", command=self._step).pack(side=tk.LEFT, padx=2)
        self.info = ttk.Label(top, text=self._sim_hint())
        self.info.pack(side=tk.LEFT, padx=8)

        mid = ttk.Frame(self.root)
        mid.pack(fill=tk.BOTH, expand=True)
        self.canvas = tk.Canvas(mid, width=640, height=480, bg="#f4f4f4", highlightthickness=0)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        right = ttk.Frame(mid, padding=6, width=240)
        right.pack(side=tk.RIGHT, fill=tk.Y)
        ttk.Label(right, text="Display state checklist").pack(anchor=tk.W)
        self.check_vars: dict[str, tk.Variable] = {}
        for st in ALL_STATES:
            v = tk.BooleanVar(value=False)
            self.check_vars[st] = v
            ttk.Checkbutton(right, text=st, variable=v, state="disabled").pack(anchor=tk.W)
        self.summary = tk.Text(right, height=10, width=32, state=tk.DISABLED)
        self.summary.pack(fill=tk.BOTH, expand=True, pady=6)

    def _sim_hint(self) -> str:
        if self.sim_bin:
            return f"rvc_sim: {self.sim_bin}"
        return "rvc_sim not found — build with CMake (build/rvc_sim)"

    def _open(self) -> None:
        path = filedialog.askopenfilename(
            initialdir=MAPS if os.path.isdir(MAPS) else REPO,
            filetypes=[("JSON", "*.json"), ("All", "*.*")],
        )
        if path:
            self._load_path(path)

    def _load_path(self, path: str) -> None:
        with open(path, encoding="utf-8") as f:
            self.scenario_doc = json.load(f)
        self.scenario_path = path
        self.trace = []
        self.frame_idx = 0
        self._reset_checks()
        self._draw_frame(None)
        self._log(f"Loaded {os.path.basename(path)}")

    def _reset_checks(self) -> None:
        for st, v in self.check_vars.items():
            v.set(False)

    def _run(self) -> None:
        if not self.scenario_path or not self.sim_bin:
            messagebox.showerror("Run", "Need scenario and rvc_sim binary")
            return
        proc = subprocess.run(
            [self.sim_bin, "--scenario", self.scenario_path, "--jsonl"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
        if proc.returncode != 0:
            messagebox.showerror("rvc_sim", proc.stderr or f"exit {proc.returncode}")
            return
        lines = [ln for ln in proc.stdout.splitlines() if ln.strip()]
        self.trace = [json.loads(ln) for ln in lines]
        self.frame_idx = 0
        for row in self.trace:
            d = row.get("display")
            if d in self.check_vars:
                self.check_vars[d].set(True)
        self._draw_frame(self.trace[0] if self.trace else None)
        self._log(f"Trace ticks: {len(self.trace)}")

    def _run_all(self) -> None:
        if not self.sim_bin:
            messagebox.showerror("Run all", "rvc_sim not found")
            return
        path = os.path.join(REPO, "system_tests", "run_all.py")
        proc = subprocess.run(
            [sys.executable, path, "--root", REPO, "--sim", self.sim_bin],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
        self._log(proc.stdout)
        if proc.stderr:
            self._log(proc.stderr)
        if proc.returncode != 0:
            messagebox.showerror("System tests", "See log — failures reported")
        else:
            messagebox.showinfo("System tests", "All scenarios OK + global state coverage")

    def _step(self) -> None:
        if not self.trace:
            return
        self.frame_idx = (self.frame_idx + 1) % len(self.trace)
        self._draw_frame(self.trace[self.frame_idx])

    def _log(self, text: str) -> None:
        self.summary.configure(state=tk.NORMAL)
        self.summary.insert(tk.END, text + "\n")
        self.summary.see(tk.END)
        self.summary.configure(state=tk.DISABLED)

    def _draw_frame(self, row: dict | None) -> None:
        self.canvas.delete("all")
        doc = self.scenario_doc
        if not doc:
            return
        w = int(doc["width"])
        h = int(doc["height"])
        obs = {tuple(p) for p in doc.get("obstacles", [])}
        dust = {(p[0], p[1]): p[2] if len(p) > 2 else 1 for p in doc.get("dust", [])}

        pad = 8
        cw = self.cell
        for r in range(h):
            for c in range(w):
                x0 = pad + c * cw
                y0 = pad + r * cw
                fill = "#ffffff"
                if (r, c) in obs:
                    fill = "#444444"
                elif (r, c) in dust:
                    fill = "#d8c48a"
                self.canvas.create_rectangle(x0, y0, x0 + cw, y0 + cw, fill=fill, outline="#cccccc")

        rr = rc = heading = ""
        disp = sess = ""
        dust_here = ""
        if row:
            rr, rc = int(row["row"]), int(row["col"])
            heading = str(row.get("heading", ""))
            disp = str(row.get("display", ""))
            sess = str(row.get("session", ""))
            dust_here = str(row.get("dust_here", ""))
            cx = pad + rc * cw + cw // 2
            cy = pad + rr * cw + cw // 2
            self.canvas.create_oval(cx - 12, cy - 12, cx + 12, cy + 12, fill="#2a6df4", outline="#103060", width=2)
            self.canvas.create_text(cx, cy, text="R", fill="white", font=("Segoe UI", 10, "bold"))

        self.canvas.create_text(
            10,
            pad + h * cw + 10,
            anchor=tk.W,
            font=("Segoe UI", 9),
            text=f"Pos: ({rr},{rc})  heading={heading}  state={disp}  session={sess}  dust_here={dust_here}",
        )

    def run(self) -> None:
        self.root.mainloop()


if __name__ == "__main__":
    if os.environ.get("RVC_HEADLESS") == "1":
        print("GUI skipped (RVC_HEADLESS=1)")
        sys.exit(0)
    App().run()
