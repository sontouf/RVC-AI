#include "rvc/grid_actuator.hpp"

namespace rvc {

GridActuator::GridActuator(GridWorld& world) : world_{world} {}

void GridActuator::stop_motion() {}

void GridActuator::forward_clean(CleaningPowerLevel power) {
  set_cleaning_power(power);
  last_power_ = power;
  world_.move_forward();
}

void GridActuator::turn_aside(AvoidSide side) { world_.turn(side); }

void GridActuator::reverse_cells(int distance) {
  for (int i = 0; i < distance; ++i) {
    world_.move_backward();
  }
}

void GridActuator::set_cleaning_power(CleaningPowerLevel level) { last_power_ = level; }

}  // namespace rvc
