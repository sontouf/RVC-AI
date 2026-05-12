# GUI 시뮬레이터 실행 가이드 (`sim/rvc_grid_gui.py`)

이 GUI는 **제어 로직을 재구현하지 않습니다.** 대신 `rvc_sim --jsonl`이 출력한 트레이스를 읽어 격자 위에 로봇 위치를 재생합니다.

## 전제 조건

- Python **3.8+**
- 표준 라이브러리 위주이며, 그래픽은 **`tkinter`** 를 사용합니다.
  - 일부 리눅스 배포판은 `python3-tk` 패키지 설치가 필요합니다.

## PowerShell (Windows)

```powershell
cd "c:\Ref\DSLAB\Harness Engineering\dsdd"
cmake -B build -S .
cmake --build build --config Release --target rvc_sim
python .\sim\rvc_grid_gui.py --sim .\build\Release\rvc_sim.exe --scenario .\system_tests\maps\open_room.json
```

단일 구성 생성기(`build\rvc_sim.exe`)를 쓰는 경우 `--sim` 경로만 조정하면 됩니다.

## WSL / Linux / macOS

```bash
cd "/mnt/c/Ref/DSLAB/Harness Engineering/dsdd"  # 예시 경로
cmake -B build -S .
cmake --build build --target rvc_sim
python3 sim/rvc_grid_gui.py --sim ./build/rvc_sim --scenario ./system_tests/maps/open_room.json
```

## 헤드리스 스모크 (CI 보조)

Tk 창 없이 import 가능 여부만 확인합니다.

```bash
RVC_HEADLESS=1 python3 sim/rvc_grid_gui.py
```

PowerShell:

```powershell
$env:RVC_HEADLESS = "1"
python .\sim\rvc_grid_gui.py
```

> 헤드리스 모드에서는 `--sim` / `--scenario` 가 필요하지 않습니다.

## 시스템 테스트와의 관계

자동 게이트는 `system_tests/run_all.py` 가 담당합니다. GUI는 사람이 **회피·후진·부스트**가 지도 상에서 합리적으로 보이는지 확인할 때 사용합니다.
