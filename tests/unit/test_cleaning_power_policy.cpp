#include <gtest/gtest.h>

#include "rvc/cleaning_power_policy.hpp"

using namespace rvc;

TEST(CleaningPowerPolicy, BoostExpires) {
  CleaningPowerPolicy p{3};
  p.schedule_boost(1, 3);
  EXPECT_EQ(p.current_power(), CleaningPowerLevel::Boost);
  p.on_tick();
  p.on_tick();
  EXPECT_EQ(p.current_power(), CleaningPowerLevel::Boost);
  p.on_tick();
  EXPECT_EQ(p.current_power(), CleaningPowerLevel::Normal);
}

TEST(CleaningPowerPolicy, HoldForManeuverOverridesBoost) {
  CleaningPowerPolicy p{3};
  p.schedule_boost(1, 100);
  p.hold_for_maneuver();
  EXPECT_EQ(p.current_power(), CleaningPowerLevel::Normal);
  p.resume_normal();
  EXPECT_EQ(p.current_power(), CleaningPowerLevel::Boost);
}

TEST(CleaningPowerPolicy, BoostLevelClampedToMax) {
  CleaningPowerPolicy p{2};
  p.schedule_boost(99, 5);
  EXPECT_EQ(p.active_boost_level(), 2);
  EXPECT_EQ(p.current_power(), CleaningPowerLevel::Boost);
}

TEST(CleaningPowerPolicy, ZeroLevelDoesNotSchedule) {
  CleaningPowerPolicy p{3};
  p.schedule_boost(0, 10);
  EXPECT_EQ(p.boost_ticks_remaining(), 0);
}
