#include "rvc/cleaning_coordinator.hpp"

#include <algorithm>

namespace rvc {

CleaningCoordinator::CleaningCoordinator(ISensorPort& sensor, IActuatorPort& actuator,
                                         const ControllerConfig& config)
    : sensor_{sensor},
      actuator_{actuator},
      config_{config},
      power_{config_.max_boost_level} {}

void CleaningCoordinator::clear_trace() { trace_.clear(); }

void CleaningCoordinator::record_state() { trace_.push_back(to_string(display_)); }

void CleaningCoordinator::on_perception(const PerceptionSnapshot& snapshot) {
  perception_push_ = snapshot;
}

void CleaningCoordinator::on_dust_detected(int level) {
  dust_push_level_ = std::max(dust_push_level_, level);
}

void CleaningCoordinator::merge_dust_push(PerceptionSnapshot& snap) {
  if (dust_push_level_ > 0) {
    snap.dust_level = std::max(snap.dust_level, dust_push_level_);
    dust_push_level_ = 0;
  }
}

void CleaningCoordinator::update_dust_debounce(const PerceptionSnapshot& snap) {
  if (snap.dust_level >= config_.dust_level_threshold) {
    ++dust_streak_;
  } else {
    dust_streak_ = 0;
  }
}

bool CleaningCoordinator::dust_confirmed_for_boost() const {
  return dust_streak_ >= config_.dust_debounce_consecutive_ticks;
}

PerceptionSnapshot CleaningCoordinator::acquire_snapshot() {
  PerceptionSnapshot snap =
      perception_push_.has_value() ? *perception_push_ : sensor_.read();
  perception_push_.reset();
  merge_dust_push(snap);
  return snap;
}

void CleaningCoordinator::apply_cleaning_power_command() {
  const CleaningPowerLevel p = power_.current_power();
  actuator_.set_cleaning_power(p);
}

void CleaningCoordinator::on_user_command(UserCommand cmd) {
  if (cmd == UserCommand::Start) {
    session_ = SessionState::Cleaning;
    display_ = DisplayState::Idle;
    pending_.clear();
    dust_streak_ = 0;
    dust_push_level_ = 0;
    perception_push_.reset();
    power_ = CleaningPowerPolicy{config_.max_boost_level};
    return;
  }
  if (cmd == UserCommand::Stop) {
    pending_.clear();
    actuator_.stop_motion();
    power_.resume_normal();
    apply_cleaning_power_command();
    session_ = SessionState::Idle;
    display_ = DisplayState::Session_Stopping;
    record_state();
    display_ = DisplayState::Idle;
    record_state();
  }
}

void CleaningCoordinator::enqueue_avoidance(const ManeuverPlan& plan) {
  power_.hold_for_maneuver();
  apply_cleaning_power_command();
  pending_.push_back({StepOp::Stop, plan.side});
  pending_.push_back({StepOp::Turn, plan.side});
  pending_.push_back({StepOp::Forward, plan.side});
}

void CleaningCoordinator::enqueue_escape(const EscapePlan& plan) {
  power_.hold_for_maneuver();
  apply_cleaning_power_command();
  pending_.push_back({StepOp::Stop, plan.side});
  for (int i = 0; i < plan.reverse_steps; ++i) {
    pending_.push_back({StepOp::Reverse, plan.side});
  }
  pending_.push_back({StepOp::Turn, plan.side});
  pending_.push_back({StepOp::Forward, plan.side});
}

void CleaningCoordinator::execute_next_pending() {
  if (pending_.empty()) {
    return;
  }
  const QueuedStep step = pending_.front();
  pending_.pop_front();
  switch (step.op) {
    case StepOp::Stop:
      actuator_.stop_motion();
      display_ = DisplayState::Maneuver_Stop;
      break;
    case StepOp::Turn:
      actuator_.turn_aside(step.side);
      display_ = DisplayState::Maneuver_Turn;
      break;
    case StepOp::Reverse:
      actuator_.reverse_cells(1);
      display_ = DisplayState::Maneuver_Reverse;
      break;
    case StepOp::Forward:
      power_.resume_normal();
      apply_cleaning_power_command();
      actuator_.forward_clean(power_.current_power());
      display_ = (power_.current_power() == CleaningPowerLevel::Boost)
                     ? DisplayState::Cleaning_Forward_Boost
                     : DisplayState::Cleaning_Forward;
      break;
  }
  if (pending_.empty()) {
    power_.resume_normal();
    apply_cleaning_power_command();
  }
}

void CleaningCoordinator::tick() {
  power_.on_tick();

  if (session_ != SessionState::Cleaning) {
    display_ = DisplayState::Idle;
    record_state();
    return;
  }

  if (!pending_.empty()) {
    execute_next_pending();
    record_state();
    return;
  }

  const PerceptionSnapshot snap = acquire_snapshot();

  const ObstacleClass obs = nav_.classify_obstacle(snap);
  if (obs == ObstacleClass::AllSidesBlocked) {
    enqueue_escape(nav_.plan_escape_enclosure(snap));
    execute_next_pending();
    record_state();
    return;
  }
  if (obs == ObstacleClass::PartialNotEnclosure && snap.front_blocked) {
    const ManeuverPlan plan = nav_.plan_avoidance(snap);
    enqueue_avoidance(plan);
    execute_next_pending();
    record_state();
    return;
  }

  update_dust_debounce(snap);
  if (dust_confirmed_for_boost()) {
    const int lvl = std::clamp(snap.dust_level, 1, config_.max_boost_level);
    power_.schedule_boost(lvl, config_.boost_duration_ticks);
  }

  if (nav_.should_continue_forward(snap)) {
    apply_cleaning_power_command();
    actuator_.forward_clean(power_.current_power());
    display_ = (power_.current_power() == CleaningPowerLevel::Boost)
                   ? DisplayState::Cleaning_Forward_Boost
                   : DisplayState::Cleaning_Forward;
  } else {
    apply_cleaning_power_command();
    actuator_.stop_motion();
    display_ = DisplayState::Maneuver_Stop;
  }

  record_state();
}

}  // namespace rvc
