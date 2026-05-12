#include <gtest/gtest.h>

#include "rvc/types.hpp"

TEST(TypesStrings, DriveCommandsRoundTripLabels) {
  using rvc::DriveCommand;
  using rvc::drive_command_to_string;

  EXPECT_STREQ(drive_command_to_string(DriveCommand::Forward), "Forward");
  EXPECT_STREQ(drive_command_to_string(DriveCommand::Backward), "Backward");
  EXPECT_STREQ(drive_command_to_string(DriveCommand::TurnLeft), "TurnLeft");
  EXPECT_STREQ(drive_command_to_string(DriveCommand::TurnRight), "TurnRight");
  EXPECT_STREQ(drive_command_to_string(DriveCommand::Stop), "Stop");
}

TEST(TypesStrings, CleanerCommandsRoundTripLabels) {
  using rvc::CleanerCommand;
  using rvc::cleaner_command_to_string;

  EXPECT_STREQ(cleaner_command_to_string(CleanerCommand::Off), "Off");
  EXPECT_STREQ(cleaner_command_to_string(CleanerCommand::Normal), "Normal");
  EXPECT_STREQ(cleaner_command_to_string(CleanerCommand::Boost), "Boost");
}
