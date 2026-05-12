# System Sequence Diagrams (SSD)

시스템(System)은 **RVC Control SW** 블랙박스로 본다.

## SSD-01 `tick()` — 센서 입력과 명령 출력

```mermaid
sequenceDiagram
  participant HW as HW 적층 / 시뮬레이터
  participant SYS as RVC Control SW
  HW->>SYS: submitSensorSnapshot(obs_front, obs_left, obs_right, dust)
  SYS->>SYS: decideNextCommand()
  SYS-->>HW: emitTickCommand(drive, cleaner)
```

## SSD-02 회피 시퀀스(요약)

```mermaid
sequenceDiagram
  participant HW as HW 적층 / 시뮬레이터
  participant SYS as RVC Control SW
  HW->>SYS: snapshot(전방 막힘)
  SYS-->>HW: Stop + CleanerOff
  HW->>SYS: snapshot(회전 준비 완료 가정)
  SYS-->>HW: TurnLeft/Right + CleanerOff
  HW->>SYS: snapshot(전방 개방)
  SYS-->>HW: Forward + CleanerOn
```

실제 구현은 한 틱에 하나의 `TickCommand`를 출력하며, SSD의 다중 메시지는 **여러 틱에 걸친 상호작용**으로 이해한다.
