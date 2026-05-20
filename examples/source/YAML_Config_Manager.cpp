//
// Program: YAML_Config_Manager
//
// Description: Demonstrates a practical application-configuration workflow:
// load settings from a YAML file, read typed values by key path, modify
// two entries at runtime, write the updated config back to disk, and verify
// the round-trip by re-parsing the saved file.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Utility.hpp"
#include <iostream>

namespace yl = YAML_Lib;
namespace fs = std::filesystem;

static const std::string kConfigFile =
    (fs::current_path() / "files" / "app_config.yaml").string();
static const std::string kUpdatedConfigFile =
    (fs::current_path() / "files" / "app_config_updated.yaml").string();

// ---------------------------------------------------------------------------
// Print all top-level sections and their immediate fields.
// ---------------------------------------------------------------------------
static void dumpConfig(const yl::YAML &yaml) {
  const auto &doc = yaml.document(0);
  for (const auto &section : yl::NRef<yl::Dictionary>(doc).value()) {
    std::cout << "  [" << section.getKey() << "]";
    if (yl::isA<yl::Dictionary>(section.getNode())) {
      for (const auto &field :
           yl::NRef<yl::Dictionary>(section.getNode()).value()) {
        std::cout << "    " << field.getKey() << " = "
                  << field.getNode().toString();
      }
    }
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
        std::cout << "YAML_Config_Manager started ...";
    std::cout << YAML_Lib::YAML::version();

    yl::YAML yaml;

    // --- Load config from file ---
    std::cout << "Loading config: " << kConfigFile;
    yaml.parse(yl::FileSource{kConfigFile});

    // Read specific typed values by key path
    auto &doc = yaml.document(0);
    std::cout << "database.host      = "
              << yl::NRef<yl::String>(doc["database"]["host"]).value();
    std::cout << "database.port      = "
              << yl::NRef<yl::Number>(doc["database"]["port"]).value<int>();
    std::cout << "server.workers     = "
              << yl::NRef<yl::Number>(doc["server"]["workers"]).value<int>();
    std::cout << "logging.level      = "
              << yl::NRef<yl::String>(doc["logging"]["level"]).value();
    std::cout << "logging.console    = "
              << (yl::NRef<yl::Boolean>(doc["logging"]["console"]).value()
                      ? "true"
                      : "false");

    std::cout << "--- Full config (before update) ---";
    dumpConfig(yaml);

    // --- Modify two values at runtime ---
    // Scale up from 4 workers to 8 for a production deployment
    doc["server"]["workers"] = 8;
    // Switch to verbose logging
    doc["logging"]["level"] = std::string("debug");

    std::cout << "--- After update ---";
    std::cout << "server.workers (updated) = "
              << yl::NRef<yl::Number>(doc["server"]["workers"]).value<int>();
    std::cout << "logging.level  (updated) = "
              << yl::NRef<yl::String>(doc["logging"]["level"]).value();

    // --- Persist updated config ---
    yaml.stringify(yl::FileDestination{kUpdatedConfigFile});
    std::cout << "Updated config written to: " << kUpdatedConfigFile;

    // --- Re-parse to verify round-trip ---
    yl::YAML verify;
    verify.parse(yl::FileSource{kUpdatedConfigFile});
    std::cout << "--- Re-parsed config (round-trip check) ---";
    std::cout << "server.workers = "
              << yl::NRef<yl::Number>(verify.document(0)["server"]["workers"])
                     .value<int>();
    std::cout
        << "logging.level  = "
        << yl::NRef<yl::String>(verify.document(0)["logging"]["level"]).value();

  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what();
  }
  std::cout << "YAML_Config_Manager exited.";
  exit(EXIT_SUCCESS);
}
