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
