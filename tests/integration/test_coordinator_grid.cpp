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
  // Front blocked, left+right open → prefer left (North), then forward.
  EXPECT_EQ(world.heading(), Heading::North);
  EXPECT_EQ(world.row(), 1);
  EXPECT_EQ(world.col(), 1);
}

TEST(CoordinatorGrid, PartialAvoidPrefersRightWhenLeftBlocked) {
  GridWorld world(5, 5);
  world.set_pose(2, 1, Heading::East);
  world.set_obstacle(2, 2, true);
  world.set_obstacle(1, 1, true);  // left-of-heading (North from East)
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

TEST(CoordinatorGrid, PartialAvoidPrefersLeftWhenRightBlocked) {
  GridWorld world(5, 5);
  world.set_pose(2, 1, Heading::East);
  world.set_obstacle(2, 2, true);
  world.set_obstacle(3, 1, true);  // right (South from East)
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  coord.tick();
  coord.tick();
  coord.tick();
  EXPECT_EQ(world.heading(), Heading::North);
  EXPECT_EQ(world.row(), 1);
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

TEST(CoordinatorGrid, IdleTickRecordsIdle) {
  GridWorld world(3, 3);
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.tick();
  EXPECT_EQ(coord.session(), SessionState::Idle);
  ASSERT_FALSE(coord.trace_states().empty());
  EXPECT_EQ(coord.trace_states().back(), "Idle");
}

TEST(CoordinatorGrid, StopCommandTracesSessionStopping) {
  GridWorld world(3, 3);
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  coord.on_user_command(UserCommand::Stop);
  const auto& tr = coord.trace_states();
  auto it = std::find(tr.begin(), tr.end(), "Session_Stopping");
  EXPECT_NE(it, tr.end());
  EXPECT_EQ(coord.session(), SessionState::Idle);
}

TEST(CoordinatorGrid, ClearTraceEmptiesHistory) {
  GridWorld world(3, 3);
  world.set_pose(1, 1, Heading::East);
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  coord.tick();
  coord.clear_trace();
  EXPECT_TRUE(coord.trace_states().empty());
}

TEST(CoordinatorGrid, EnclosureRunsEscapeManeuvers) {
  GridWorld world(5, 5);
  world.set_pose(2, 2, Heading::East);
  world.set_obstacle(2, 3, true);  // front
  world.set_obstacle(1, 2, true);  // left
  world.set_obstacle(3, 2, true);  // right
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  bool saw_reverse = false;
  bool saw_stop = false;
  for (int i = 0; i < 12; ++i) {
    coord.tick();
    for (const auto& s : coord.trace_states()) {
      if (s == "Maneuver_Reverse") {
        saw_reverse = true;
      }
      if (s == "Maneuver_Stop") {
        saw_stop = true;
      }
    }
  }
  EXPECT_TRUE(saw_stop);
  EXPECT_TRUE(saw_reverse);
}

TEST(CoordinatorGrid, DustDetectedEventMergesIntoSnapshot) {
  GridWorld world(4, 4);
  world.set_pose(1, 1, Heading::East);
  GridSensor sensor(world);
  GridActuator actuator(world);
  ControllerConfig cfg;
  cfg.dust_debounce_consecutive_ticks = 1;
  cfg.boost_duration_ticks = 4;
  CleaningCoordinator coord(sensor, actuator, cfg);
  coord.on_user_command(UserCommand::Start);
  coord.on_dust_detected(4);
  coord.tick();
  const auto& tr = coord.trace_states();
  auto it = std::find(tr.begin(), tr.end(), "Cleaning_Forward_Boost");
  EXPECT_NE(it, tr.end());
}

TEST(CoordinatorGrid, DustBelowThresholdResetsDebounceStreak) {
  GridWorld world(4, 4);
  world.set_pose(1, 1, Heading::East);
  GridSensor sensor(world);
  GridActuator actuator(world);
  ControllerConfig cfg;
  cfg.dust_debounce_consecutive_ticks = 3;
  cfg.dust_level_threshold = 2;
  cfg.boost_duration_ticks = 4;
  CleaningCoordinator coord(sensor, actuator, cfg);
  coord.on_user_command(UserCommand::Start);
  coord.on_perception({false, false, false, 5});
  coord.tick();
  coord.on_perception({false, false, false, 0});  // below threshold — streak reset
  coord.tick();
  coord.on_perception({false, false, false, 5});
  coord.tick();
  const auto& tr = coord.trace_states();
  auto it = std::find(tr.begin(), tr.end(), "Cleaning_Forward_Boost");
  EXPECT_EQ(it, tr.end());
}

TEST(CoordinatorGrid, PartialOnlyLeftBlockedStillMovesForward) {
  GridWorld world(5, 5);
  world.set_pose(2, 2, Heading::East);
  world.set_obstacle(1, 2, true);  // left of robot (North) when facing East
  GridSensor sensor(world);
  GridActuator actuator(world);
  CleaningCoordinator coord(sensor, actuator);
  coord.on_user_command(UserCommand::Start);
  coord.tick();
  EXPECT_EQ(world.col(), 3);
}
