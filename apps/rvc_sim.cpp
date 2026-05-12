#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "rvc/cleaning_coordinator.hpp"
#include "rvc/grid_world.hpp"
#include "rvc/types.hpp"

namespace {

using json = nlohmann::json;

rvc::Heading parse_heading(const std::string& value) {
  if (value == "N") {
    return rvc::Heading::North;
  }
  if (value == "E") {
    return rvc::Heading::East;
  }
  if (value == "S") {
    return rvc::Heading::South;
  }
  if (value == "W") {
    return rvc::Heading::West;
  }
  throw std::runtime_error("Unknown heading value: " + value);
}

json load_json_file(const std::string& path) {
  std::ifstream input(path);
  if (!input) {
    throw std::runtime_error("Failed to open scenario file: " + path);
  }
  json data;
  input >> data;
  return data;
}

int run_scenario(const json& scenario, bool jsonl) {
  const int max_ticks = scenario.at("max_ticks").get<int>();

  std::vector<std::string> rows;
  for (const auto& row : scenario.at("grid")) {
    rows.push_back(row.get<std::string>());
  }

  rvc::RobotPose pose;
  const json& start = scenario.at("start");
  pose.row = start.at("row").get<int>();
  pose.col = start.at("col").get<int>();
  pose.heading = parse_heading(start.at("heading").get<std::string>());

  rvc::GridWorld world;
  world.load_map(rows);

  if (scenario.contains("dust")) {
    for (const auto& spot : scenario.at("dust")) {
      const int r = spot.at(0).get<int>();
      const int c = spot.at(1).get<int>();
      world.set_dust_cell(r, c, true);
    }
  }

  world.set_pose(pose);

  rvc::CleaningCoordinator brain;
  brain.reset();

  for (int tick = 0; tick < max_ticks; ++tick) {
    const rvc::SensorSnapshot snapshot = world.sense();
    const rvc::TickCommand cmd = brain.next_tick(snapshot);
    world.apply(cmd);

    if (jsonl) {
      json line;
      line["tick"] = tick;
      line["drive"] = rvc::drive_command_to_string(cmd.drive);
      line["cleaner"] = rvc::cleaner_command_to_string(cmd.cleaner);
      line["row"] = world.pose().row;
      line["col"] = world.pose().col;
      std::cout << line.dump() << "\n";
    }
  }

  json summary;
  summary["result"] = "ok";
  summary["cleaned"] = world.cleaned_cells();
  summary["ticks"] = max_ticks;
  std::cout << summary.dump() << "\n";
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  bool jsonl = false;
  std::string scenario_path;

  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--jsonl") {
      jsonl = true;
      continue;
    }
    if (scenario_path.empty()) {
      scenario_path = arg;
      continue;
    }

    std::cerr << "Unexpected argument: " << arg << "\n";
    std::cerr << "Usage: rvc_sim [--jsonl] <scenario.json>\n";
    return 2;
  }

  if (scenario_path.empty()) {
    std::cerr << "Usage: rvc_sim [--jsonl] <scenario.json>\n";
    return 2;
  }

  try {
    const json scenario = load_json_file(scenario_path);
    return run_scenario(scenario, jsonl);
  } catch (const std::exception& ex) {
    std::cerr << "rvc_sim error: " << ex.what() << "\n";
    return 1;
  }
}
