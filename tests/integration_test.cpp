#include <gtest/gtest.h>

#include "rvc/cleaning_coordinator.hpp"
#include "rvc/grid_world.hpp"

using rvc::CleaningCoordinator;
using rvc::GridWorld;
using rvc::Heading;
using rvc::RobotPose;

TEST(Integration, RobotCleansMultipleCellsWithBrainAndWorld) {
  GridWorld world;
  world.load_map({
      ".......",
      ".......",
      ".......",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::East;
  world.set_pose(pose);

  CleaningCoordinator brain;
  brain.reset();

  for (int i = 0; i < 12; ++i) {
    const auto snapshot = world.sense();
    const auto cmd = brain.next_tick(snapshot);
    world.apply(cmd);
  }

  EXPECT_GT(world.cleaned_cells(), 2);
}
