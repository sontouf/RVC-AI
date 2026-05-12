#include "rvc/grid_world.hpp"

namespace {

void heading_delta(rvc::Heading heading, int& dr, int& dc) {
  using rvc::Heading;
  switch (heading) {
    case Heading::North:
      dr = -1;
      dc = 0;
      break;
    case Heading::South:
      dr = 1;
      dc = 0;
      break;
    case Heading::East:
      dr = 0;
      dc = 1;
      break;
    case Heading::West:
      dr = 0;
      dc = -1;
      break;
  }
}

rvc::Heading rotate_left(rvc::Heading heading) {
  using rvc::Heading;
  switch (heading) {
    case Heading::North:
      return Heading::West;
    case Heading::West:
      return Heading::South;
    case Heading::South:
      return Heading::East;
    case Heading::East:
      return Heading::North;
  }
  return Heading::North;  // GCOVR_EXCL_LINE
}

rvc::Heading rotate_right(rvc::Heading heading) {
  using rvc::Heading;
  switch (heading) {
    case Heading::North:
      return Heading::East;
    case Heading::East:
      return Heading::South;
    case Heading::South:
      return Heading::West;
    case Heading::West:
      return Heading::North;
  }
  return Heading::North;  // GCOVR_EXCL_LINE
}

}  // namespace

namespace rvc {

GridWorld::GridWorld() : cleaned_cells_(0) {}

RobotPose GridWorld::pose() const {
  return pose_;
}

int GridWorld::cleaned_cells() const {
  return cleaned_cells_;
}

void GridWorld::ensure_layers() {
  const int rows = static_cast<int>(grid_.size());
  int cols = 0;
  if (rows > 0) {
    cols = static_cast<int>(grid_[0].size());
  }

  dust_.assign(static_cast<size_t>(rows), std::vector<bool>(static_cast<size_t>(cols), false));
  cleaned_.assign(static_cast<size_t>(rows), std::vector<bool>(static_cast<size_t>(cols), false));
}

void GridWorld::load_map(const std::vector<std::string>& rows) {
  grid_ = rows;
  cleaned_cells_ = 0;
  ensure_layers();
}

void GridWorld::set_dust_cell(int row, int col, bool value) {
  if (!in_bounds(row, col)) {
    return;
  }
  dust_[static_cast<size_t>(row)][static_cast<size_t>(col)] = value;
}

void GridWorld::set_pose(const RobotPose& pose) {
  pose_ = pose;
}

bool GridWorld::in_bounds(int row, int col) const {
  if (row < 0 || col < 0) {
    return false;
  }
  if (row >= static_cast<int>(grid_.size())) {
    return false;
  }
  if (grid_.empty()) {
    return false;
  }
  if (col >= static_cast<int>(grid_[static_cast<size_t>(row)].size())) {
    return false;
  }
  return true;
}

bool GridWorld::is_blocked(int row, int col) const {
  if (!in_bounds(row, col)) {
    return true;
  }
  const char tile = grid_[static_cast<size_t>(row)][static_cast<size_t>(col)];
  return tile == '#';
}

SensorSnapshot GridWorld::sense() const {
  SensorSnapshot snapshot;

  int forward_dr = 0;
  int forward_dc = 0;
  heading_delta(pose_.heading, forward_dr, forward_dc);

  const int left_dr = -forward_dc;
  const int left_dc = forward_dr;

  const int right_dr = forward_dc;
  const int right_dc = -forward_dr;

  const int front_r = pose_.row + forward_dr;
  const int front_c = pose_.col + forward_dc;
  const int left_r = pose_.row + left_dr;
  const int left_c = pose_.col + left_dc;
  const int right_r = pose_.row + right_dr;
  const int right_c = pose_.col + right_dc;

  snapshot.obstacle_front = is_blocked(front_r, front_c);
  snapshot.obstacle_left = is_blocked(left_r, left_c);
  snapshot.obstacle_right = is_blocked(right_r, right_c);

  if (in_bounds(pose_.row, pose_.col)) {
    snapshot.dust_detected =
        dust_[static_cast<size_t>(pose_.row)][static_cast<size_t>(pose_.col)];
  }

  return snapshot;
}

void GridWorld::mark_clean_current_cell() {
  if (!in_bounds(pose_.row, pose_.col)) {
    return;
  }
  if (is_blocked(pose_.row, pose_.col)) {
    return;
  }

  const size_t row = static_cast<size_t>(pose_.row);
  const size_t col = static_cast<size_t>(pose_.col);
  if (!cleaned_[row][col]) {
    cleaned_[row][col] = true;
    cleaned_cells_++;
  }
}

void GridWorld::turn_left() {
  pose_.heading = rotate_left(pose_.heading);
}

void GridWorld::turn_right() {
  pose_.heading = rotate_right(pose_.heading);
}

bool GridWorld::try_step_forward(int sign) {
  int dr = 0;
  int dc = 0;
  heading_delta(pose_.heading, dr, dc);

  dr *= sign;
  dc *= sign;

  const int next_r = pose_.row + dr;
  const int next_c = pose_.col + dc;

  if (is_blocked(next_r, next_c)) {
    return false;
  }

  pose_.row = next_r;
  pose_.col = next_c;
  return true;
}

bool GridWorld::apply(const TickCommand& cmd) {
  if (cmd.cleaner != CleanerCommand::Off) {
    mark_clean_current_cell();
  }

  switch (cmd.drive) {
    case DriveCommand::Stop:
      return true;
    case DriveCommand::TurnLeft:
      turn_left();
      return true;
    case DriveCommand::TurnRight:
      turn_right();
      return true;
    case DriveCommand::Forward:
      return try_step_forward(1);
    case DriveCommand::Backward:
      return try_step_forward(-1);
  }

  return true;  // GCOVR_EXCL_LINE
}

}  // namespace rvc
