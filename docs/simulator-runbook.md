# GUI 시뮬레이터 실행 가이드 (`sim/rvc_grid_gui.py`)

## 역할 구분

| 구성 요소 | 역할 |
|-----------|------|
| **`rvc_tickd`** (C++) | `GridWorld` + `CleaningCoordinator` 기준 **한 틱씩** JSON 라인 프로토콜로 응답 (제어 로직의 단일 소스). |
| **`rvc_grid_gui.py`** (Python + Tkinter) | **맵 편집**(장애물·먼지·로봇 위치·방향), **스텝 실행**, 센서 HUD. |
| **`rvc_sim`** | JSON 시나리오 **전체 자동 재생** (시스템 테스트·트레이스 리플레이). |

## 전제 조건

- Python **3.8+**
- **`tkinter`** (Linux는 `python3-tk` 등 패키지 필요할 수 있음)
- CMake로 **`rvc_tickd`** 빌드 (인터랙티브 모드)

## 1) 인터랙티브 스튜디오 (기본)

### 빌드

```powershell
cd "c:\Ref\DSLAB\Harness Engineering\dsdd"
cmake -B build -S .
cmake --build build --config Release --target rvc_tickd
```

### 실행

```powershell
python .\sim\rvc_grid_gui.py
# 또는
python .\sim\rvc_grid_gui.py --tickd .\build\Release\rvc_tickd.exe
```

WSL / Linux:

```bash
cmake --build build --target rvc_tickd
python3 sim/rvc_grid_gui.py --tickd ./build/rvc_tickd
```

### 사용법

- **도구**: 로봇 위치 / 장애물 / 바닥 / 먼지 / 먼지 지우기 — 캔버스 셀을 클릭해 편집합니다.
- **로봇 방향**: `N` `E` `S` `W` 콤보.
- **엔진 동기화 (init)**: 현재 편집 맵을 `rvc_tickd`에 보냅니다 (틱 카운터 리셋).
- **한 틱 (Space)**: 첫 실행 시 **init + 첫 틱**을 한 번에 수행한 뒤, 이후부터는 틱마다 이동·센서가 갱신됩니다.
- **자동 연속 실행**: 체크 후 간격(ms)에 따라 `한 틱`을 반복합니다.
- **맵 크기 / 시나리오 JSON 불러오기**: `system_tests/maps/*.json` 형식과 호환 (`grid`, `dust`, `start`).

## 2) 트레이스 리플레이 (기존 방식)

`rvc_sim --jsonl` 출력만 애니메이션합니다.

```powershell
python .\sim\rvc_grid_gui.py --replay --sim .\build\Release\rvc_sim.exe --scenario .\system_tests\maps\open_room.json
```

## 3) 헤드리스 스모크 (CI)

```bash
RVC_HEADLESS=1 python3 sim/rvc_grid_gui.py
```

Tk import 확인만 하며 **`--tickd` 불필요**합니다.

## `rvc_tickd` JSON 프로토콜 (참고)

stdin에 **한 줄에 JSON 하나**씩:

```json
{"op":"init","grid":["..."],"dust":[[1,2]],"start":{"row":1,"col":1,"heading":"E"}}
{"op":"tick"}
{"op":"quit"}
```

stdout에 한 줄 JSON 응답: `pose`, `sensors`, `drive`, `cleaner`, `cleaned`, `cleaned_cells` 등.

## 시스템 테스트

자동 검증은 `system_tests/run_all.py` + `rvc_sim` 이 담당합니다. GUI는 사람이 동작을 확인할 때 사용합니다.
