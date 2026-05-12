---
description: RVC OOI/TDD — GoogleTest로 Red→Green→Refactor (요구 추적표 동반)
---

1. `arch/vnv/tdd-traceability.md`를 연 후 부족한 매핑을 채운다.
2. `tests/`에 **실패 테스트부터** 추가한다.
3. `include/`·`src/`를 최소 변경으로 통과시킨다.
4. 로컬에서 `ctest --test-dir build -L unit` 및 `-L integration` 이 통과해야 한다 (또는 `ctest --test-dir build --output-on-failure`).

비기능은 커버리지·정적 분석은 CI 워크플로에서 검증한다.
