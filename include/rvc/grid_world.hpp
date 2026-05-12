#pragma once

#include <string>
#include <vector>

#include "rvc/types.hpp"

namespace rvc {

class GridWorld {
 public:
  GridWorld();

  void load_map(const std::vector<std::string>& rows);
  void set_dust_cell(int row, int col, bool value);
  void set_pose(const RobotPose& pose);

  RobotPose pose() const;

  SensorSnapshot sense() const;

  bool apply(const TickCommand& cmd);

  int cleaned_cells() const;

 private:
  std::vector<std::string> grid_;
  std::vector<std::vector<bool>> dust_;
  std::vector<std::vector<bool>> cleaned_;
  RobotPose pose_;
  int cleaned_cells_;

  bool in_bounds(int row, int col) const;
  bool is_blocked(int row, int col) const;
  void ensure_layers();

  void turn_left();
  void turn_right();

  bool try_step_forward(int sign);
  void mark_clean_current_cell();
};

}  // namespace rvc
