//
// Program: YAML_Config_Manager
//
// Description: Demonstrates a practical application-configuration workflow:
// load settings from a YAML file, read typed values by key path, modify
// two entries at runtime, write the updated config back to disk, and verify
// the round-trip by re-parsing the saved file.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"

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
    PLOG_INFO << "  [" << section.getKey() << "]";
    if (yl::isA<yl::Dictionary>(section.getNode())) {
      for (const auto &field :
           yl::NRef<yl::Dictionary>(section.getNode()).value()) {
        PLOG_INFO << "    " << field.getKey() << " = "
                  << field.getNode().toString();
      }
    }
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
    init(plog::debug, "YAML_Config_Manager.log");
    PLOG_INFO << "YAML_Config_Manager started ...";
    PLOG_INFO << YAML_Lib::YAML::version();

    yl::YAML yaml;

    // --- Load config from file ---
    PLOG_INFO << "Loading config: " << kConfigFile;
    yaml.parse(yl::FileSource{kConfigFile});

    // Read specific typed values by key path
    auto &doc = yaml.document(0);
    PLOG_INFO << "database.host      = "
              << yl::NRef<yl::String>(doc["database"]["host"]).value();
    PLOG_INFO << "database.port      = "
              << yl::NRef<yl::Number>(doc["database"]["port"]).value<int>();
    PLOG_INFO << "server.workers     = "
              << yl::NRef<yl::Number>(doc["server"]["workers"]).value<int>();
    PLOG_INFO << "logging.level      = "
              << yl::NRef<yl::String>(doc["logging"]["level"]).value();
    PLOG_INFO << "logging.console    = "
              << (yl::NRef<yl::Boolean>(doc["logging"]["console"]).value()
                      ? "true"
                      : "false");

    PLOG_INFO << "--- Full config (before update) ---";
    dumpConfig(yaml);

    // --- Modify two values at runtime ---
    // Scale up from 4 workers to 8 for a production deployment
    doc["server"]["workers"] = 8;
    // Switch to verbose logging
    doc["logging"]["level"] = std::string("debug");

    PLOG_INFO << "--- After update ---";
    PLOG_INFO << "server.workers (updated) = "
              << yl::NRef<yl::Number>(doc["server"]["workers"]).value<int>();
    PLOG_INFO << "logging.level  (updated) = "
              << yl::NRef<yl::String>(doc["logging"]["level"]).value();

    // --- Persist updated config ---
    yaml.stringify(yl::FileDestination{kUpdatedConfigFile});
    PLOG_INFO << "Updated config written to: " << kUpdatedConfigFile;

    // --- Re-parse to verify round-trip ---
    yl::YAML verify;
    verify.parse(yl::FileSource{kUpdatedConfigFile});
    PLOG_INFO << "--- Re-parsed config (round-trip check) ---";
    PLOG_INFO << "server.workers = "
              << yl::NRef<yl::Number>(verify.document(0)["server"]["workers"])
                     .value<int>();
    PLOG_INFO
        << "logging.level  = "
        << yl::NRef<yl::String>(verify.document(0)["logging"]["level"]).value();

  } catch (const std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "YAML_Config_Manager exited.";
  exit(EXIT_SUCCESS);
}
