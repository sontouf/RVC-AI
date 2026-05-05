#pragma once

#include "rvc/types.hpp"

namespace rvc {

class NavigationPolicy {
 public:
  [[nodiscard]] bool should_continue_forward(const PerceptionSnapshot& snap) const;
  [[nodiscard]] ObstacleClass classify_obstacle(const PerceptionSnapshot& snap) const;
  [[nodiscard]] ManeuverPlan plan_avoidance(const PerceptionSnapshot& snap) const;
  [[nodiscard]] EscapePlan plan_escape_enclosure(const PerceptionSnapshot& snap) const;
};

}  // namespace rvc
