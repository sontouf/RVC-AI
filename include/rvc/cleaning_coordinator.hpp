#pragma once

#include <deque>
#include <optional>
#include <string>
#include <vector>

#include "rvc/cleaning_power_policy.hpp"
#include "rvc/controller_config.hpp"
#include "rvc/navigation_policy.hpp"
#include "rvc/ports.hpp"
#include "rvc/types.hpp"

namespace rvc {

enum class UserCommand { Start, Stop };

/// Application layer — SSD 시스템 연산과의 매핑은 arch/design/implementation-mapping.md 참고.
class CleaningCoordinator {
 public:
  CleaningCoordinator(ISensorPort& sensor, IActuatorPort& actuator,
                      const ControllerConfig& config = {});

  /// UC-001 — `startAutoCleaning` / `stopAutoCleaning` 에 대응.
  void on_user_command(UserCommand cmd);

  /// 푸시형 인식(드라이버가 스냅샷을 넣을 때). 호출된 틱의 `tick()` 에서 1회 소비 후 센서 폴백.
  void on_perception(const PerceptionSnapshot& snapshot);

  /// 푸시형 먼지 이벤트(UC-005). 해당 `tick()` 의 병합 스냅샷에 반영 후 소비.
  void on_dust_detected(int level);

  /// 제어 루프 1스텝: 내부에서 `read()` 또는 푸시 스냅샷을 사용(Synchronous SSD).
  void tick();

  [[nodiscard]] SessionState session() const { return session_; }
  [[nodiscard]] DisplayState display_state() const { return display_; }
  [[nodiscard]] const std::vector<std::string>& trace_states() const { return trace_; }

  [[nodiscard]] int boost_ticks_remaining() const { return power_.boost_ticks_remaining(); }

  void clear_trace();

 private:
  enum class StepOp { Stop, Turn, Forward, Reverse };

  struct QueuedStep {
    StepOp op{};
    AvoidSide side{AvoidSide::Right};
  };

  [[nodiscard]] PerceptionSnapshot acquire_snapshot();
  void merge_dust_push(PerceptionSnapshot& snap);
  void update_dust_debounce(const PerceptionSnapshot& snap);
  [[nodiscard]] bool dust_confirmed_for_boost() const;
  void apply_cleaning_power_command();
  void record_state();
  void enqueue_avoidance(const ManeuverPlan& plan);
  void enqueue_escape(const EscapePlan& plan);
  void execute_next_pending();

  ISensorPort& sensor_;
  IActuatorPort& actuator_;
  ControllerConfig config_;
  NavigationPolicy nav_;
  CleaningPowerPolicy power_;
  SessionState session_{SessionState::Idle};
  DisplayState display_{DisplayState::Idle};
  std::deque<QueuedStep> pending_;
  std::vector<std::string> trace_;

  std::optional<PerceptionSnapshot> perception_push_;
  int dust_push_level_{0};

  int dust_streak_{0};
};

}  // namespace rvc
