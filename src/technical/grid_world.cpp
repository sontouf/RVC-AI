#include "rvc/grid_world.hpp"

namespace rvc {

namespace {

int drow(Heading h) {
  switch (h) {
    case Heading::North:
      return -1;
    case Heading::South:
      return 1;
    case Heading::East:
    case Heading::West:
      return 0;
  }
#if defined(__GNUC__) || defined(__clang__)
  __builtin_unreachable();
#else
  return 0;
#endif
}

int dcol(Heading h) {
  switch (h) {
    case Heading::East:
      return 1;
    case Heading::West:
      return -1;
    case Heading::North:
    case Heading::South:
      return 0;
  }
#if defined(__GNUC__) || defined(__clang__)
  __builtin_unreachable();
#else
  return 0;
#endif
}

Heading turn_cw(Heading h) {
  return static_cast<Heading>((static_cast<int>(h) + 1) & 3);
}

Heading turn_ccw(Heading h) {
  return static_cast<Heading>((static_cast<int>(h) + 3) & 3);
}

}  // namespace

GridWorld::GridWorld(int width, int height) : width_{width}, height_{height} {
  obstacles_.assign(static_cast<size_t>(width * height), 0);
  dust_.assign(static_cast<size_t>(width * height), 0);
}

void GridWorld::set_obstacle(int row, int col, bool blocked) {
  if (row < 0 || col < 0 || row >= height_ || col >= width_) {
    return;
  }
  obstacles_[static_cast<size_t>(idx(row, col, width_))] =
      static_cast<uint8_t>(blocked ? 1 : 0);
}

void GridWorld::set_dust(int row, int col, int level) {
  if (row < 0 || col < 0 || row >= height_ || col >= width_) {
    return;
  }
  dust_[static_cast<size_t>(idx(row, col, width_))] = level;
}

bool GridWorld::obstacle_at(int row, int col) const {
  if (row < 0 || col < 0 || row >= height_ || col >= width_) {
    return true;
  }
  return obstacles_[static_cast<size_t>(idx(row, col, width_))] != 0;
}

int GridWorld::dust_at(int row, int col) const {
  if (row < 0 || col < 0 || row >= height_ || col >= width_) {
    return 0;
  }
  return dust_[static_cast<size_t>(idx(row, col, width_))];
}

void GridWorld::set_pose(int row, int col, Heading h) {
  row_ = row;
  col_ = col;
  heading_ = h;
}

bool GridWorld::is_blocked(int row, int col) const { return obstacle_at(row, col); }

void GridWorld::move_forward() {
  const int nr = row_ + drow(heading_);
  const int nc = col_ + dcol(heading_);
  if (nr < 0 || nc < 0 || nr >= height_ || nc >= width_) {
    return;
  }
  if (obstacle_at(nr, nc)) {
    return;
  }
  row_ = nr;
  col_ = nc;
}

void GridWorld::move_backward() {
  const int nr = row_ - drow(heading_);
  const int nc = col_ - dcol(heading_);
  if (nr < 0 || nc < 0 || nr >= height_ || nc >= width_) {
    return;
  }
  if (obstacle_at(nr, nc)) {
    return;
  }
  row_ = nr;
  col_ = nc;
}

void GridWorld::turn(AvoidSide side) {
  if (side == AvoidSide::Right) {
    heading_ = turn_cw(heading_);
  } else {
    heading_ = turn_ccw(heading_);
  }
}

}  // namespace rvc
