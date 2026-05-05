---
description: OOAD+V&V 전체 파이프라인—arch-with-ai 스타일로 단계별 /ooad 명령 실행 순서를 한 눈에 정리한다. 마일스톤·review-phase-checkpoints 포함.
---

# RVC 프로젝트: OOAD · 요구 · 구현 · 검증 명령 순서

아래 **순서대로** 슬래시 명령을 실행한다. 반복 항목은 산출물 개수만큼 돌린다.

## 1. 비전·요구

1. `/ooad/define-vision` → `arch/system.md`
2. `/ooad/specify-fr-nfr` → `arch/requirements/fr-nfr.md` (UC 생기면 추적 표 갱신)

## 2. OOA

3. `/ooad/extract-usecases` → `arch/usecases.md`
4. `/ooad/specify-usecase` → **UC마다** `arch/usecase/UC-nnn.md`
5. `/ooad/model-domain` → `arch/domain/model.md`
6. `/ooad/model-ssd` → **시나리오마다** `arch/ssd/…`

## 3. OOD

7. `/ooad/design-interaction` → **시나리오·연산마다** `arch/design/interaction/…`
8. `/ooad/design-classes` → `arch/design/class-diagram.md` (7번과 **반복 보완**)
9. `/ooad/design-packages` (선택) → `arch/design/packages.md`

## 4. 구현

10. `/ooad/map-to-code` → `src/` (또는 `agentk.sourceDirectory`)

## 5. 자동 테스트 (GTest)

11. `/ooad/develop-unit-tests-gtest` → 단위·통합, stub/driver, 커버리지

## 6. 시스템 테스트·CI·정적 분석

12. `/ooad/plan-system-tests-gui` → `arch/vnv/system-tests.md` (≥30, pos/neg, 비-GTest)
13. `/ooad/configure-cicd-github-actions` → `build → unit → integration → system` 순
14. 시스템 테스트 통과 후 `/ooad/archive-static-analysis-report` → `reports/static-analysis/`

## 품질 점검 (원할 때마다)

- `/ooad/review-phase-checkpoints` — 각 단계(OOA 끝, OOD 끝, 구현 전 등)에서 체크리스트 검토

---

**지시:** 위 순서를 사용자에게 요약 표시한 뒤, **현재 어느 단계인지** 묻고 그 단계에 맞는 명령으로 산출물을 작성·갱신한다. 선행 파일이 없으면 앞 단계부터 제안한다.
