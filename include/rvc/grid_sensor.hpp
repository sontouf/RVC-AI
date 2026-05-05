#pragma once

#include "rvc/ports.hpp"
#include "rvc/grid_world.hpp"

namespace rvc {

class GridSensor final : public ISensorPort {
 public:
  explicit GridSensor(const GridWorld& world);

  PerceptionSnapshot read() const override;

 private:
  const GridWorld& world_;
};

}  // namespace rvc
