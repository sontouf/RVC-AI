// Line-oriented JSON daemon for interactive GUI: one authoritative tick per message.
// Protocol (stdin, one JSON object per line):
//   {"op":"init","grid":["..."],"dust":[[r,c],...],"start":{"row":r,"col":c,"heading":"E"}}
//   {"op":"tick"}
//   {"op":"quit"}
// Response: one JSON line on stdout; flush after each line.

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
  throw std::runtime_error("Unknown heading: " + value);
}

std::string heading_to_string(rvc::Heading h) {
  switch (h) {
    case rvc::Heading::North:
      return "N";
    case rvc::Heading::East:
      return "E";
    case rvc::Heading::South:
      return "S";
    case rvc::Heading::West:
      return "W";
  }
  return "N";
}

json pose_json(const rvc::RobotPose& pose) {
  json j;
  j["row"] = pose.row;
  j["col"] = pose.col;
  j["heading"] = heading_to_string(pose.heading);
  return j;
}

json snapshot_json(const rvc::SensorSnapshot& s) {
  json j;
  j["obstacle_front"] = s.obstacle_front;
  j["obstacle_left"] = s.obstacle_left;
  j["obstacle_right"] = s.obstacle_right;
  j["dust_detected"] = s.dust_detected;
  return j;
}

json cleaned_cells_array_bounded(const rvc::GridWorld& world, int rows, int cols) {
  json arr = json::array();
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (world.is_cell_cleaned(r, c)) {
        arr.push_back(json::array({r, c}));
      }
    }
  }
  return arr;
}

void apply_init(const json& msg, rvc::GridWorld& world, rvc::CleaningCoordinator& brain, int& rows, int& cols) {
  std::vector<std::string> rows_vec;
  for (const auto& row : msg.at("grid")) {
    rows_vec.push_back(row.get<std::string>());
  }
  if (rows_vec.empty()) {
    throw std::runtime_error("grid must be non-empty");
  }
  cols = static_cast<int>(rows_vec[0].size());
  for (const auto& row : rows_vec) {
    if (static_cast<int>(row.size()) != cols) {
      throw std::runtime_error("grid rows must have equal width");
    }
  }
  rows = static_cast<int>(rows_vec.size());

  world.load_map(rows_vec);
  const json& start = msg.at("start");
  rvc::RobotPose pose;
  pose.row = start.at("row").get<int>();
  pose.col = start.at("col").get<int>();
  pose.heading = parse_heading(start.at("heading").get<std::string>());
  world.set_pose(pose);

  if (msg.contains("dust")) {
    for (const auto& spot : msg.at("dust")) {
      const int r = spot.at(0).get<int>();
      const int c = spot.at(1).get<int>();
      world.set_dust_cell(r, c, true);
    }
  }

  brain.reset();
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);

  rvc::GridWorld world;
  rvc::CleaningCoordinator brain;
  int map_rows = 0;
  int map_cols = 0;
  int tick_count = 0;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    if (line.empty()) {
      continue;
    }

    json reply;
    try {
      const json msg = json::parse(line);
      const std::string op = msg.at("op").get<std::string>();

      if (op == "quit") {
        reply["ok"] = true;
        std::cout << reply.dump() << std::endl;
        return 0;
      }

      if (op == "init") {
        tick_count = 0;
        apply_init(msg, world, brain, map_rows, map_cols);
        const rvc::SensorSnapshot snap = world.sense();
        reply["ok"] = true;
        reply["tick"] = tick_count;
        reply["pose"] = pose_json(world.pose());
        reply["sensors"] = snapshot_json(snap);
        reply["drive"] = rvc::drive_command_to_string(rvc::DriveCommand::Stop);
        reply["cleaner"] = rvc::cleaner_command_to_string(rvc::CleanerCommand::Off);
        reply["cleaned"] = world.cleaned_cells();
        reply["cleaned_cells"] = cleaned_cells_array_bounded(world, map_rows, map_cols);
        std::cout << reply.dump() << std::endl;
        continue;
      }

      if (op == "tick") {
        if (map_rows == 0) {
          throw std::runtime_error("send init before tick");
        }
        const rvc::SensorSnapshot snap = world.sense();
        const rvc::TickCommand cmd = brain.next_tick(snap);
        world.apply(cmd);
        tick_count++;

        reply["ok"] = true;
        reply["tick"] = tick_count;
        reply["drive"] = rvc::drive_command_to_string(cmd.drive);
        reply["cleaner"] = rvc::cleaner_command_to_string(cmd.cleaner);
        reply["pose"] = pose_json(world.pose());
        reply["sensors"] = snapshot_json(world.sense());
        reply["cleaned"] = world.cleaned_cells();
        reply["cleaned_cells"] = cleaned_cells_array_bounded(world, map_rows, map_cols);
        std::cout << reply.dump() << std::endl;
        continue;
      }

      throw std::runtime_error("unknown op: " + op);
    } catch (const std::exception& ex) {
      reply["ok"] = false;
      reply["error"] = ex.what();
      std::cout << reply.dump() << std::endl;
    }
  }

  return 0;
}
