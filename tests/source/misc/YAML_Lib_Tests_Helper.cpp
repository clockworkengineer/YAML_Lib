
#include "YAML_Lib_Tests.hpp"

/// <summary>
/// Prefix path to test data file name.
/// </summary>
/// <param name="yamlFileName">Test YAML data file name</param>
/// <returns>Full path to a test data file</returns>
std::string prefixPath(const std::string &yamlFileName) {
  return (std::filesystem::current_path() / "files" / yamlFileName).string();
}

/// <summary>
/// Compare the current stringed object.
/// </summary>
/// <param name="yaml"></param>
/// <param name="destinationYAML">YAML parser instance</param>
/// <returns>Full path to a test data file</returns>
void compareYAML(const YAML &yaml,
                 const std::string &destinationYAML) {
  BufferDestination destination;
  REQUIRE_NOTHROW(yaml.stringify(destination));
  REQUIRE(destination.toString() == destinationYAML);
}
/// <summary>
/// Compare string against the contents of a file.
/// </summary>
/// <param name="str">String to compare.</param>
/// <param name="fileName">File to compare against string</param>
/// <returns>true if file the same as str, false otherwise</returns>
bool compareFile(const std::string &str, const std::string &fileName) {
  std::stringstream fileContents;
  std::ifstream file(fileName, std::ifstream::binary | std::ifstream::ate);
  if (file.fail()) {
    return false; // file problem
  }
  file.seekg(0, std::ifstream::beg);
  fileContents << file.rdbuf();
  if (fileContents.str().size() != str.size()) {
    return false;
  }
  return fileContents.str() == str;
}
