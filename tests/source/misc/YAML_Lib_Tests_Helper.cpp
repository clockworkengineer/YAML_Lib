
#include "YAML_Lib_Tests.hpp"

/// <summary>
/// Prefix path to test data file name.
/// </summary>
/// <param name="yamlFileName">Test YAML data file name</param>
/// <returns>Full path to a test data file</returns>
std::string prefixTestDataPath(const std::string &yamlFileName) {
  if (std::filesystem::is_directory("./files")) {
    return (std::filesystem::current_path() / "./files" / yamlFileName)
        .string();
  } else {
    return (std::filesystem::current_path() / "../files" / yamlFileName)
        .string();
  }
}

/// <summary>
/// Compare the current stringed object.
/// </summary>
/// <param name="yaml"></param>
/// <param name="destinationYAML">YAML parser instance</param>
/// <returns>Full path to a test data file</returns>
void compareYAML(const YAML &yaml, const std::string &destinationYAML) {
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
/// <summary>
/// Generate unique file name.
/// </summary>
/// <returns>Unique torrent file name</returns>
std::string generateRandomFileName(void) {
  std::string name1 = std::tmpnam(nullptr);
  return name1;
}
std::string generateEscapes(const unsigned char first,
                            const unsigned char last) {
  std::string result;
  for (char16_t utf16Char = first; utf16Char < last + 1; utf16Char++) {
    const auto digits = "0123456789ABCDEF";
    result += "\\u";
    result += digits[utf16Char >> 12 & 0x0f];
    result += digits[utf16Char >> 8 & 0x0f];
    result += digits[utf16Char >> 4 & 0x0f];
    result += digits[utf16Char & 0x0f];
  }
  return result;
}
