cmake_minimum_required(VERSION 3.16)
project(rvc_sw_controller LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(RVC_BUILD_TESTS "Build GTest targets" ON)
option(RVC_BUILD_SIM "Build scenario runner (rvc_sim)" ON)
option(RVC_ENABLE_COVERAGE "GCC/Clang: compile/link with --coverage (NFR-TEST-001)" OFF)

if(RVC_ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:--coverage>")
    add_link_options("$<$<COMPILE_LANGUAGE:CXX>:--coverage>")
  endif()
endif()

add_library(rvc_core
  src/types.cpp
  src/domain/navigation_policy.cpp
  src/domain/cleaning_power_policy.cpp
  src/app/cleaning_coordinator.cpp
  src/technical/grid_world.cpp
  src/technical/grid_sensor.cpp
  src/technical/grid_actuator.cpp
)
target_include_directories(rvc_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(rvc_core PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

if(RVC_BUILD_SIM)
  include(FetchContent)
  FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
  )
  FetchContent_MakeAvailable(json)
  add_executable(rvc_sim tools/rvc_sim_main.cpp)
  target_link_libraries(rvc_sim PRIVATE rvc_core nlohmann_json::nlohmann_json)
endif()

if(RVC_BUILD_TESTS)
  enable_testing()
  include(FetchContent)
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  add_executable(rvc_unit_tests
    tests/unit/test_navigation_policy.cpp
    tests/unit/test_cleaning_power_policy.cpp
  )
  target_link_libraries(rvc_unit_tests PRIVATE rvc_core GTest::gtest_main)
  add_executable(rvc_integration_tests
    tests/integration/test_coordinator_grid.cpp
  )
  target_link_libraries(rvc_integration_tests PRIVATE rvc_core GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(rvc_unit_tests DISCOVERY_MODE PRE_TEST)
  gtest_discover_tests(rvc_integration_tests DISCOVERY_MODE PRE_TEST)
endif()
