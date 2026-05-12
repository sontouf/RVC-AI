# RVC Controller (OOAD / V&V)

Robot vacuum **control software** developed with **OOAD (Unified Process style)** artifacts under `arch/`, **TDD** with **GoogleTest**, and **GitHub Actions** CI/CD.

The **GUI simulator** lives under `sim/` (Python + Tkinter). **System tests** drive the `rvc_sim` executable with JSON scenarios under `system_tests/maps/`.

## Documentation map

| Topic | Path |
|-------|------|
| Cursor rules/skills/commands standards | `docs/cursor-conventions.md` |
| Plan / Agent / Multitask playbook | `docs/agent-mode-playbook.md` |
| Preliminary requirements (FR draft) | `docs/preliminary-requirements.md` |
| Course theory ↔ artifacts | `docs/ooad-course-mapping.md` |
| Simulator (PowerShell & WSL) | `docs/simulator-runbook.md` |
| CI/CD & deployment evidence | `docs/ci-and-deployment-report.md` |
| Traceability & V&V | `arch/vnv/` |

## Build (Windows / Linux)

```bash
cmake -B build -S .
cmake --build build
./build/rvc_tests          # Linux / Git Bash
build\Release\rvc_tests.exe   # VS multi-config (Windows)
```

Or run tests via CTest:

```bash
ctest --test-dir build --output-on-failure
```

## Coverage (optional local / CI)

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DRVC_ENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

CI uploads **lcov HTML** and binaries as workflow artifacts.

## System tests

```bash
cmake -B build -S .
cmake --build build --target rvc_sim
python3 system_tests/run_all.py --sim ./build/rvc_sim
```

Windows PowerShell:

```powershell
cmake --build build --config Release --target rvc_sim
python system_tests/run_all.py --sim ./build/Release/rvc_sim.exe
```

## GUI simulator

See `docs/simulator-runbook.md`.

```bash
python3 sim/rvc_grid_gui.py --help
```

Headless smoke (CI helper):

```bash
RVC_HEADLESS=1 python3 sim/rvc_grid_gui.py
```

Repository: [https://github.com/sontouf/RVC-AI](https://github.com/sontouf/RVC-AI)
