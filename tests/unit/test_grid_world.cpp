#include <gtest/gtest.h>

#include "rvc/grid_world.hpp"

using namespace rvc;

TEST(GridWorld, OutOfBoundsSetObstacleIgnored) {
  GridWorld w(3, 3);
  w.set_obstacle(-1, 0, true);
  w.set_obstacle(0, -1, true);
  w.set_obstacle(99, 0, true);
  w.set_obstacle(0, 99, true);
  EXPECT_FALSE(w.obstacle_at(0, 0));
}

TEST(GridWorld, OutOfBoundsSetDustIgnored) {
  GridWorld w(3, 3);
  w.set_dust(-1, 0, 5);
  EXPECT_EQ(w.dust_at(0, 0), 0);
}

TEST(GridWorld, ObstacleAtOutOfBoundsTreatsAsBlocked) {
  GridWorld w(3, 3);
  EXPECT_TRUE(w.obstacle_at(-1, 0));
  EXPECT_TRUE(w.obstacle_at(0, 99));
}

TEST(GridWorld, DustAtOutOfBoundsReturnsZero) {
  GridWorld w(3, 3);
  EXPECT_EQ(w.dust_at(5, 5), 0);
}

TEST(GridWorld, MoveForwardBlockedByObstacle) {
  GridWorld w(4, 4);
  w.set_pose(1, 1, Heading::East);
  w.set_obstacle(1, 2, true);
  w.move_forward();
  EXPECT_EQ(w.row(), 1);
  EXPECT_EQ(w.col(), 1);
}

TEST(GridWorld, MoveForwardStoppedAtBorder) {
  GridWorld w(3, 3);
  w.set_pose(0, 1, Heading::North);
  w.move_forward();
  EXPECT_EQ(w.row(), 0);
}

TEST(GridWorld, MoveBackwardStoppedOutOfGridHeadingSouth) {
  GridWorld w(3, 3);
  w.set_pose(0, 1, Heading::South);
  w.move_backward();
  EXPECT_EQ(w.row(), 0);
}

TEST(GridWorld, MoveBackwardIntoFreeCell) {
  GridWorld w(4, 4);
  w.set_pose(1, 2, Heading::East);
  w.move_backward();
  EXPECT_EQ(w.row(), 1);
  EXPECT_EQ(w.col(), 1);
}

TEST(GridWorld, MoveBackwardBlockedByObstacle) {
  GridWorld w(4, 4);
  w.set_pose(1, 2, Heading::East);
  w.set_obstacle(1, 1, true);
  w.move_backward();
  EXPECT_EQ(w.col(), 2);
}

TEST(GridWorld, TurnRightAndLeft) {
  GridWorld w(2, 2);
  w.set_pose(0, 0, Heading::North);
  w.turn(AvoidSide::Right);
  EXPECT_EQ(w.heading(), Heading::East);
  w.turn(AvoidSide::Left);
  EXPECT_EQ(w.heading(), Heading::North);
}
