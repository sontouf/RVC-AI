# TDD Traceability Matrix

개발 순서: **Red → Green → Refactor** 를 각 기능 분기마다 반복.

| 요구 | 테스트 (GoogleTest) | 구현 포인트 |
|------|---------------------|-------------|
| FR-001 | `CleaningCoordinator.ForwardWhenClear_FR001`, `GridWorld.ForwardMovesEastWhenHeadingEast` | 전진 + 청소 카운트 |
| FR-002 | `CleaningCoordinator.AvoidancePrefersLeftWhenBothSidesOpen_FR002`, `CleaningCoordinator.AvoidanceUsesRightWhenOnlyRightOpen_FR002` | 큐에 Stop/Turn/Forward |
| FR-003 | `CleaningCoordinator.DeadEndTriggersBackup_FR003`, `CleaningCoordinator.DeadEndEscapeSequence_FR003` | 후진·회전 큐 |
| FR-004 | `CleaningCoordinator.BoostActivatesOnDust_FR004`, `CleaningCoordinator.BoostDecrementsWhileForwarding_FR004` | 부스트 카운터 |
| NFR-TEST-001 | `rvc_unit_tests` + `rvc_integration_tests` (CTest `-L unit` / `-L integration`) | GTest 단계별 실행 |
| NFR-READ-001 | `TypesStrings.*` (명명 일관성 보조) | `drive_command_to_string` 등 진단 문자열 |

## 비기능(NFR) 검증

| NFR | 검증 수단 |
|-----|-----------|
| NFR-COV-001 | CI에서 `gcovr --fail-under-line 90` (타깃: `include/rvc`, `src/rvc`) |
| NFR-CI-001 | `.github/workflows/ci.yml` |
| NFR-READ-001 | 코드 리뷰 체크리스트 + `.cursor/rules/rvc-cpp-solid-readability.mdc` |
