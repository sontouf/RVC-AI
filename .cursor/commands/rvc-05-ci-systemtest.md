---
description: RVC 시스템 테스트·GUI 시뮬레이터·CI 검증
---

1. `cmake --build`로 `rvc_sim`를 생성한다.
2. `python3 system_tests/run_all.py --sim <path>` (Windows는 `py -3` 또는 `python`).
3. GUI 스모크는 `docs/simulator-runbook.md` 절차를 따른다.
4. 변경이 CI에 영향 있으면 `.github/workflows/ci.yml`를 업데이트한다.
5. 결과 요약을 `docs/ci-and-deployment-report.md`에 반영한다.
