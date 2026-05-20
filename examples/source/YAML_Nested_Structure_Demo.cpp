//
// Program: YAML_Nested_Structure_Demo
//
// Description: Creates and manipulates a nested YAML structure, then writes it
// to a file.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Utility.hpp"
#include <iostream>

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
    // Initialise logging.
        std::cout << "YAML_Nested_Structure_Demo started ...";
    std::cout << YAML_Lib::YAML::version();
    yl::YAML yaml;
    yaml["user"]["name"] = "Alice";
    yaml["user"]["roles"] = {"admin", "editor"};
    yaml["settings"]["theme"] = "dark";
    yaml["settings"]["notifications"] = true;
    yaml.stringify(yl::FileDestination("files/nested_demo.yaml"));
    std::cout << "Created nested YAML structure and wrote to file.";
  } catch (const std::exception &ex) {
    std::cerr << "Error: [" << ex.what() << "]\n";
  }
  std::cout << "YAML_Nested_Structure_Demo exited.";
  exit(EXIT_SUCCESS);
}
