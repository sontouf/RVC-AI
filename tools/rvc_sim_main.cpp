#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_set>

#include "rvc/cleaning_coordinator.hpp"
#include "rvc/grid_actuator.hpp"
#include "rvc/grid_sensor.hpp"
#include "rvc/grid_world.hpp"

namespace {

rvc::Heading parse_heading(const std::string& s) {
  if (s == "N" || s == "North") {
    return rvc::Heading::North;
  }
  if (s == "E" || s == "East") {
    return rvc::Heading::East;
  }
  if (s == "S" || s == "South") {
    return rvc::Heading::South;
  }
  return rvc::Heading::West;
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
  return "?";
}

std::string session_to_string(rvc::SessionState s) {
  return (s == rvc::SessionState::Cleaning) ? "Cleaning" : "Idle";
}

}  // namespace

int main(int argc, char** argv) {
  std::string scenario_path;
  bool jsonl = false;
  int max_ticks_override = -1;

  for (int i = 1; i < argc; ++i) {
    const std::string a = argv[i];
    if (a == "--scenario" && i + 1 < argc) {
      scenario_path = argv[++i];
    } else if (a == "--jsonl") {
      jsonl = true;
    } else if (a == "--max-ticks" && i + 1 < argc) {
      max_ticks_override = std::stoi(argv[++i]);
    }
  }

  if (scenario_path.empty()) {
    std::cerr << "Usage: rvc_sim --scenario path.json [--jsonl] [--max-ticks N]\n";
    return 2;
  }

  std::ifstream in(scenario_path);
  if (!in) {
    std::cerr << "Cannot open " << scenario_path << "\n";
    return 2;
  }

  nlohmann::json doc;
  in >> doc;

  const int w = doc.at("width").get<int>();
  const int h = doc.at("height").get<int>();
  rvc::GridWorld world(w, h);

  if (doc.contains("obstacles")) {
    for (const auto& p : doc.at("obstacles")) {
      world.set_obstacle(p.at(0).get<int>(), p.at(1).get<int>(), true);
    }
  }

  if (doc.contains("dust")) {
    for (const auto& p : doc.at("dust")) {
      const int lvl = p.size() > 2 ? p.at(2).get<int>() : 1;
      world.set_dust(p.at(0).get<int>(), p.at(1).get<int>(), lvl);
    }
  }

  const auto& st = doc.at("start");
  world.set_pose(st.at("row").get<int>(), st.at("col").get<int>(),
                 parse_heading(st.at("heading").get<std::string>()));

  rvc::ControllerConfig cfg;
  if (doc.contains("controller")) {
    const auto& c = doc.at("controller");
    if (c.contains("boost_duration_ticks")) {
      cfg.boost_duration_ticks = c.at("boost_duration_ticks").get<int>();
    }
    if (c.contains("dust_debounce_consecutive_ticks")) {
      cfg.dust_debounce_consecutive_ticks = c.at("dust_debounce_consecutive_ticks").get<int>();
    }
    if (c.contains("dust_level_threshold")) {
      cfg.dust_level_threshold = c.at("dust_level_threshold").get<int>();
    }
    if (c.contains("max_boost_level")) {
      cfg.max_boost_level = c.at("max_boost_level").get<int>();
    }
  }

  rvc::GridSensor sensor(world);
  rvc::GridActuator actuator(world);
  rvc::CleaningCoordinator coord(sensor, actuator, cfg);

  int max_ticks = doc.value("max_ticks", 200);
  if (max_ticks_override > 0) {
    max_ticks = max_ticks_override;
  }

  const bool auto_start = doc.value("auto_start", true);

  coord.clear_trace();
  if (auto_start) {
    coord.on_user_command(rvc::UserCommand::Start);
  }

  nlohmann::json inject = doc.value("inject", nlohmann::json::array());

  for (int t = 0; t < max_ticks; ++t) {
    for (const auto& ev : inject) {
      if (ev.at("at_tick").get<int>() != t) {
        continue;
      }
      const std::string cmd = ev.at("command").get<std::string>();
      if (cmd == "Stop") {
        coord.on_user_command(rvc::UserCommand::Stop);
      } else if (cmd == "Start") {
        coord.on_user_command(rvc::UserCommand::Start);
      }
    }

    coord.tick();

    if (jsonl) {
      nlohmann::json line;
      line["tick"] = t;
      line["row"] = world.row();
      line["col"] = world.col();
      line["heading"] = heading_to_string(world.heading());
      line["display"] = rvc::to_string(coord.display_state());
      line["session"] = session_to_string(coord.session());
      line["dust_here"] = world.dust_at(world.row(), world.col());
      line["boost_ticks"] = coord.boost_ticks_remaining();
      std::cout << line.dump() << "\n";
    }

    if (doc.contains("goal_cell")) {
      const auto& g = doc.at("goal_cell");
      if (world.row() == g.at(0).get<int>() && world.col() == g.at(1).get<int>()) {
        break;
      }
    }
  }

  if (jsonl) {
    std::cout.flush();
  }

  const auto& trace = coord.trace_states();

  // JSONL consumers (e.g. system_tests/run_all.py) only see per-tick `display`.
  // Transient states like Session_Stopping may exist only in trace_states().
  if (jsonl) {
    std::unordered_set<std::string> uniq(trace.begin(), trace.end());
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& s : uniq) {
      arr.push_back(s);
    }
    nlohmann::json footer;
    footer["_trace_states"] = std::move(arr);
    std::cout << footer.dump() << "\n";
    std::cout.flush();
  }

  std::unordered_set<std::string> seen(trace.begin(), trace.end());

  bool ok = true;
  if (doc.contains("required_states")) {
    for (const auto& s : doc.at("required_states")) {
      const std::string req = s.get<std::string>();
      if (!seen.count(req)) {
        std::cerr << "Missing required state in trace: " << req << "\n";
        ok = false;
      }
    }
  }

  if (doc.contains("forbidden_states")) {
    for (const auto& f : doc.at("forbidden_states")) {
      const std::string fs = f.get<std::string>();
      if (seen.count(fs)) {
        std::cerr << "Forbidden state appeared: " << fs << "\n";
        ok = false;
      }
    }
  }

  if (!ok) {
    return 1;
  }
  return 0;
}
