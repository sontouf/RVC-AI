# Class Diagram — 구현 뷰

## 핵심 타입

```mermaid
classDiagram
  class CleaningCoordinator {
    +reset()
    +next_tick(snapshot) TickCommand
    -maybe_refresh_boost(snapshot)
    -enqueue_avoidance(snapshot)
    -enqueue_dead_end_escape()
    -maneuver_queue_
    -boost_ticks_remaining_
  }
  class GridWorld {
    +GridWorld(grid_rows)
    +set_pose(pose)
    +sense() SensorSnapshot
    +apply(cmd) bool
    +cleaned_cells() int
  }
  class ScenarioRunner {
    +run(max_ticks) Result
  }
  namespace Types {
    class SensorSnapshot
    class TickCommand
    class DriveCommand
    class CleanerCommand
    class RobotPose
    class Heading
  }
  CleaningCoordinator ..> SensorSnapshot
  CleaningCoordinator ..> TickCommand
  GridWorld ..> SensorSnapshot
  GridWorld ..> TickCommand
  GridWorld ..> RobotPose
```

## 패키징(물리 모듈)

- `include/rvc/types.hpp` — 공용 열거형·구조체
- `include/rvc/cleaning_coordinator.hpp`
- `include/rvc/grid_world.hpp`
- `src/rvc/cleaning_coordinator.cpp`
- `src/rvc/grid_world.cpp`
- `apps/rvc_sim.cpp` — 시나리오 로더 + 실행 루프
