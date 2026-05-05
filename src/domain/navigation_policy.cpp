#include "rvc/navigation_policy.hpp"

namespace rvc {

bool NavigationPolicy::should_continue_forward(const PerceptionSnapshot& snap) const {
  return !snap.front_blocked;
}

ObstacleClass NavigationPolicy::classify_obstacle(const PerceptionSnapshot& snap) const {
  const int blocked =
      (snap.front_blocked ? 1 : 0) + (snap.left_blocked ? 1 : 0) + (snap.right_blocked ? 1 : 0);
  if (blocked >= 3) {
    return ObstacleClass::AllSidesBlocked;
  }
  if (blocked == 0) {
    return ObstacleClass::Clear;
  }
  return ObstacleClass::PartialNotEnclosure;
}

ManeuverPlan NavigationPolicy::plan_avoidance(const PerceptionSnapshot& snap) const {
  ManeuverPlan plan;
  // Front blocked: use whichever side is open; both open → deterministic Left (FR-003 / UC-003 A1).
  if (snap.front_blocked) {
    const bool left_free = !snap.left_blocked;
    const bool right_free = !snap.right_blocked;
    if (left_free && !right_free) {
      plan.side = AvoidSide::Left;
    } else if (!left_free && right_free) {
      plan.side = AvoidSide::Right;
    } else if (left_free && right_free) {
      plan.side = AvoidSide::Left;
    } else {
      // Trapped on front; sides blocked — should be handled as enclosure elsewhere.
      plan.side = AvoidSide::Right;
    }
  } else {
    // Partial without front blocked: bias right (no turn needed for forward; plan unused).
    plan.side = AvoidSide::Right;
  }
  return plan;
}

EscapePlan NavigationPolicy::plan_escape_enclosure(const PerceptionSnapshot&) const {
  EscapePlan p;
  p.reverse_steps = 1;
  p.side = AvoidSide::Right;
  return p;
}

}  // namespace rvc
