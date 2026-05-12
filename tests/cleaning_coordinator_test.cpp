#include <gtest/gtest.h>

#include "rvc/cleaning_coordinator.hpp"

using rvc::CleanerCommand;
using rvc::CleaningCoordinator;
using rvc::DriveCommand;
using rvc::SensorSnapshot;

TEST(CleaningCoordinator, ForwardWhenClear_FR001) {
  CleaningCoordinator coordinator;
  coordinator.reset();

  SensorSnapshot sensors{};
  sensors.obstacle_front = false;

  const rvc::TickCommand cmd = coordinator.next_tick(sensors);
  EXPECT_EQ(cmd.drive, DriveCommand::Forward);
  EXPECT_EQ(cmd.cleaner, CleanerCommand::Normal);
}

TEST(CleaningCoordinator, AvoidancePrefersLeftWhenBothSidesOpen_FR002) {
  CleaningCoordinator coordinator;
  coordinator.reset();

  SensorSnapshot blocked{};
  blocked.obstacle_front = true;
  blocked.obstacle_left = false;
  blocked.obstacle_right = false;

  const rvc::TickCommand first = coordinator.next_tick(blocked);
  EXPECT_EQ(first.drive, DriveCommand::Stop);
  EXPECT_EQ(first.cleaner, CleanerCommand::Off);

  const rvc::TickCommand second = coordinator.next_tick(blocked);
  EXPECT_EQ(second.drive, DriveCommand::TurnLeft);
  EXPECT_EQ(second.cleaner, CleanerCommand::Off);

  SensorSnapshot clear{};
  clear.obstacle_front = false;

  const rvc::TickCommand third = coordinator.next_tick(clear);
  EXPECT_EQ(third.drive, DriveCommand::Forward);
  EXPECT_EQ(third.cleaner, CleanerCommand::Normal);
}

TEST(CleaningCoordinator, DeadEndTriggersBackup_FR003) {
  CleaningCoordinator coordinator;
  coordinator.reset();

  SensorSnapshot boxed{};
  boxed.obstacle_front = true;
  boxed.obstacle_left = true;
  boxed.obstacle_right = true;

  const rvc::TickCommand first = coordinator.next_tick(boxed);
  EXPECT_EQ(first.drive, DriveCommand::Backward);
  EXPECT_EQ(first.cleaner, CleanerCommand::Off);
}

TEST(CleaningCoordinator, BoostActivatesOnDust_FR004) {
  CleaningCoordinator coordinator;
  coordinator.reset();

  SensorSnapshot dusty{};
  dusty.obstacle_front = false;
  dusty.dust_detected = true;

  const rvc::TickCommand cmd = coordinator.next_tick(dusty);
  EXPECT_EQ(cmd.drive, DriveCommand::Forward);
  EXPECT_EQ(cmd.cleaner, CleanerCommand::Boost);
}

TEST(CleaningCoordinator, AvoidanceUsesRightWhenOnlyRightOpen_FR002) {
  CleaningCoordinator coordinator;
  coordinator.reset();

  SensorSnapshot blocked{};
  blocked.obstacle_front = true;
  blocked.obstacle_left = true;
  blocked.obstacle_right = false;

  const rvc::TickCommand stop = coordinator.next_tick(blocked);
  EXPECT_EQ(stop.drive, DriveCommand::Stop);

  const rvc::TickCommand turn = coordinator.next_tick(blocked);
  EXPECT_EQ(turn.drive, DriveCommand::TurnRight);
}

TEST(CleaningCoordinator, DeadEndEscapeSequence_FR003) {
  CleaningCoordinator coordinator;
  coordinator.reset();

  SensorSnapshot boxed{};
  boxed.obstacle_front = true;
  boxed.obstacle_left = true;
  boxed.obstacle_right = true;

  ASSERT_EQ(coordinator.next_tick(boxed).drive, DriveCommand::Backward);
  ASSERT_EQ(coordinator.next_tick(boxed).drive, DriveCommand::Backward);
  ASSERT_EQ(coordinator.next_tick(boxed).drive, DriveCommand::TurnLeft);
}

TEST(CleaningCoordinator, BoostDecrementsWhileForwarding_FR004) {
  CleaningCoordinator coordinator;
  coordinator.reset();

  SensorSnapshot dusty{};
  dusty.obstacle_front = false;
  dusty.dust_detected = true;

  ASSERT_EQ(coordinator.next_tick(dusty).cleaner, CleanerCommand::Boost);

  SensorSnapshot clear{};
  clear.obstacle_front = false;

  const rvc::TickCommand second = coordinator.next_tick(clear);
  EXPECT_EQ(second.drive, DriveCommand::Forward);
  EXPECT_EQ(second.cleaner, CleanerCommand::Boost);
}
