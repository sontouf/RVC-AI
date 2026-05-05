#include <gtest/gtest.h>

#include "rvc/navigation_policy.hpp"

using namespace rvc;

TEST(NavigationPolicy, ClassifyClear) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  EXPECT_EQ(nav.classify_obstacle(s), ObstacleClass::Clear);
}

TEST(NavigationPolicy, ClassifyPartial) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  s.front_blocked = true;
  EXPECT_EQ(nav.classify_obstacle(s), ObstacleClass::PartialNotEnclosure);
}

TEST(NavigationPolicy, ClassifyEnclosure) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  s.front_blocked = true;
  s.left_blocked = true;
  s.right_blocked = true;
  EXPECT_EQ(nav.classify_obstacle(s), ObstacleClass::AllSidesBlocked);
}

TEST(NavigationPolicy, AvoidanceWhenFrontBlocked) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  s.front_blocked = true;
  s.left_blocked = false;
  s.right_blocked = false;
  const ManeuverPlan p = nav.plan_avoidance(s);
  EXPECT_EQ(p.side, AvoidSide::Right);
}

TEST(NavigationPolicy, AvoidPrefersLeftWhenRightBlocked) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  s.front_blocked = true;
  s.left_blocked = false;
  s.right_blocked = true;
  const ManeuverPlan p = nav.plan_avoidance(s);
  EXPECT_EQ(p.side, AvoidSide::Left);
}

TEST(NavigationPolicy, AvoidanceBothSidesBlockedWhenFrontBlocked) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  s.front_blocked = true;
  s.left_blocked = true;
  s.right_blocked = true;
  const ManeuverPlan p = nav.plan_avoidance(s);
  EXPECT_EQ(p.side, AvoidSide::Right);
}

TEST(NavigationPolicy, AvoidanceBiasesRightWhenFrontClear) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  s.front_blocked = false;
  const ManeuverPlan p = nav.plan_avoidance(s);
  EXPECT_EQ(p.side, AvoidSide::Right);
}

TEST(NavigationPolicy, PlanEscapeEnclosure) {
  NavigationPolicy nav;
  PerceptionSnapshot s{};
  const EscapePlan p = nav.plan_escape_enclosure(s);
  EXPECT_EQ(p.reverse_steps, 1);
  EXPECT_EQ(p.side, AvoidSide::Right);
}
