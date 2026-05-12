#include <gtest/gtest.h>

#include "rvc/grid_world.hpp"
#include "rvc/types.hpp"

using rvc::CleanerCommand;
using rvc::DriveCommand;
using rvc::GridWorld;
using rvc::Heading;
using rvc::RobotPose;
using rvc::TickCommand;

TEST(GridWorld, ForwardMovesEastWhenHeadingEast) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::East;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Forward;
  cmd.cleaner = CleanerCommand::Normal;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.pose().row, 1);
  EXPECT_EQ(world.pose().col, 2);
  EXPECT_GE(world.cleaned_cells(), 1);
}

TEST(GridWorld, SenseDetectsWallAhead) {
  GridWorld world;
  world.load_map({
      "###",
      "#.#",
      "###",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::East;
  world.set_pose(pose);

  const auto snapshot = world.sense();
  EXPECT_TRUE(snapshot.obstacle_front);
}

TEST(GridWorld, ForwardIntoWallReturnsFalse) {
  GridWorld world;
  world.load_map({
      "###",
      "#.#",
      "###",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::East;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Forward;
  cmd.cleaner = CleanerCommand::Normal;

  EXPECT_FALSE(world.apply(cmd));
}

TEST(GridWorld, BackwardMovesOppositeForward) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::East;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Backward;
  cmd.cleaner = CleanerCommand::Off;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.pose().row, 1);
  EXPECT_EQ(world.pose().col, 0);
}

TEST(GridWorld, DustSensorReadsOverlay) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  world.set_dust_cell(1, 1, true);

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::North;
  world.set_pose(pose);

  EXPECT_TRUE(world.sense().dust_detected);
}

TEST(GridWorld, IgnoresDustMarkersOutsideMap) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  world.set_dust_cell(99, 99, true);

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::North;
  world.set_pose(pose);

  EXPECT_FALSE(world.sense().dust_detected);
}
