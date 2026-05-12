# 건국대 객체지향개발방법론(이론) ↔ 본 프로젝트 산출물 매핑

이 저장소는 건국대 `OOAD – 객체지향개발방법론` 교재의 흐름(UML·UP 중심)을 따르되, 실제 구현은 **C++/GoogleTest/TDD**와 **GitHub Actions**로 고정합니다.

## 교재 목차와 프로젝트 산출물

| 교재 주제(요지) | 프로젝트 산출물 |
|-----------------|----------------|
| 객체지향 소프트웨어 개발 개요 | `README.md`, `docs/preliminary-requirements.md` |
| UML 개요 | 각 설계 문서의 **Mermaid UML** 다이어그램 |
| Unified Process(UP) 단계 | Rules/Skill에 정의된 **Inception→Elaboration 성격의 단계 게이트** |
| 유스케이스 | `arch/use-cases/use-cases.md` |
| 시스템 시퀀스 다이어그램(SSD) | `arch/analysis/system-sequence-diagrams.md` |
| 도메인 모델 | `arch/analysis/domain-model.md` |
| 시퀀스 다이어그램(SD) | `arch/design/sequence-diagrams.md` |
| 클래스 다이어그램 | `arch/design/class-diagram.md` |
| 상태 다이어그램(선택) | 필요 시 `arch/design/` 하위에 추가 (본 릴리스는 행위를 Coordinator와 명시적 큐로 단순화) |
| 설계→코드 매핑(OOI) | `arch/design/implementation-mapping.md`, `src/`, `include/` |

## RVC 과제 브리프의 UP 단계와의 정렬

`RVC FR.pdf`의 “Overall Steps”는 다음과 같이 본 저장소 구조에 대응합니다.

| 브리프 단계 | 저장소에서의 위치 |
|-------------|-------------------|
| Inception: 유스케이스 + CI/CD | `arch/use-cases/`, `.github/workflows/`, `.cursor/` |
| OOA: 유스케이스 정제 + SSD + 도메인 모델 | `arch/analysis/` |
| OOD: 시퀀스·클래스·SOLID | `arch/design/` |
| OOI: UT/ST, 시뮬레이터 | `tests/`, `system_tests/`, `sim/` |

## 교재 내용을 “그대로 복붙”하지 않는 이유

교재 예제(도서관 시스템 등)의 도메인은 RVC와 다릅니다. 따라서 **방법론과 표기법만을 계승**하고, 도메인 개체·계약은 RVC 센서/청소 행위로 재작성합니다.
