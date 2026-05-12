# Cursor 표준 작성 방식 (Rules / Skills / Commands)

이 프로젝트는 Cursor가 공식적으로 안내하는 형태에 맞춰 `.cursor/rules`, `.cursor/skills`, `.cursor/commands`를 유지합니다.

## Rules (`.cursor/rules/*.mdc`)

- **파일 형식**: Markdown + YAML **프론트매터**가 있는 `.mdc`.
- **위치**: 저장소 루트 기준 `.cursor/rules/`.
- **핵심 필드**
  - `description`: 규칙 요약 (선택 UI에 노출).
  - `alwaysApply: true|false`: 모든 대화에 적용 여부.
  - `globs: "**/*.cpp"` 등: 특정 파일 작업 시 자동 적용(파일 패턴).
- **작성 원칙**
  - 한 파일당 **한 가지 관심사** (OOAD 절차 / C++ 가독성 / 테스트 정책 등으로 분리).
  - **짧고 실행 가능한 문장** 위주 (장문 논문 형태 지양).
  - 예시는 짧은 GOOD/BAD 형태가 가장 잘 먹힙니다.

공식 가이드 성격의 요약은 Cursor 문서의 **Project Rules** 설명과 같은 맥락입니다.

## Skills (`.cursor/skills/<skill-name>/SKILL.md`)

- **디렉터리 구조**: `SKILL.md` 필수, 필요 시 `reference.md`, `examples.md`, `scripts/` 추가.
- **위치**
  - 프로젝트 스킬: `.cursor/skills/…` (이 저장소가 표준).
  - 개인 스킬: 사용자 홈의 Cursor skills 디렉터리 (팀과 공유되지 않음).
- **프론트매터**
  - `name`: 소문자, 숫자, 하이픈 (고유 ID).
  - `description`: 언제 이 스킬을 써야 하는지 **구체적으로** (검색/추천 품질을 좌우).
  - `disable-model-invocation`: 기본적으로 스킬은 **명시적으로 불렀을 때만** 쓰도록 두는 경우가 많음. 항상 자동 로드가 필요하면 규칙에서 스킬 이름을 언급하거나 설명을 매우 구체화합니다.
- **본문**
  - 단계별 체크리스트, 산출물 경로, 완료 정의(DoD), 이전 단계 파일을 읽고 진행하는 지침을 넣습니다.

## Commands (`.cursor/commands/*.md`)

- **파일 형식**: Markdown + YAML 프론트매터.
- **본문**: 사용자가 커맨드를 실행했을 때 에이전트가 따를 **짧은 SOP**(표준 작업 절차)나 프롬프트 템플릿.
- **역할**: 스킬 전체가 아니라 **한 단계만** 빠르게 진행할 때 유용합니다.

## 이 저장소에서의 역할 분담

| 자산 | 역할 |
|------|------|
| Rules | OOAD 단계 준수, TDD/커버리지/CI 불변조건, C++/SOLID·가독성 |
| Skill `rvc-ooad-vnv` | 요구사항→설계→구현→검증까지 **전체 파이프라인** |
| Commands `rvc-0x-*` | Inception/OOA/OOD/OOI/CI 단계별 **진입점** |

## 참고

- Cursor 내장 스킬 디렉터리(`skills-cursor` 등)는 **사용자가 직접 수정하지 않는 것**을 권장합니다. 프로젝트 표준은 항상 `.cursor/skills/`에 둡니다.
