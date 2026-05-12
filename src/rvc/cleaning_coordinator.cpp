#include "rvc/cleaning_coordinator.hpp"

namespace rvc {

CleaningCoordinator::CleaningCoordinator() {
  reset();
}

void CleaningCoordinator::reset() {
  boost_ticks_remaining_ = 0;
  while (!maneuver_queue_.empty()) {
    maneuver_queue_.pop();
  }
}

void CleaningCoordinator::maybe_refresh_boost(const SensorSnapshot& sensors) {
  if (sensors.dust_detected) {
    boost_ticks_remaining_ = kBoostDurationTicks;
  }
}

void CleaningCoordinator::enqueue_dead_end_escape() {
  for (int i = 0; i < kBackupStrideTicks; ++i) {
    maneuver_queue_.push({DriveCommand::Backward, CleanerCommand::Off});
  }
  maneuver_queue_.push({DriveCommand::TurnLeft, CleanerCommand::Off});
  maneuver_queue_.push({DriveCommand::Forward, CleanerCommand::Normal});
}

void CleaningCoordinator::enqueue_avoidance(const SensorSnapshot& sensors) {
  if (!sensors.obstacle_front) {
    return;
  }

  if (sensors.obstacle_front && sensors.obstacle_left && sensors.obstacle_right) {
    enqueue_dead_end_escape();
    return;
  }

  maneuver_queue_.push({DriveCommand::Stop, CleanerCommand::Off});

  if (!sensors.obstacle_left) {
    maneuver_queue_.push({DriveCommand::TurnLeft, CleanerCommand::Off});
  } else if (!sensors.obstacle_right) {
    maneuver_queue_.push({DriveCommand::TurnRight, CleanerCommand::Off});
  } else {
    enqueue_dead_end_escape();
    return;
  }

  maneuver_queue_.push({DriveCommand::Forward, CleanerCommand::Normal});
}

TickCommand CleaningCoordinator::next_tick(const SensorSnapshot& sensors) {
  maybe_refresh_boost(sensors);

  if (!maneuver_queue_.empty()) {
    TickCommand cmd = maneuver_queue_.front();
    maneuver_queue_.pop();

    if (cmd.drive == DriveCommand::Forward) {
      if (boost_ticks_remaining_ > 0) {
        cmd.cleaner = CleanerCommand::Boost;
        boost_ticks_remaining_--;
      }
    }

    return cmd;
  }

  if (!sensors.obstacle_front) {
    TickCommand out;
    out.drive = DriveCommand::Forward;
    if (boost_ticks_remaining_ > 0) {
      out.cleaner = CleanerCommand::Boost;
      boost_ticks_remaining_--;
    } else {
      out.cleaner = CleanerCommand::Normal;
    }
    return out;
  }

  enqueue_avoidance(sensors);
  TickCommand cmd = maneuver_queue_.front();
  maneuver_queue_.pop();
  return cmd;
}

}  // namespace rvc
