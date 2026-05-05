#pragma once

#include <cstdint>
#include <vector>

#include "rvc/types.hpp"

namespace rvc {

enum class Heading { North = 0, East = 1, South = 2, West = 3 };

class GridWorld {
 public:
  GridWorld(int width, int height);

  void set_obstacle(int row, int col, bool blocked);
  void set_dust(int row, int col, int level);

  [[nodiscard]] int width() const { return width_; }
  [[nodiscard]] int height() const { return height_; }
  [[nodiscard]] bool obstacle_at(int row, int col) const;
  [[nodiscard]] int dust_at(int row, int col) const;

  void set_pose(int row, int col, Heading h);
  [[nodiscard]] int row() const { return row_; }
  [[nodiscard]] int col() const { return col_; }
  [[nodiscard]] Heading heading() const { return heading_; }

  [[nodiscard]] bool is_blocked(int row, int col) const;

  void move_forward();
  void move_backward();
  void turn(AvoidSide side);

 private:
  int width_{0};
  int height_{0};
  int row_{0};
  int col_{0};
  Heading heading_{Heading::North};
  std::vector<uint8_t> obstacles_;
  std::vector<int> dust_;
  static int idx(int r, int c, int w) { return r * w + c; }
};

}  // namespace rvc
