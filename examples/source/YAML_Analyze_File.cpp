//
// Program: YAML_Analyze_File
//
// Description: For a each YAML file in a directory parse it, then analyze its
// YAML tree and produce an output report of the analysis.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Analyzer.hpp"
#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;

/// <summary>
/// Parse YAML file and analyze its YAML tree.
/// </summary>
/// <param name="fileName">YAML file name</param>
void processYAMLFile(const std::string &fileName) {
  std::cout << "Analyzing " << fileName;
  const yl::YAML yaml;
  YAML_Analyzer yamlAnalyzer;
  yaml.parse(yl::FileSource{fileName});
  yaml.traverse(yamlAnalyzer);
  std::cout << yamlAnalyzer.dump();
  std::cout << "Finished " << fileName << ".";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  // Initialise logging.
    std::cout << "YAML_Analyze_File started ...";
  // Output YAML Lib version and data structure metrics
  std::cout << YAML_Lib::YAML::version();
  std::cout << YAML_Analyzer::dumpNumericSizes();
  std::cout << YAML_Analyzer::dumpNodeSizes();
  // Analyze YAML files.
  for (auto &fileName : Utility::createYAMLFileList()) {
    try {
      processYAMLFile(fileName);
    } catch (std::exception &ex) {
      std::cerr << "Error: " << ex.what();
    }
  }
  std::cout << "YAML_Analyze_File exited.";
  exit(EXIT_SUCCESS);
}