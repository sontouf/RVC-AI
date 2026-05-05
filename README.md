# RVC SW Controller (grid simulator)

[![CI](https://github.com/sontouf/RVC-AI/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/sontouf/RVC-AI/actions/workflows/ci.yml)

CMake C++17 core: `CleaningCoordinator`, grid `Sensor`/`Actuator` adapters, scenario runner **`rvc_sim`.  
System tests: **40+ JSON maps** under `system_tests/maps/` plus `system_tests/run_all.py` (per-scenario asserts, **forbidden_states**, **global display-state coverage**).  
GUI: `sim/rvc_grid_gui.py` (tkinter) animates traces from `rvc_sim --jsonl`.

구현 ↔ SSD/DCD: **`arch/design/implementation-mapping.md`**.

## Build

```bash
cmake -B build -S .
cmake --build build
./build/rvc_unit_tests
./build/rvc_integration_tests
python3 scripts/generate_system_maps.py
python3 system_tests/run_all.py --sim ./build/rvc_sim
```

Windows: `build\Release\rvc_sim.exe` (multi-config) or `build\rvc_sim.exe`.

### Coverage (NFR-TEST-001, optional local)

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DRVC_ENABLE_COVERAGE=ON -DRVC_BUILD_SIM=OFF
cmake --build build
./build/rvc_unit_tests && ./build/rvc_integration_tests
lcov --capture --directory build --output-file coverage.info
```

## GUI

```bash
python sim/rvc_grid_gui.py
```

Headless gate (CI helper): `RVC_HEADLESS=1 python sim/rvc_grid_gui.py` exits immediately.

## CI/CD

GitHub Actions (저장소 **sontouf/RVC-AI**): **build → unit (GTest) → integration (GTest) → coverage (lcov artifact) → system (`run_all.py`) → static analysis → deploy** on `main`/`master`.

Actions 탭에서 **Run workflow** 로 수동 실행 가능 (`workflow_dispatch`).

## First push to GitHub (`sontouf`)

원격이 이미 `RVC-AI` 로 연결된 경우: `git push -u origin main` 만으로 CI가 실행됩니다.

1. **GitHub CLI** (권장): 터미널에서 `gh auth login` 으로 로그인한 뒤:
   ```powershell
   cd "c:\Ref\DSLAB\Harness Engineering\RVC"
   gh repo create sontouf/새이름 --public --source=. --remote=origin --push
   ```
   이미 `origin`이 있으면 위 단계는 생략합니다.

2. **PAT + 스크립트**: [토큰](https://github.com/settings/tokens) 발급 후:
   ```powershell
   $env:GITHUB_TOKEN = "ghp_...."
   .\scripts\publish-to-github.ps1 -RepoName "저장소이름"
   ```

## Architecture

See `arch/` (OOAD). DCD: `arch/design/class-diagram.md`. 시스템 테스트 전략: `arch/vnv/system-tests.md`.
