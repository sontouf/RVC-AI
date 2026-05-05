#pragma once

namespace rvc {

/// Tunables for UC-005 / NFR (T, debounce, clamp). Defaults match prior simulator behavior.
struct ControllerConfig {
  /// UC-005 Typical step 2 — 유지 시간 T (제어 틱 단위).
  int boost_duration_ticks{8};
  /// UC-005 E1 — 연속 확인 틱 수(임계 이상 먼지 레벨이 이 횟수만큼 연속일 때만 부스트 스케줄).
  int dust_debounce_consecutive_ticks{1};
  /// 이 값 **미만**이면 먼지 없음으로 간주(노이즈 억제).
  int dust_level_threshold{1};
  /// UC-005 E2 — 요청 부스트 레벨 상한(클램프). 레벨 0은 부스트 없음.
  int max_boost_level{3};
};

}  // namespace rvc
