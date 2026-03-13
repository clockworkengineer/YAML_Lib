//
// Program: YAML_Nested_Structure_Demo
//
// Description: Creates and manipulates a nested YAML structure, then writes it
// to a file.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
    // Initialise logging.
    init(plog::debug, "YAML_Nested_Structure_Demo.log");
    PLOG_INFO << "YAML_Nested_Structure_Demo started ...";
    PLOG_INFO << YAML_Lib::YAML::version();
    yl::YAML yaml;
    yaml["user"]["name"] = "Alice";
    yaml["user"]["roles"] = {"admin", "editor"};
    yaml["settings"]["theme"] = "dark";
    yaml["settings"]["notifications"] = true;
    yaml.stringify(yl::FileDestination("files/nested_demo.yaml"));
    PLOG_INFO << "Created nested YAML structure and wrote to file.";
  } catch (const std::exception &ex) {
    PLOG_ERROR << "Error: [" << ex.what() << "]\n";
  }
  PLOG_INFO << "YAML_Nested_Structure_Demo exited.";
  exit(EXIT_SUCCESS);
}
