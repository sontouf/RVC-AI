#include "rvc/types.hpp"

namespace rvc {

std::string to_string(DisplayState s) {
  switch (s) {
    case DisplayState::Idle:
      return "Idle";
    case DisplayState::Cleaning_Forward:
      return "Cleaning_Forward";
    case DisplayState::Cleaning_Forward_Boost:
      return "Cleaning_Forward_Boost";
    case DisplayState::Maneuver_Stop:
      return "Maneuver_Stop";
    case DisplayState::Maneuver_Turn:
      return "Maneuver_Turn";
    case DisplayState::Maneuver_Reverse:
      return "Maneuver_Reverse";
    case DisplayState::Session_Stopping:
      return "Session_Stopping";
  }
  return "Unknown";
}

}  // namespace rvc
