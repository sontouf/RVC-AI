# RVC SW Controller (grid simulator)

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

GitHub Actions: **build → unit (GTest) → integration (GTest) → coverage (lcov artifact) → system (`run_all.py`) → static analysis → deploy** on `main`/`master`.

## First push to GitHub (`sontouf`)

1. **GitHub CLI** (권장): 터미널에서 `gh auth login` 으로 로그인한 뒤, 저장소 폴더에서:
   ```powershell
   cd "c:\Ref\DSLAB\Harness Engineering\RVC"
   gh repo create sontouf/rvc-sw-controller --public --source=. --remote=origin --push
   ```
   저장소 이름은 바꿔도 됩니다. 이미 `origin`이 있으면 `gh repo create ...` 대신 웹에서 빈 repo 만든 후 `git remote add origin ...` / `git push -u origin main`.

2. **PAT + 스크립트**: [Fine-grained 또는 classic token](https://github.com/settings/tokens) (`repo` 또는 Contents 쓰기) 발급 후:
   ```powershell
   $env:GITHUB_TOKEN = "ghp_...."
   .\scripts\publish-to-github.ps1 -RepoName "rvc-sw-controller"
   ```

푸시 후 **https://github.com/sontouf/&lt;repo&gt;/actions** 에서 CI 실행 여부를 확인합니다.

## Architecture

See `arch/` (OOAD). DCD: `arch/design/class-diagram.md`. 시스템 테스트 전략: `arch/vnv/system-tests.md`.
