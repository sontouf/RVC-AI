#!/usr/bin/env python3
"""Generate system test map JSON files under system_tests/maps/."""
from __future__ import annotations

import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAPS = os.path.join(ROOT, "system_tests", "maps")


def write(path: str, obj: dict) -> None:
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(obj, f, indent=2)
        f.write("\n")


def base(name: str, **extra) -> dict:
    d = {
        "name": name,
        "width": 6,
        "height": 6,
        "obstacles": [],
        "dust": [],
        "start": {"row": 2, "col": 1, "heading": "E"},
        "max_ticks": 120,
        "auto_start": True,
    }
    d.update(extra)
    return d


def main() -> None:
    os.makedirs(MAPS, exist_ok=True)

    write(
        os.path.join(MAPS, "s_001_idle_only.json"),
        base(
            "s_001_idle_only",
            auto_start=False,
            max_ticks=4,
            required_states=["Idle"],
        ),
    )

    write(
        os.path.join(MAPS, "s_002_forward_open.json"),
        base(
            "s_002_forward_open",
            obstacles=[],
            required_states=["Cleaning_Forward"],
            max_ticks=8,
        ),
    )

    write(
        os.path.join(MAPS, "s_003_dust_boost.json"),
        base(
            "s_003_dust_boost",
            dust=[[2, 2, 2]],
            start={"row": 2, "col": 2, "heading": "E"},
            required_states=["Cleaning_Forward_Boost"],
            max_ticks=15,
        ),
    )

    write(
        os.path.join(MAPS, "s_004_partial_avoid.json"),
        base(
            "s_004_partial_avoid",
            obstacles=[[2, 3]],
            start={"row": 2, "col": 2, "heading": "E"},
            required_states=["Maneuver_Stop", "Maneuver_Turn"],
            max_ticks=40,
        ),
    )

    write(
        os.path.join(MAPS, "s_005_enclosure_escape.json"),
        base(
            "s_005_enclosure_escape",
            obstacles=[[2, 3], [2, 1], [1, 2], [3, 2]],
            start={"row": 2, "col": 2, "heading": "E"},
            required_states=["Maneuver_Reverse", "Maneuver_Turn"],
            max_ticks=80,
        ),
    )

    write(
        os.path.join(MAPS, "s_006_session_stopping.json"),
        base(
            "s_006_session_stopping",
            inject=[{"at_tick": 4, "command": "Stop"}],
            required_states=["Session_Stopping", "Idle"],
            max_ticks=20,
        ),
    )

    write(
        os.path.join(MAPS, "s_007_master_walkthrough.json"),
        {
            "name": "s_007_master_walkthrough",
            "width": 10,
            "height": 5,
            "obstacles": [
                [2, 6],
                [1, 8],
                [3, 8],
                [2, 9],
            ],
            "dust": [[2, 3, 2]],
            "start": {"row": 2, "col": 1, "heading": "E"},
            "max_ticks": 220,
            "auto_start": False,
            "inject": [
                {"at_tick": 2, "command": "Start"},
                {"at_tick": 180, "command": "Stop"},
            ],
            "required_states": [],
        },
    )

    write(
        os.path.join(MAPS, "s_neg_041_open_no_reverse.json"),
        base(
            "s_neg_041_open_no_reverse",
            width=5,
            height=5,
            obstacles=[],
            dust=[],
            start={"row": 2, "col": 1, "heading": "E"},
            max_ticks=25,
            required_states=["Cleaning_Forward"],
            forbidden_states=["Maneuver_Reverse"],
        ),
    )

    # Wide grid + checker pillars + dust: many avoid turns and at least one boost window.
    w_st, h_st = 16, 11
    obs_st: list[list[int]] = []
    for c in range(w_st):
        obs_st.append([0, c])
        obs_st.append([h_st - 1, c])
    for r in range(h_st):
        obs_st.append([r, 0])
        obs_st.append([r, w_st - 1])
    for r in range(2, h_st - 2):
        for c in range(3, w_st - 2):
            if (r + c) % 2 == 0:
                obs_st.append([r, c])
    mid_r = h_st // 2
    # Dust on starting cell (sensor reads current cell) plus deeper cells for sustained tour.
    dust_st = [[mid_r, 1, 2], [mid_r, 8, 2], [mid_r - 2, 11, 1]]
    write(
        os.path.join(MAPS, "s_042_complex_stress.json"),
        {
            "name": "s_042_complex_stress",
            "width": w_st,
            "height": h_st,
            "obstacles": obs_st,
            "dust": dust_st,
            "start": {"row": mid_r, "col": 1, "heading": "E"},
            "max_ticks": 650,
            "auto_start": True,
            "required_states": ["Cleaning_Forward", "Maneuver_Turn", "Cleaning_Forward_Boost"],
        },
    )

    # Zigzag walls forcing repeated avoids; dust on-path for boost debounce.
    write(
        os.path.join(MAPS, "s_043_maze_escape_mix.json"),
        {
            "name": "s_043_maze_escape_mix",
            "width": 14,
            "height": 10,
            "obstacles": [
                [5, 2],
                [5, 3],
                [5, 4],
                [4, 4],
                [3, 4],
                [3, 3],
                [3, 2],
                [6, 6],
                [5, 6],
                [4, 6],
                [4, 7],
                [4, 8],
                [5, 8],
                [6, 8],
                [6, 7],
                [2, 7],
                [2, 8],
                [2, 9],
                [7, 4],
                [8, 4],
                [8, 5],
                [8, 6],
                [7, 6],
                [1, 5],
                [1, 6],
            ],
            "dust": [[5, 1, 2], [6, 9, 1]],
            "start": {"row": 5, "col": 1, "heading": "E"},
            "max_ticks": 450,
            "auto_start": True,
            "required_states": ["Maneuver_Turn", "Maneuver_Stop", "Cleaning_Forward"],
        },
    )

    # Variations s_008..s_040 (size/placement fuzz, still deterministic)
    for i in range(8, 41):
        w = 5 + (i % 5)
        h = 5 + ((i * 3) % 5)
        ox = (i * 7) % (w - 2) + 1
        oy = (i * 11) % (h - 2) + 1
        obs = [[oy, ox]]
        if i % 3 == 0:
            obs.append([(oy - 1) % h, (ox + 1) % w])
        dust = []
        if i % 2 == 0:
            dust.append([(oy + 1) % h, max(0, (ox - 1) % w), 1])
        write(
            os.path.join(MAPS, f"s_{i:03d}_variant.json"),
            {
                "name": f"s_{i:03d}_variant",
                "width": w,
                "height": h,
                "obstacles": obs,
                "dust": dust,
                "start": {"row": h // 2, "col": 1, "heading": "E"},
                "max_ticks": 100,
                "auto_start": True,
                "required_states": ["Cleaning_Forward"],
            },
        )

    print(f"Wrote maps to {MAPS}")


if __name__ == "__main__":
    main()
