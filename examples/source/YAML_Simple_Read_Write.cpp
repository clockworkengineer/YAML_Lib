//
// Program: YAML_Simple_Read_Write
//
// Description: Reads a simple YAML file and writes it back out, demonstrating
// basic parse and stringify operations.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
    // Initialise logging.
        std::cout << "YAML_Simple_Read_Write started ...";
    std::cout << YAML_Lib::YAML::version();
    yl::YAML yaml;
    std::string inputFile = "files/simple.yaml";
    std::string outputFile = "files/simple_out.yaml";
    yaml.parse(yl::FileSource(inputFile));
    yaml.stringify(yl::FileDestination(outputFile));
    std::cout << "Parsed and wrote YAML file.";
  } catch (const std::exception &ex) {
    std::cerr << "Error: [" << ex.what() << "]\n";
  }
  std::cout << "YAML_Simple_Read_Write exited.";
  exit(EXIT_SUCCESS);
}
