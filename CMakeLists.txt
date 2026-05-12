cmake_minimum_required(VERSION 3.20)

project(rvc_controller LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(RVC_ENABLE_COVERAGE "Enable GCC/Clang coverage flags" OFF)

if(RVC_ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
  else()
    message(WARNING "Coverage flags are only wired for GCC/Clang; disabling.")
    set(RVC_ENABLE_COVERAGE OFF)
  endif()
endif()

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

enable_testing()

add_library(rvc_core
  src/rvc/types.cpp
  src/rvc/cleaning_coordinator.cpp
  src/rvc/grid_world.cpp
)
target_include_directories(rvc_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(rvc_core PUBLIC cxx_std_17)

add_executable(rvc_sim apps/rvc_sim.cpp)
target_link_libraries(rvc_sim PRIVATE rvc_core nlohmann_json::nlohmann_json)
target_compile_features(rvc_sim PRIVATE cxx_std_17)

add_executable(rvc_tests
  tests/cleaning_coordinator_test.cpp
  tests/grid_world_test.cpp
  tests/integration_test.cpp
  tests/types_test.cpp
)
target_link_libraries(rvc_tests PRIVATE rvc_core GTest::gtest_main)
target_compile_features(rvc_tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(rvc_tests)
