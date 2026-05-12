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
./build/rvc_unit_tests
./build/rvc_integration_tests
```

Windows (multi-config 예시):

```powershell
build\Release\rvc_unit_tests.exe
build\Release\rvc_integration_tests.exe
```

CTest는 **라벨**로 단계를 나눕니다 (CI와 동일):

```bash
ctest --test-dir build -L unit --output-on-failure
ctest --test-dir build -L integration --output-on-failure
```

전체:

```bash
ctest --test-dir build --output-on-failure
```

## Coverage (optional local / CI)

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DRVC_ENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

CI는 `/.github/workflows/ci.yml` 오케스트레이터가 **build → unit → integration(coverage) → system(sim) → deploy** 순으로 실행하고, 최종 아티팩트 **`rvc-deployment-bundle`** 을 올립니다.

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
