#pragma once

#include <queue>

#include "rvc/types.hpp"

namespace rvc {

class CleaningCoordinator {
 public:
  CleaningCoordinator();

  void reset();

  TickCommand next_tick(const SensorSnapshot& sensors);

 private:
  static constexpr int kBoostDurationTicks = 5;
  static constexpr int kBackupStrideTicks = 2;

  int boost_ticks_remaining_;
  std::queue<TickCommand> maneuver_queue_;

  void maybe_refresh_boost(const SensorSnapshot& sensors);
  void enqueue_avoidance(const SensorSnapshot& sensors);
  void enqueue_dead_end_escape();
};

}  // namespace rvc
