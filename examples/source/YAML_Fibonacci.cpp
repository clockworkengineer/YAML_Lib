
//
// Program: YAML_Fibonacci
//
// Description: On each activation add the next entry in the fibonacci
// sequence to the array stored in fibonacci.yaml; if the file does
// not exist then create the initial sequence of [0,1,2].
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yml = YAML_Lib;

/// <summary>
/// Return Fibonacci yaml file name.
/// </summary>
/// <returns>YAML settings file name.</returns>
std::string yamlFibonacciFile() {
  return (std::filesystem::current_path() / "files" / "fibonacci.yaml")
      .string();
}
/// <summary>
/// Read in current fibonacci sequence from YAML file, calculate the
/// next in sequence and write back to YAML file.
/// </summary>
void nextFibonacci() {
  yml::YAML yaml;
  if (!std::filesystem::exists(yamlFibonacciFile())) {
    // If YAML file does not exist create initial sequence
    yaml.parse(yml::BufferSource{"---\n - 1\n - 1\n - 2\n...\n"});
  } else {
    // Parse in current sequence
    yaml.parse(yml::FileSource{yamlFibonacciFile()});
    auto &fibonacciArray = yml::YRef<yml::Array>(yaml.document(0));
    // Get index of last element
    const auto last = fibonacciArray.size() - 1;
    // Get last two in sequence
    const auto first =
        yml::YRef<yml::Number>(fibonacciArray[last - 1]).value<long>();
    const auto second =
        yml::YRef<yml::Number>(fibonacciArray[last]).value<long>();
    // Create new element for next in sequence
    fibonacciArray.add(yml::Node::make<yml::Number>(first + second));
  }
  // Write updated sequence back to file
  yaml.stringify(yml::FileDestination{yamlFibonacciFile()});
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
    // Initialise logging.
    init(plog::debug, "YAML_Fibonacci.log");
    PLOG_INFO << "YAML_Fibonacci started ...";
    // Log version
    PLOG_INFO << YAML_Lib::YAML::version();
    // Update current sequence
    nextFibonacci();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "YAML_Fibonacci exited.";
  exit(EXIT_SUCCESS);
}