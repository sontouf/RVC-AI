#include <algorithm>

#include "rvc/cleaning_power_policy.hpp"

namespace rvc {

CleaningPowerPolicy::CleaningPowerPolicy(int max_boost_level)
    : max_boost_level_(std::max(0, max_boost_level)) {}

void CleaningPowerPolicy::on_tick() {
  if (maneuver_hold_) {
    return;
  }
  if (boost_remaining_ <= 0) {
    active_boost_level_ = 0;
    return;
  }
  --boost_remaining_;
  if (boost_remaining_ <= 0) {
    active_boost_level_ = 0;
  }
}

void CleaningPowerPolicy::schedule_boost(int level, int duration_ticks) {
  if (duration_ticks <= 0) {
    return;
  }
  int clamped = std::clamp(level, 0, max_boost_level_);
  if (clamped <= 0) {
    return;
  }
  active_boost_level_ = std::max(active_boost_level_, clamped);
  boost_remaining_ += duration_ticks;
}

void CleaningPowerPolicy::hold_for_maneuver() { maneuver_hold_ = true; }

void CleaningPowerPolicy::resume_normal() { maneuver_hold_ = false; }

CleaningPowerLevel CleaningPowerPolicy::current_power() const {
  if (maneuver_hold_) {
    return CleaningPowerLevel::Normal;
  }
  if (boost_remaining_ > 0 && active_boost_level_ > 0) {
    return CleaningPowerLevel::Boost;
  }
  return CleaningPowerLevel::Normal;
}

}  // namespace rvc
