#include <gtest/gtest.h>

#include "rvc/types.hpp"

using namespace rvc;

TEST(Types, ToStringAllDisplayStates) {
  EXPECT_EQ(to_string(DisplayState::Idle), "Idle");
  EXPECT_EQ(to_string(DisplayState::Cleaning_Forward), "Cleaning_Forward");
  EXPECT_EQ(to_string(DisplayState::Cleaning_Forward_Boost), "Cleaning_Forward_Boost");
  EXPECT_EQ(to_string(DisplayState::Maneuver_Stop), "Maneuver_Stop");
  EXPECT_EQ(to_string(DisplayState::Maneuver_Turn), "Maneuver_Turn");
  EXPECT_EQ(to_string(DisplayState::Maneuver_Reverse), "Maneuver_Reverse");
  EXPECT_EQ(to_string(DisplayState::Session_Stopping), "Session_Stopping");
}

TEST(Types, ToStringUnknownEnumValue) {
  EXPECT_EQ(to_string(static_cast<DisplayState>(999)), "Unknown");
}
