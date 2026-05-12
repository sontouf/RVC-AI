#include "rvc/types.hpp"

namespace rvc {

const char* drive_command_to_string(DriveCommand cmd) {
  switch (cmd) {
    case DriveCommand::Forward:
      return "Forward";
    case DriveCommand::Backward:
      return "Backward";
    case DriveCommand::TurnLeft:
      return "TurnLeft";
    case DriveCommand::TurnRight:
      return "TurnRight";
    case DriveCommand::Stop:
      return "Stop";
  }

  return "Stop";
}

const char* cleaner_command_to_string(CleanerCommand cmd) {
  switch (cmd) {
    case CleanerCommand::Off:
      return "Off";
    case CleanerCommand::Normal:
      return "Normal";
    case CleanerCommand::Boost:
      return "Boost";
  }

  return "Off";
}

}  // namespace rvc
