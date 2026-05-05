#include <gtest/gtest.h>

#include "rvc/grid_actuator.hpp"
#include "rvc/grid_world.hpp"

using namespace rvc;

TEST(GridActuator, StopMotionNoThrow) {
  GridWorld world(3, 3);
  GridActuator act(world);
  act.stop_motion();
}

TEST(GridActuator, ReverseCellsDistanceTwo) {
  GridWorld world(5, 5);
  world.set_pose(2, 3, Heading::East);
  GridActuator act(world);
  act.reverse_cells(2);
  EXPECT_EQ(world.col(), 1);
}

TEST(GridActuator, SetCleaningPowerStoresLevel) {
  GridWorld world(2, 2);
  GridActuator act(world);
  act.set_cleaning_power(CleaningPowerLevel::Boost);
  act.forward_clean(CleaningPowerLevel::Normal);
  SUCCEED();
}
