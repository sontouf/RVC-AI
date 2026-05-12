#pragma once

namespace rvc {

enum class DriveCommand { Forward, Backward, TurnLeft, TurnRight, Stop };

enum class CleanerCommand { Off, Normal, Boost };

struct SensorSnapshot {
  bool obstacle_front = false;
  bool obstacle_left = false;
  bool obstacle_right = false;
  bool dust_detected = false;
};

struct TickCommand {
  DriveCommand drive = DriveCommand::Stop;
  CleanerCommand cleaner = CleanerCommand::Off;
};

enum class Heading { North, East, South, West };

struct RobotPose {
  int row = 0;
  int col = 0;
  Heading heading = Heading::East;
};

const char* drive_command_to_string(DriveCommand cmd);
const char* cleaner_command_to_string(CleanerCommand cmd);

}  // namespace rvc
