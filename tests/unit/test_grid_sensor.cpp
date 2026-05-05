#include <gtest/gtest.h>

#include "rvc/grid_sensor.hpp"
#include "rvc/grid_world.hpp"

using namespace rvc;

namespace {

void expect_readings(int width, int height, int row, int col, Heading h) {
  GridWorld world(width, height);
  world.set_pose(row, col, h);
  GridSensor sensor(world);
  const PerceptionSnapshot s = sensor.read();
  EXPECT_GE(s.dust_level, 0);
  // Borders / OOB: sensor uses is_blocked which treats OOB as blocked
  (void)s;
}

}  // namespace

TEST(GridSensor, ReadAllHeadingsAtCenter) {
  GridWorld world(5, 5);
  world.set_pose(2, 2, Heading::North);
  GridSensor sn(world);
  (void)sn.read();
  world.set_pose(2, 2, Heading::South);
  (void)sn.read();
  world.set_pose(2, 2, Heading::East);
  (void)sn.read();
  world.set_pose(2, 2, Heading::West);
  (void)sn.read();
}

TEST(GridSensor, ReadDetectsDustAtCell) {
  GridWorld world(4, 4);
  world.set_pose(1, 1, Heading::East);
  world.set_dust(1, 1, 7);
  GridSensor sensor(world);
  EXPECT_EQ(sensor.read().dust_level, 7);
}

TEST(GridSensor, ReadAtCornerNorthWest) {
  expect_readings(4, 4, 0, 0, Heading::North);
}
