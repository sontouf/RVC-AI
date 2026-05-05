#include <algorithm>

#include <gtest/gtest.h>

#include "rvc/cleaning_coordinator.hpp"
#include "rvc/grid_actuator.hpp"
#include "rvc/grid_sensor.hpp"
#include "rvc/grid_world.hpp"

using namespace rvc;

TEST(CoordinatorGrid, ForwardMovesEast) {
  GridWorld world(5, 5);
  world.set_pose(2, 1, Heading::East);
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  coord.tick();
  EXPECT_EQ(world.row(), 2);
  EXPECT_EQ(world.col(), 2);
}

TEST(CoordinatorGrid, PartialAvoidThenForward) {
  GridWorld world(5, 5);
  world.set_pose(2, 1, Heading::East);
  world.set_obstacle(2, 2, true);
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  coord.tick();
  coord.tick();
  coord.tick();
  EXPECT_EQ(world.heading(), Heading::South);
  EXPECT_EQ(world.row(), 3);
  EXPECT_EQ(world.col(), 1);
}

TEST(CoordinatorGrid, DustSchedulesBoost) {
  GridWorld world(4, 4);
  world.set_pose(1, 1, Heading::East);
  world.set_dust(1, 1, 2);
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  coord.tick();
  const auto& tr = coord.trace_states();
  bool has_boost = false;
  for (const auto& s : tr) {
    if (s == "Cleaning_Forward_Boost") {
      has_boost = true;
    }
  }
  EXPECT_TRUE(has_boost);
}

TEST(CoordinatorGrid, DustDebouncesConsecutiveReadings) {
  GridWorld world(6, 6);
  world.set_pose(2, 2, Heading::East);
  GridSensor sensor(world);
  GridActuator actuator(world);
  ControllerConfig cfg;
  cfg.dust_debounce_consecutive_ticks = 2;
  cfg.boost_duration_ticks = 6;
  CleaningCoordinator coord(sensor, actuator, cfg);
  coord.on_user_command(UserCommand::Start);
  for (int i = 0; i < 3; ++i) {
    PerceptionSnapshot s{};
    s.dust_level = (i >= 1) ? 5 : 0;  // dust on 2nd and 3rd pushed perceptions
    coord.on_perception(s);
    coord.tick();
  }
  const auto& tr = coord.trace_states();
  auto it = std::find(tr.begin(), tr.end(), "Cleaning_Forward_Boost");
  EXPECT_NE(it, tr.end());
}
