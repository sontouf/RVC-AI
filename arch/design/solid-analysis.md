# SOLID 분석 — RVC Control SW

## 단일 책임 원칙 (SRP)

| 클래스 | 책임 |
|--------|------|
| `CleaningCoordinator` | 센서 스냅샷을 받아 **청소·주행 명령을 결정**한다. |
| `GridWorld` | 2D 맵에서 **센서를 합성**하고 **물리적으로 명령을 적용**한다. |

시뮬레이터 GUI는 테스트 편의용으로 별도 프로세스(`sim/rvc_grid_gui.py`)에 두어 SW 본체 책임과 분리한다.

## 개방·폐쇄 원칙 (OCP)

알고리즘 변형(예: 회피 우선순위 변경)은 `CleaningCoordinator` 내부의 큐 기반 전략으로 국소화하여 **외부 공개 API 변경 없이** 수정 가능하도록 했다.

## 리스코프 치환 원칙 (LSP)

본 릴리스는 단일 구현체 중심이나, 향후 `ICleaningPolicy` 인터페이스를 도입할 때에도 `next_tick` 계약을 유지하면 클라이언트(`GridWorld`/`ScenarioRunner`)가 치환 가능하다.

## 인터페이스 분리 원칙 (ISP)

대형 “만능 로봇” 인터페이스 대신 `DriveCommand`/`CleanerCommand`로 **출력 신호를 분리**했다.

## 의존성 역전 원칙 (DIP)

시뮬레이션 루프는 저수준 세부가 아니라 `CleaningCoordinator`의 결정 결과(`TickCommand`)에 의존한다. HW 적층은 이 명령 타입만 해석하면 된다.
