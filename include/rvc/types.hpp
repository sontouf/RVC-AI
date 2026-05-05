#pragma once

#include <string>

namespace rvc {

enum class AvoidSide { Left, Right };

enum class ObstacleClass {
  Clear,
  PartialNotEnclosure,
  AllSidesBlocked,
};

enum class SessionState { Idle, Cleaning };

enum class CleaningPowerLevel { Normal, Boost };

enum class DisplayState {
  Idle,
  Cleaning_Forward,
  Cleaning_Forward_Boost,
  Maneuver_Stop,
  Maneuver_Turn,
  Maneuver_Reverse,
  Session_Stopping,
};

std::string to_string(DisplayState s);

struct PerceptionSnapshot {
  bool front_blocked{false};
  bool left_blocked{false};
  bool right_blocked{false};
  int dust_level{0};
};

struct ManeuverPlan {
  AvoidSide side{AvoidSide::Right};
};

struct EscapePlan {
  int reverse_steps{1};
  AvoidSide side{AvoidSide::Right};
};

}  // namespace rvc
