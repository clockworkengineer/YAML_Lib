
//
// Program: YAML_Fibonacci
//
// Description: On each activation add the next entry in the fibonacci
// sequence to the array stored in fibonacci.yaml; if the file does
// not exist then create the initial sequence of [0,1].
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
  const yml::YAML yaml;
  if (!std::filesystem::exists(yamlFibonacciFile())) {
    // If YAML file does not exist create initial sequence
    yaml.parse(yml::BufferSource{"---\n - 1\n - 1\n - 2\n...\n"});
  } else {
    // Parse in current sequence
    yaml.parse(yml::FileSource{yamlFibonacciFile()});

    // if (yml::isA<yml::Document>(yaml.document(0))) {
    //   PLOG_INFO << "Document present...";
    // }
    // if (yml::isA<yml::Array>(yaml.document(0)[0])) {
    //   PLOG_INFO << "Array present...";
    // }
    // REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 3);
    // REQUIRE(YRef<Number>(yaml.document(0)[0][0]).value<long>() == 1);
    // REQUIRE(YRef<Number>(yaml.document(0)[0][1]).value<long>() == 1);
    // REQUIRE(YRef<Number>(yaml.document(0)[0][2]).value<long>() == 2);
    // auto &fibonacciArray = yml::YRef<yml::Array>(yaml.document(0))[0];
    // // Get index of last element
    // const auto last = fibonacciArray.size() - 1;
    // // Get last two in sequence
    // const auto first = yml::YRef<yml::Number>(yaml.root()[last -
    // 1]).value<long>(); const auto second =
    // yml::YRef<yml::Number>(yaml.root()[last]).value<long>();
    // // Create new element for next in sequence
    // fibonacciArray.add(yml::YNode::make<yml::Number>(first+second));
    // yaml.root().addChild(xNode);
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
    PLOG_INFO << yml::YAML().version();
    // Update current sequence
    nextFibonacci();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "YAML_Fibonacci exited.";
  exit(EXIT_SUCCESS);
}