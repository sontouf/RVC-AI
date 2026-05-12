# Implementation Mapping (OOA/OOD → 코드)

| 산출물 요소 | 코드 위치 |
|-------------|-----------|
| UC-01~04 행위 통합 | `CleaningCoordinator::next_tick` |
| 도메인 개념 Pose/Heading | `include/rvc/types.hpp` |
| SurfaceCell / 공간 관측 | `GridWorld::sense`, 문자 맵 표현 |
| SSD `tick()` 계약 | `apps/rvc_sim.cpp`의 메인 루프 |
| 인터랙티브 GUI 한 틱 | `apps/rvc_tickd.cpp` (stdin JSON line 프로토콜) |
| 시스템 테스트 입력 | `system_tests/maps/*.json` |
| 단위·통합 테스트 | `tests/gtest/*.cpp` |

## 테스트 매핑

| 요구 ID | 테스트 스위트 |
|---------|----------------|
| FR-002 | `CleaningCoordinator.AvoidancePrefersLeftWhenBothSidesOpen` 등 |
| FR-003 | `CleaningCoordinator.DeadEndTriggersBackupSequence` |
| FR-004 | `CleaningCoordinator.BoostActivatesOnDust` |
| SR-SIM | `system_tests/run_all.py` + `rvc_sim` |
