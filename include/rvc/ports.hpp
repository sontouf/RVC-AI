#pragma once

#include "rvc/types.hpp"

namespace rvc {

struct ISensorPort {
  virtual ~ISensorPort() = default;
  virtual PerceptionSnapshot read() const = 0;
};

struct IActuatorPort {
  virtual ~IActuatorPort() = default;
  virtual void stop_motion() = 0;
  virtual void forward_clean(CleaningPowerLevel power) = 0;
  virtual void turn_aside(AvoidSide side) = 0;
  virtual void reverse_cells(int distance) = 0;
  virtual void set_cleaning_power(CleaningPowerLevel level) = 0;
};

}  // namespace rvc
