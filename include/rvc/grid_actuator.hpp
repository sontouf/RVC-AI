#pragma once

#include "rvc/ports.hpp"
#include "rvc/grid_world.hpp"

namespace rvc {

class GridActuator final : public IActuatorPort {
 public:
  explicit GridActuator(GridWorld& world);

  void stop_motion() override;
  void forward_clean(CleaningPowerLevel power) override;
  void turn_aside(AvoidSide side) override;
  void reverse_cells(int distance) override;
  void set_cleaning_power(CleaningPowerLevel level) override;

  [[nodiscard]] CleaningPowerLevel last_commanded_cleaning_power() const {
    return last_power_;
  }

 private:
  GridWorld& world_;
  CleaningPowerLevel last_power_{CleaningPowerLevel::Normal};
};

}  // namespace rvc
