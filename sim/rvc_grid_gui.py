#!/usr/bin/env python3
"""
RVC grid studio: edit obstacles / dust / robot pose, then step through ticks.
Control logic stays in C++ (`rvc_tickd` JSON line protocol). Optional `--replay` uses `rvc_sim --jsonl`.
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk
from typing import Dict, List, Optional, Set, Tuple


# --- palette (slate / cyan accent) ---
COL_BG = "#0f172a"
COL_PANEL = "#1e293b"
COL_TEXT = "#e2e8f0"
COL_MUTED = "#94a3b8"
COL_CELL = "#334155"
COL_CELL_HI = "#475569"
COL_WALL = "#020617"
COL_WALL_EDGE = "#64748b"
COL_DUST = "#fbbf24"
COL_DUST_RING = "#f59e0b"
COL_CLEAN_GLOW = "#22c55e"
COL_ROBOT = "#38bdf8"
COL_ROBOT_STROKE = "#e0f2fe"
COL_ARROW = "#fb923c"
COL_GRID_LINE = "#475569"


def repo_root() -> Path:
    return Path(__file__).resolve().parent.parent


def default_tickd_paths() -> List[Path]:
    root = repo_root()
    return [
        root / "build" / "rvc_tickd.exe",
        root / "build" / "Release" / "rvc_tickd.exe",
        root / "build" / "Debug" / "rvc_tickd.exe",
        root / "build" / "rvc_tickd",
    ]


def find_tickd(explicit: Optional[Path]) -> Optional[Path]:
    if explicit is not None:
        return explicit if explicit.is_file() else None
    for p in default_tickd_paths():
        if p.is_file():
            return p
    return None


class TickDaemon:
    """One line JSON in / one line JSON out subprocess."""

    def __init__(self, exe: Path) -> None:
        self.exe = exe
        self.proc: Optional[subprocess.Popen[str]] = None

    def ensure_running(self) -> None:
        if self.proc is not None and self.proc.poll() is None:
            return
        self.proc = subprocess.Popen(
            [str(self.exe)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1,
        )

    def close(self) -> None:
        if self.proc is None:
            return
        try:
            if self.proc.stdin:
                self.proc.stdin.write(json.dumps({"op": "quit"}) + "\n")
                self.proc.stdin.flush()
        except BrokenPipeError:
            pass
        self.proc.wait(timeout=3)
        self.proc = None

    def request(self, payload: Dict) -> Dict:
        self.ensure_running()
        assert self.proc is not None and self.proc.stdin and self.proc.stdout
        line = json.dumps(payload, separators=(",", ":"))
        self.proc.stdin.write(line + "\n")
        self.proc.stdin.flush()
        out = self.proc.stdout.readline()
        if not out:
            err = self.proc.stderr.read() if self.proc.stderr else ""
            raise RuntimeError(f"tickd closed stdout. stderr: {err!r}")
        return json.loads(out)


class InteractiveApp:
    def __init__(self, root: tk.Tk, tickd: Path) -> None:
        self.root = root
        self.tickd_path = tickd
        self.daemon = TickDaemon(tickd)

        self.rows = 9
        self.cols = 11
        self.grid: List[List[str]] = [["." for _ in range(self.cols)] for _ in range(self.rows)]
        self.dust: Set[Tuple[int, int]] = set()
        self.robot: Tuple[int, int] = (self.rows // 2, self.cols // 2)
        self.heading = tk.StringVar(value="E")
        self.tool = tk.StringVar(value="robot")

        self.last_state: Optional[Dict] = None
        self.engine_dirty = True
        self._heading_programmatic = False

        self.cell_px = 46
        self.pad = 28

        self.auto_run = tk.BooleanVar(value=False)
        self.auto_delay = tk.IntVar(value=280)

        self._build_ui()
        self.redraw_all()

    def _build_ui(self) -> None:
        self.root.title("RVC Grid Studio")
        self.root.configure(bg=COL_BG)
        self.root.minsize(920, 640)

        style = ttk.Style()
        if sys.platform == "win32":
            style.theme_use("vista")
        else:
            style.theme_use("clam")
        style.configure("TFrame", background=COL_BG)
        style.configure("TLabelframe", background=COL_PANEL, foreground=COL_TEXT)
        style.configure("TLabelframe.Label", background=COL_PANEL, foreground=COL_TEXT)
        style.configure("TLabel", background=COL_PANEL, foreground=COL_TEXT)
        style.configure("TRadiobutton", background=COL_PANEL, foreground=COL_TEXT)
        style.configure("TCheckbutton", background=COL_PANEL, foreground=COL_TEXT)
        style.configure("TButton", padding=6)
        style.configure("Title.TLabel", background=COL_BG, foreground=COL_TEXT, font=("Segoe UI", 16, "bold"))
        style.configure("Sub.TLabel", background=COL_BG, foreground=COL_MUTED, font=("Segoe UI", 10))

        outer = ttk.Frame(self.root, padding=12)
        outer.pack(fill="both", expand=True)

        ttk.Label(outer, text="RVC Grid Studio", style="Title.TLabel").pack(anchor="w")
        ttk.Label(
            outer,
            text="편집 후 [엔진 동기화] 또는 [한 틱] — 실제 제어는 C++ rvc_tickd",
            style="Sub.TLabel",
        ).pack(anchor="w", pady=(0, 8))

        body = ttk.Frame(outer)
        body.pack(fill="both", expand=True)

        self.canvas = tk.Canvas(
            body,
            width=self.cols * self.cell_px + 2 * self.pad,
            height=self.rows * self.cell_px + 2 * self.pad,
            bg=COL_BG,
            highlightthickness=0,
        )
        self.canvas.pack(side="left", fill="both", expand=True)
        self.canvas.bind("<Button-1>", self.on_canvas_click)

        panel = ttk.LabelFrame(body, text=" 도구 & 실행 ", padding=12)
        panel.pack(side="right", fill="y", padx=(12, 0))

        for text, val in [
            ("로봇 위치", "robot"),
            ("장애물", "wall"),
            ("바닥", "floor"),
            ("먼지", "dust"),
            ("먼지 지우기", "erase_dust"),
        ]:
            ttk.Radiobutton(panel, text=text, value=val, variable=self.tool).pack(anchor="w", pady=2)

        ttk.Separator(panel, orient="horizontal").pack(fill="x", pady=10)

        ttk.Label(panel, text="로봇 방향").pack(anchor="w")
        self.heading_combo = ttk.Combobox(
            panel,
            textvariable=self.heading,
            values=["N", "E", "S", "W"],
            state="readonly",
            width=8,
        )
        self.heading_combo.pack(anchor="w", pady=(4, 10))
        self.heading_combo.bind("<<ComboboxSelected>>", self._on_heading_user_change)

        ttk.Button(panel, text="엔진 동기화 (init)", command=self.sync_engine).pack(fill="x", pady=2)
        ttk.Button(panel, text="한 틱 (Space)", command=self.step_tick).pack(fill="x", pady=2)
        ttk.Button(panel, text="틱 초기화 (프로세스 재시작)", command=self.hard_reset_engine).pack(fill="x", pady=2)

        ttk.Separator(panel, orient="horizontal").pack(fill="x", pady=10)
        ttk.Checkbutton(panel, text="자동 연속 실행", variable=self.auto_run, command=self._toggle_auto).pack(anchor="w")
        ttk.Label(panel, text="간격 (ms)").pack(anchor="w", pady=(6, 0))
        ttk.Spinbox(panel, from_=80, to=1200, increment=20, textvariable=self.auto_delay, width=10).pack(anchor="w")

        ttk.Separator(panel, orient="horizontal").pack(fill="x", pady=10)
        ttk.Button(panel, text="맵 크기…", command=self.resize_dialog).pack(fill="x", pady=2)
        ttk.Button(panel, text="시나리오 JSON 불러오기", command=self.load_json_dialog).pack(fill="x", pady=2)

        self.status = tk.StringVar(value="준비됨")
        ttk.Label(panel, textvariable=self.status, wraplength=240, justify="left").pack(anchor="w", pady=(14, 0))

        self.sensor_vars = {
            "front": tk.StringVar(value="—"),
            "left": tk.StringVar(value="—"),
            "right": tk.StringVar(value="—"),
            "dust": tk.StringVar(value="—"),
        }
        sf = ttk.LabelFrame(panel, text=" 센서 ", padding=8)
        sf.pack(fill="x", pady=(12, 0))
        for key, label in [("front", "전방"), ("left", "좌측"), ("right", "우측"), ("dust", "먼지")]:
            row = ttk.Frame(sf)
            row.pack(fill="x", pady=2)
            ttk.Label(row, text=label, width=6).pack(side="left")
            ttk.Label(row, textvariable=self.sensor_vars[key], width=8).pack(side="left")

        self.root.bind("<space>", lambda e: self.step_tick())
        self._auto_after_id: Optional[str] = None

    def _set_heading_from_engine(self, h: str) -> None:
        self._heading_programmatic = True
        self.heading.set(h)
        self._heading_programmatic = False

    def _on_heading_user_change(self, _evt=None) -> None:
        if self._heading_programmatic:
            return
        self.engine_dirty = True

    def _toggle_auto(self) -> None:
        if self.auto_run.get():
            self._schedule_auto()
        else:
            if self._auto_after_id:
                self.root.after_cancel(self._auto_after_id)
                self._auto_after_id = None

    def _schedule_auto(self) -> None:
        if not self.auto_run.get():
            return
        self.step_tick()
        self._auto_after_id = self.root.after(self.auto_delay.get(), self._schedule_auto)

    def hard_reset_engine(self) -> None:
        try:
            self.daemon.close()
        except Exception:
            pass
        self.daemon = TickDaemon(self.tickd_path)
        self.engine_dirty = True
        self.last_state = None
        self.status.set("엔진 프로세스를 재시작했습니다. [엔진 동기화] 또는 [한 틱]을 누르세요.")

    def resize_dialog(self) -> None:
        d = tk.Toplevel(self.root)
        d.title("맵 크기")
        d.configure(bg=COL_PANEL)
        rr = tk.StringVar(value=str(self.rows))
        cc = tk.StringVar(value=str(self.cols))

        ttk.Label(d, text="행").grid(row=0, column=0, padx=8, pady=8)
        ttk.Entry(d, textvariable=rr, width=6).grid(row=0, column=1)
        ttk.Label(d, text="열").grid(row=1, column=0, padx=8, pady=8)
        ttk.Entry(d, textvariable=cc, width=6).grid(row=1, column=1)

        def apply() -> None:
            try:
                nr = int(rr.get())
                nc = int(cc.get())
            except ValueError:
                messagebox.showerror("오류", "숫자를 입력하세요.")
                return
            if nr < 3 or nc < 3 or nr > 40 or nc > 40:
                messagebox.showerror("오류", "3~40 범위로 지정하세요.")
                return
            self._resize_map(nr, nc)
            d.destroy()

        ttk.Button(d, text="적용", command=apply).grid(row=2, column=0, columnspan=2, pady=10)

    def _resize_map(self, nr: int, nc: int) -> None:
        self.rows, self.cols = nr, nc
        self.grid = [["." for _ in range(nc)] for _ in range(nr)]
        self.dust = set()
        self.robot = (nr // 2, nc // 2)
        self.engine_dirty = True
        self.last_state = None
        self.canvas.config(width=nc * self.cell_px + 2 * self.pad, height=nr * self.cell_px + 2 * self.pad)
        self.redraw_all()
        self.status.set(f"맵 {nr}×{nc} 로 초기화했습니다.")

    def load_json_dialog(self) -> None:
        path = filedialog.askopenfilename(filetypes=[("JSON", "*.json"), ("All", "*.*")])
        if not path:
            return
        try:
            data = json.loads(Path(path).read_text(encoding="utf-8"))
            rows = [str(r) for r in data["grid"]]
            self._resize_map(len(rows), len(rows[0]))
            self.grid = [list(row) for row in rows]
            self.dust = set((int(p[0]), int(p[1])) for p in data.get("dust", []))
            st = data["start"]
            self.robot = (int(st["row"]), int(st["col"]))
            self.heading.set(str(st["heading"]))
            self.engine_dirty = True
            self.redraw_all()
            self.status.set(f"불러옴: {path}")
        except Exception as exc:
            messagebox.showerror("오류", str(exc))

    def cell_from_event(self, event: tk.Event) -> Optional[Tuple[int, int]]:
        x = event.x - self.pad
        y = event.y - self.pad
        if x < 0 or y < 0:
            return None
        c = x // self.cell_px
        r = y // self.cell_px
        if 0 <= r < self.rows and 0 <= c < self.cols:
            return r, c
        return None

    def on_canvas_click(self, event: tk.Event) -> None:
        cell = self.cell_from_event(event)
        if cell is None:
            return
        r, c = cell
        tool = self.tool.get()

        if tool == "robot":
            ch = self.grid[r][c]
            if ch == "#":
                self.status.set("장애물 위에는 로봇을 둘 수 없습니다.")
                return
            self.robot = (r, c)
            self.engine_dirty = True
        elif tool == "wall":
            if (r, c) == self.robot:
                self.status.set("로봇이 있는 칸은 장애물로 바꿀 수 없습니다.")
                return
            self.grid[r][c] = "#"
            self.dust.discard((r, c))
            self.engine_dirty = True
        elif tool == "floor":
            self.grid[r][c] = "."
            self.engine_dirty = True
        elif tool == "dust":
            if self.grid[r][c] == "#":
                return
            self.dust.add((r, c))
            self.engine_dirty = True
        elif tool == "erase_dust":
            self.dust.discard((r, c))

        self.redraw_all()

    def grid_rows_as_strings(self) -> List[str]:
        return ["".join(row) for row in self.grid]

    def build_init_payload(self) -> Dict:
        dust_list = sorted(self.dust)
        return {
            "op": "init",
            "grid": self.grid_rows_as_strings(),
            "dust": [[r, c] for r, c in dust_list],
            "start": {"row": self.robot[0], "col": self.robot[1], "heading": self.heading.get()},
        }

    def sync_engine(self) -> None:
        try:
            rep = self.daemon.request(self.build_init_payload())
            if not rep.get("ok"):
                raise RuntimeError(rep.get("error", "unknown"))
            self.last_state = rep
            self.engine_dirty = False
            self.robot = (int(rep["pose"]["row"]), int(rep["pose"]["col"]))
            self._set_heading_from_engine(str(rep["pose"]["heading"]))
            self._apply_state_to_hud(rep)
            self.status.set("엔진과 동기화 완료 (init).")
            self.redraw_all()
        except Exception as exc:
            messagebox.showerror("tickd 오류", str(exc))
            self.status.set(str(exc))

    def step_tick(self) -> None:
        try:
            if self.engine_dirty:
                rep = self.daemon.request(self.build_init_payload())
                if not rep.get("ok"):
                    raise RuntimeError(rep.get("error", "unknown"))
                self.engine_dirty = False
                self.last_state = rep
                self._apply_state_to_hud(rep)
                # First physical motion tick in the same gesture (init does not advance time).
                rep2 = self.daemon.request({"op": "tick"})
                if not rep2.get("ok"):
                    raise RuntimeError(rep2.get("error", "unknown"))
                self.last_state = rep2
                rr = int(rep2["pose"]["row"])
                cc = int(rep2["pose"]["col"])
                self.robot = (rr, cc)
                self._set_heading_from_engine(str(rep2["pose"]["heading"]))
                self._apply_state_to_hud(rep2)
                self.status.set(
                    f"틱 {rep2['tick']}  |  {rep2['drive']} / {rep2['cleaner']}  |  청소 칸 {rep2['cleaned']}"
                )
                self.redraw_all()
                return

            rep = self.daemon.request({"op": "tick"})
            if not rep.get("ok"):
                raise RuntimeError(rep.get("error", "unknown"))
            self.last_state = rep
            rr = int(rep["pose"]["row"])
            cc = int(rep["pose"]["col"])
            self.robot = (rr, cc)
            self._set_heading_from_engine(str(rep["pose"]["heading"]))
            self._apply_state_to_hud(rep)
            self.status.set(
                f"틱 {rep['tick']}  |  {rep['drive']} / {rep['cleaner']}  |  청소 칸 {rep['cleaned']}"
            )
            self.redraw_all()
        except Exception as exc:
            messagebox.showerror("tickd 오류", str(exc))
            self.status.set(str(exc))

    def _apply_state_to_hud(self, rep: Dict) -> None:
        s = rep.get("sensors", {})
        self.sensor_vars["front"].set("막힘" if s.get("obstacle_front") else "통과")
        self.sensor_vars["left"].set("막힘" if s.get("obstacle_left") else "통과")
        self.sensor_vars["right"].set("막힘" if s.get("obstacle_right") else "통과")
        self.sensor_vars["dust"].set("감지" if s.get("dust_detected") else "없음")

    def cleaned_set_from_state(self) -> Set[Tuple[int, int]]:
        if not self.last_state:
            return set()
        out: Set[Tuple[int, int]] = set()
        for pair in self.last_state.get("cleaned_cells", []):
            out.add((int(pair[0]), int(pair[1])))
        return out

    def redraw_all(self) -> None:
        self.canvas.delete("all")
        cleaned = self.cleaned_set_from_state()
        rr, cc = self.robot

        for r in range(self.rows):
            for c in range(self.cols):
                x0 = self.pad + c * self.cell_px
                y0 = self.pad + r * self.cell_px
                x1 = x0 + self.cell_px - 2
                y1 = y0 + self.cell_px - 2
                ch = self.grid[r][c]

                if ch == "#":
                    self.canvas.create_rectangle(
                        x0, y0, x1, y1, fill=COL_WALL, outline=COL_WALL_EDGE, width=2
                    )
                else:
                    base = COL_CELL_HI if (r + c) % 2 else COL_CELL
                    self.canvas.create_rectangle(x0, y0, x1, y1, fill=base, outline=COL_GRID_LINE, width=1)

                if ch != "#" and (r, c) in self.dust:
                    mx = (x0 + x1) / 2
                    my = (y0 + y1) / 2
                    self.canvas.create_oval(mx - 9, my - 9, mx + 9, my + 9, fill=COL_DUST, outline=COL_DUST_RING, width=2)

                if ch != "#" and (r, c) in cleaned:
                    self.canvas.create_rectangle(
                        x0 + 4, y0 + 4, x1 - 4, y1 - 4, fill="", outline=COL_CLEAN_GLOW, width=3
                    )

        # Robot
        rx0 = self.pad + cc * self.cell_px + 8
        ry0 = self.pad + rr * self.cell_px + 8
        rx1 = rx0 + self.cell_px - 18
        ry1 = ry0 + self.cell_px - 18
        self.canvas.create_oval(rx0, ry0, rx1, ry1, fill=COL_ROBOT, outline=COL_ROBOT_STROKE, width=3)

        # Heading wedge
        cx = (rx0 + rx1) / 2
        cy = (ry0 + ry1) / 2
        h = self.heading.get()
        span = (self.cell_px - 18) * 0.35
        if h == "N":
            pts = [cx, cy - span, cx - span * 0.55, cy + span * 0.25, cx + span * 0.55, cy + span * 0.25]
        elif h == "S":
            pts = [cx, cy + span, cx - span * 0.55, cy - span * 0.25, cx + span * 0.55, cy - span * 0.25]
        elif h == "W":
            pts = [cx - span, cy, cx + span * 0.25, cy - span * 0.55, cx + span * 0.25, cy + span * 0.55]
        else:  # E
            pts = [cx + span, cy, cx - span * 0.25, cy - span * 0.55, cx - span * 0.25, cy + span * 0.55]
        self.canvas.create_polygon(*pts, fill=COL_ARROW, outline=COL_ARROW, width=1)

    def on_close(self) -> None:
        try:
            self.daemon.close()
        except Exception:
            pass
        self.root.destroy()


def run_headless_smoke() -> None:
    import tkinter as _tk  # noqa: F401


def run_replay(sim: Path, scenario: Path, delay_ms: int) -> int:
    proc = subprocess.run(
        [str(sim), "--jsonl", str(scenario)],
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
    summary: Optional[dict] = None
    for line in lines:
        obj = json.loads(line)
        if "drive" in obj:
            frames.append(obj)
        else:
            summary = obj
    if summary is None:
        raise RuntimeError("Missing summary line from rvc_sim")

    spec = json.loads(scenario.read_text(encoding="utf-8"))
    grid_rows = spec["grid"]

    root = tk.Tk()
    root.title("RVC — trace replay")
    root.configure(bg=COL_BG)
    style = ttk.Style()
    style.theme_use("vista" if sys.platform == "win32" else "clam")
    style.configure("Sub.TLabel", background=COL_BG, foreground=COL_MUTED, font=("Segoe UI", 10))

    cell_px = 44
    rows = len(grid_rows)
    cols = max(len(row) for row in grid_rows) if rows else 0
    canvas = tk.Canvas(root, width=cols * cell_px + 40, height=rows * cell_px + 40, bg=COL_BG, highlightthickness=0)
    canvas.pack()
    status = tk.StringVar()
    ttk.Label(root, textvariable=status, style="Sub.TLabel").pack(fill="x", padx=8, pady=6)

    def draw_frame(frame: Optional[dict]) -> None:
        canvas.delete("all")
        for r, row in enumerate(grid_rows):
            for c, ch in enumerate(row):
                x0 = 20 + c * cell_px
                y0 = 20 + r * cell_px
                x1 = x0 + cell_px - 2
                y1 = y0 + cell_px - 2
                fill = COL_WALL if ch == "#" else COL_CELL
                canvas.create_rectangle(x0, y0, x1, y1, fill=fill, outline=COL_GRID_LINE)
        if frame is not None:
            rr = int(frame["row"])
            cc = int(frame["col"])
            x0 = 20 + cc * cell_px + 6
            y0 = 20 + rr * cell_px + 6
            canvas.create_oval(x0, y0, x0 + cell_px - 14, y0 + cell_px - 14, fill=COL_ROBOT, outline=COL_ROBOT_STROKE, width=2)
            status.set(f"{frame.get('drive')} / {frame.get('cleaner')}  |  {summary}")
        else:
            status.set(f"완료  |  {summary}")

    idx = 0

    def tick() -> None:
        nonlocal idx
        if idx < len(frames):
            draw_frame(frames[idx])
            idx += 1
            root.after(delay_ms, tick)
        else:
            draw_frame(None)

    tick()
    root.mainloop()
    return 0


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="RVC interactive grid studio (or trace replay).")
    p.add_argument("--tickd", type=Path, help="Path to rvc_tickd executable (default: search under build/)")
    p.add_argument("--replay", action="store_true", help="Replay rvc_sim --jsonl trace instead of interactive mode")
    p.add_argument("--sim", type=Path, help="rvc_sim path (replay mode)")
    p.add_argument("--scenario", type=Path, help="Scenario JSON (replay mode)")
    p.add_argument("--delay-ms", type=int, default=120, help="Replay frame delay")
    return p.parse_args()


def main() -> int:
    if os.environ.get("RVC_HEADLESS") == "1":
        try:
            run_headless_smoke()
        except ImportError:
            print("tkinter not installed; skipping GUI import check.", file=sys.stderr)
        return 0

    args = parse_args()
    if args.replay:
        if not args.sim or not args.scenario:
            print("Replay mode requires --sim and --scenario", file=sys.stderr)
            return 2
        return run_replay(args.sim, args.scenario, args.delay_ms)

    tickd = find_tickd(args.tickd)
    if tickd is None:
        print(
            "rvc_tickd 를 찾을 수 없습니다. CMake로 빌드한 뒤 --tickd 경로를 지정하세요.\n"
            "예: cmake --build build --target rvc_tickd",
            file=sys.stderr,
        )
        return 2

    root = tk.Tk()
    app = InteractiveApp(root, tickd)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
