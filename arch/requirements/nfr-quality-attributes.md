# Quality Attribute Requirements (Scenarios)

## 신뢰성·테스트 용이성

| ID | 시나리오 | 측정 |
|----|----------|------|
| NFR-QA-TEST-01 | 회피·후진·부스트 분기마다 **단위 테스트가 존재**한다. | `ctest`, 테스트 이름 규칙 `CleaningCoordinator.*` |
| NFR-QA-TEST-02 | 실제 실행 파일을 구동하는 **시스템 테스트 스위트**가 존재한다. | `system_tests/run_all.py` exit 0 |

## 유지보수성·모듈성

| ID | 시나리오 | 증거 산출물 |
|----|----------|-------------|
| NFR-QA-MAINT-01 | 청소 판단 로직과 격자 세계 물리가 **단일 거대 클래스에 혼재하지 않는다**. | `CleaningCoordinator` vs `GridWorld` |
| NFR-QA-MAINT-02 | SOLID 위반 가능 지점이 설계 문서에서 검토된다. | `arch/design/solid-analysis.md` |

## 성능(소프트웨어 관점)

| ID | 시나리오 | 목표 |
|----|----------|------|
| NFR-QA-PERF-01 | 단일 시나리오 시뮬레이션은 개발용 맵 크기에서 **1초 미만** 수준을 목표로 한다(환경 의존). | 로컬 벤치(비게이트) |

## 보안·안전(본 과제 범위 내)

| ID | 시나리오 | 목표 |
|----|----------|------|
| NFR-QA-SAFE-01 | 시뮬레이터 입력 JSON은 필수 필드 검증 실패 시 명확히 오류 종료한다. | `rvc_sim` stderr 메시지 |

## CI/CD 품질 게이트

| ID | 요구 |
|----|------|
| NFR-QA-CICD-01 | PR/푸시 시 빌드·단위/통합 테스트·커버리지·시스템 테스트가 실행된다. |
| NFR-QA-CICD-02 | 산출물(커버리지 HTML, 바이너리)이 워크플로 아티팩트로 보존된다. |
