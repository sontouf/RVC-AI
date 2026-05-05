# 시스템 테스트 전략 (V&V) — RVC SW Controller

근거: `arch/requirements/fr-nfr.md` NFR-SYS-001, NFR-TEST-001.

## 목적

- 자동 청소 **행동**을 그리드 시뮬레이터에서 검증한다 (GTest 비사용).
- **≥ 30** 시나리오, **positive / negative** 혼합, **표시 상태(DisplayState)** 전역 커버리지.

## 산출물

| 경로 | 설명 |
|------|------|
| `scripts/generate_system_maps.py` | 맵 JSON 생성 (`system_tests/maps/`) |
| `system_tests/run_all.py` | `rvc_sim` 일괄 실행, 시나리오별 `required_states`, **전체 합집합 대비 금지/필수 상태** |
| `tools/rvc_sim` | JSON 시나리오 로더, `--jsonl` 트레이스, `required_states` / `forbidden_states` |
| `sim/rvc_grid_gui.py` | tkinter GUI — 맵·위치·먼지·상태·체크리스트 (인간 **가시 검증**) |

## Positive vs Negative

- **Positive**: `required_states` 가 **해당 실행 트레이스**에 모두 등장.
- **Negative (금지 상태)**: `forbidden_states` 가 트레이스에 **등장하면 실패** (예: 개활지에서 `Maneuver_Reverse` 금지).

## 커버리지 기준

- `run_all.py` 가 수집한 표시 상태 합집합이 다음을 **모두 포함**해야 한다:  
  `Idle`, `Cleaning_Forward`, `Cleaning_Forward_Boost`, `Maneuver_Stop`, `Maneuver_Turn`, `Maneuver_Reverse`, `Session_Stopping`.

## CI vs 로컬 GUI

- CI: 헤드리스로 `run_all.py` 만 실행 (NFR 파이프라인 게이트).
- 로컬: GUI로 시각적 회귀 및 시연 (fr-nfr 의 GUI 가시 확인에 부합).

## NFR-PERF-001 (범위)

- 현재 그리드 시뮬은 **이산 제어 틱**만 정의한다. 실측 주기(ms)·샘플링 목표는 타겟 HW/ `cpp-conventions`에서 정의 후 하드타이머 통합 시 검증한다.

## 추적

- FR-001~005 ↔ 시나리오 집합 (회피·탈출·먼지·세션·전진).
- 구현 ↔ SSD: `arch/design/implementation-mapping.md`.
