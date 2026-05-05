#pragma once

#include "rvc/types.hpp"

namespace rvc {

class CleaningPowerPolicy {
 public:
  explicit CleaningPowerPolicy(int max_boost_level = 3);

  void on_tick();
  /// UC-005: 요청 레벨은 max_boost_level 로 클램프 후 타이머 연장(A1).
  void schedule_boost(int level, int duration_ticks);
  void hold_for_maneuver();
  void resume_normal();

  [[nodiscard]] CleaningPowerLevel current_power() const;

  [[nodiscard]] int boost_ticks_remaining() const { return boost_remaining_; }
  [[nodiscard]] int active_boost_level() const { return active_boost_level_; }
  [[nodiscard]] bool maneuver_hold() const { return maneuver_hold_; }

 private:
  int max_boost_level_{3};
  int boost_remaining_{0};
  int active_boost_level_{0};
  bool maneuver_hold_{false};
};

}  // namespace rvc
