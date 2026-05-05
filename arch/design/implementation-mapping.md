# 구현 ↔ SSD/DCD 추적 (`implementation-mapping.md`)

본 문서는 블랙박스 SSD의 **시스템 연산 이름**(Pascal/camelCase)과 **C++ 구현 식별자**를 연결한다. 동작 의미가 동일하면 추적성이 유지된다.

## 시스템 연산 (SSD) → C++

| SSD / DCD (개념) | C++ 구현 |
|------------------|----------|
| `startAutoCleaning()` | `CleaningCoordinator::on_user_command(UserCommand::Start)` |
| `stopAutoCleaning()` | `CleaningCoordinator::on_user_command(UserCommand::Stop)` |
| `reportPerception(...)` (동기 루프) | `tick()` 내부에서 `ISensorPort::read()` 또는 직전에 설정된 `on_perception(…)` 1회 소비 |
| 먼지 이벤트 (동기 병합) | `on_dust_detected(int level)` — 해당 `tick()` 의 병합 스냅샷에 반영 |
| `commandCleaningPower(level, …)` | `IActuatorPort::set_cleaning_power(CleaningPowerLevel)` + 이동 시 `forward_clean` 과 조합 |
| `stopCleaningMotion` | `IActuatorPort::stop_motion()` |
| `commandTurnAside` | `IActuatorPort::turn_aside(AvoidSide)` |
| `commandForwardClean` | `IActuatorPort::forward_clean(CleaningPowerLevel)` |
| `commandReverse` | `IActuatorPort::reverse_cells(int)` |

## DCD 인터페이스 이름

- 문서 Mermaid의 `SensorPort` / `ActuatorPort` → 구현체 **`ISensorPort` / `IActuatorPort`** (`include/rvc/ports.hpp`).
- Mermaid의 `PerceptionSnapshot` 필드명(camelCase) → 구현 **`front_blocked`** 등 snake_case (`include/rvc/types.hpp`).

## 제어 루프 모델

- **표준**: 임베디드형 **동기 `tick()`** — 한 틱에서 인식→정책→명령 순서가 SSD 한 사이클에 대응.
- **푸시 드라이버**: `on_perception` / `on_dust_detected` 로 이벤트성 입력을 같은 `tick()` 계약 안으로 흡수.

## UC-005 튜닝 (`ControllerConfig`)

| UC / NFR | 구성 필드 |
|----------|-----------|
| Typical: 유지 시간 T | `boost_duration_ticks` |
| E1: 노이즈/디바운스 | `dust_debounce_consecutive_ticks`, `dust_level_threshold` |
| E2: 레벨 상한 | `max_boost_level` → `CleaningPowerPolicy::schedule_boost` 클램프 |

시나리오 JSON의 `"controller"` 블록으로 `rvc_sim` 에서 주입 가능.

## 시뮬레이터

- 그리드: `GridSensor` / `GridActuator` 가 포트를 구현 — `arch/system.md` 의 HW 블랙박스 가정과 일치.
