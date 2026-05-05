#include "rvc/grid_sensor.hpp"

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
  return 0;
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
  return 0;
}

Heading left_of(Heading h) { return static_cast<Heading>((static_cast<int>(h) + 3) & 3); }

Heading right_of(Heading h) { return static_cast<Heading>((static_cast<int>(h) + 1) & 3); }

}  // namespace

GridSensor::GridSensor(const GridWorld& world) : world_{world} {}

PerceptionSnapshot GridSensor::read() const {
  PerceptionSnapshot s;
  const int r = world_.row();
  const int c = world_.col();
  const Heading h = world_.heading();
  const int fr = r + drow(h);
  const int fc = c + dcol(h);
  const int lr = r + drow(left_of(h));
  const int lc = c + dcol(left_of(h));
  const int rr = r + drow(right_of(h));
  const int rc = c + dcol(right_of(h));
  s.front_blocked = world_.is_blocked(fr, fc);
  s.left_blocked = world_.is_blocked(lr, lc);
  s.right_blocked = world_.is_blocked(rr, rc);
  s.dust_level = world_.dust_at(r, c);
  return s;
}

}  // namespace rvc
