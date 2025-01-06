//
// Program: YAML_Analyze_File
//
// Description: For a each YAML file in a directory parse it, then analyze its
// YAML tree and produce an output report of the analysis.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"
#include "YAML_Analyzer.hpp"

namespace yl = YAML_Lib;

/// <summary>
/// Parse YAML file and analyze its YAML tree.
/// </summary>
/// <param name="fileName">YAML file name</param>
void processYAMLFile(const std::string &fileName)
{
  PLOG_INFO << "Analyzing " << fileName;
  const yl::YAML yaml;
  YAML_Analyzer yamlAnalyzer;
  yaml.parse(yl::FileSource{ fileName });
  yaml.traverse(yamlAnalyzer);
  PLOG_INFO << yamlAnalyzer.dump();
  PLOG_INFO << "Finished " << fileName << ".";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  // Initialise logging.
  plog::init(plog::debug, "YAML_Analyze_File.log");
  PLOG_INFO << "YAML_Analyze_File started ...";
  // Output YAML Lib version and data structure metrics
  PLOG_INFO << yl::YAML().version();
  PLOG_INFO << YAML_Analyzer::dumpNumericSizes();
  PLOG_INFO << YAML_Analyzer::dumpYNodeSizes();
  // Analyze YAML files.
  for (auto &fileName : Utility::createYAMLFileList()) {
    try {
      processYAMLFile(fileName);
    } catch (std::exception &ex) {
      PLOG_ERROR << "Error: " << ex.what();
    }
  }
  PLOG_INFO << "YAML_Analyze_File exited.";
  exit(EXIT_SUCCESS);
}