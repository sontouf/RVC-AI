---
name: rvc-ooad-vnv
description: >-
  RVC controller 저장소에서 OOAD(요구→유스케이스→SSD→도메인→SD→클래스→SOLID) 산출물과
  TDD(GoogleTest)·시스템 테스트·GitHub Actions까지 단계별로 진행할 때 사용한다.
---

# RVC OOAD & V&V 스킬

## 시작 전

1. 사용자가 제공한 최신 요구 브리프를 확인한다.
2. 다음 파일을 순서대로 연다(있으면 읽고 없으면 작성한다):
   - `docs/preliminary-requirements.md`
   - `arch/requirements/system-requirements.md`
   - `arch/use-cases/use-cases.md`

## 단계별 DoD

### Inception

- 기능/비기능 요구가 표 형태로 추적 가능해야 한다.
- 유스케이스가 각 FR과 연결된다.

### OOA

- SSD가 최소 1개 시나리오에 대해 블랙박스 입출력을 보여준다.
- 도메인 모델이 구현 클래스 이름과 과도하게 닮지 않도록 **개념 중심**으로 유지한다.

### OOD

- 시퀀스 다이어그램이 `CleaningCoordinator` ↔ `GridWorld` 책임 분할과 모순 없어야 한다.
- 클래스 다이어그램과 실제 `include/` 트리가 크게 어긋나면 `implementation-mapping.md`를 갱신한다.

### OOI / TDD

- Red→Green→Refactor를 명시적으로 따른다.
- 테스트 이름은 요구 ID 또는 유스케이스 ID를 포함한다.

### System test & Simulator

- `system_tests/run_all.py`가 로컬에서 통과해야 한다.
- GUI 스모크는 `docs/simulator-runbook.md`를 따른다.

### CI/CD

- 커버리지 게이트를 깨지 않는다.

## 멀티에이전트

- 문서 품질 다듬기와 CMake 정리처럼 **의존 낮은 작업**만 병렬화한다.
