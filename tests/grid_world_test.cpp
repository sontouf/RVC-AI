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

TEST(GridWorld, CleanerSkipsMarkWhenPoseOutOfBounds) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  RobotPose pose;
  pose.row = -1;
  pose.col = 1;
  pose.heading = Heading::East;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Stop;
  cmd.cleaner = CleanerCommand::Normal;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.cleaned_cells(), 0);
}

TEST(GridWorld, CleanerSkipsMarkWhenStandingOnObstacleCell) {
  GridWorld world;
  world.load_map({
      "#",
  });

  RobotPose pose;
  pose.row = 0;
  pose.col = 0;
  pose.heading = Heading::East;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Stop;
  cmd.cleaner = CleanerCommand::Normal;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.cleaned_cells(), 0);
}

TEST(GridWorld, TurnLeftAndTurnRightRotateHeading) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::North;
  world.set_pose(pose);

  TickCommand turn_left;
  turn_left.drive = DriveCommand::TurnLeft;
  turn_left.cleaner = CleanerCommand::Off;
  ASSERT_TRUE(world.apply(turn_left));
  EXPECT_EQ(world.pose().heading, Heading::West);

  TickCommand turn_right;
  turn_right.drive = DriveCommand::TurnRight;
  turn_right.cleaner = CleanerCommand::Off;
  ASSERT_TRUE(world.apply(turn_right));
  EXPECT_EQ(world.pose().heading, Heading::North);
}

TEST(GridWorld, ForwardWhenHeadingNorth) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::North;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Forward;
  cmd.cleaner = CleanerCommand::Off;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.pose().row, 0);
  EXPECT_EQ(world.pose().col, 1);
}

TEST(GridWorld, ForwardWhenHeadingSouth) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::South;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Forward;
  cmd.cleaner = CleanerCommand::Off;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.pose().row, 2);
  EXPECT_EQ(world.pose().col, 1);
}

TEST(GridWorld, ForwardWhenHeadingWest) {
  GridWorld world;
  world.load_map({
      "...",
      "...",
      "...",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::West;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Forward;
  cmd.cleaner = CleanerCommand::Off;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.pose().row, 1);
  EXPECT_EQ(world.pose().col, 0);
}

TEST(GridWorld, MarksCellCleanedOnlyOnce) {
  GridWorld world;
  world.load_map({
      ".",
  });

  RobotPose pose;
  pose.row = 0;
  pose.col = 0;
  pose.heading = Heading::East;
  world.set_pose(pose);

  TickCommand cmd;
  cmd.drive = DriveCommand::Stop;
  cmd.cleaner = CleanerCommand::Normal;

  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.cleaned_cells(), 1);
  ASSERT_TRUE(world.apply(cmd));
  EXPECT_EQ(world.cleaned_cells(), 1);
  EXPECT_TRUE(world.is_cell_cleaned(0, 0));
}

TEST(GridWorld, IsCellCleanedFalseOutsideMap) {
  GridWorld world;
  world.load_map({"."});
  RobotPose pose;
  pose.row = 0;
  pose.col = 0;
  pose.heading = Heading::East;
  world.set_pose(pose);
  EXPECT_FALSE(world.is_cell_cleaned(5, 5));
}

TEST(GridWorld, SenseUsesHeadingForRelativeDirections) {
  GridWorld world;
  world.load_map({
      "###",
      "#.#",
      "###",
  });

  RobotPose pose;
  pose.row = 1;
  pose.col = 1;
  pose.heading = Heading::North;
  world.set_pose(pose);

  const auto snapshot = world.sense();
  EXPECT_TRUE(snapshot.obstacle_front);
  EXPECT_TRUE(snapshot.obstacle_left);
  EXPECT_TRUE(snapshot.obstacle_right);
}
