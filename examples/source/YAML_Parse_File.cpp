//
// Program: YAML_Parse_File
//
// Description: For each YAML file in a directory parse it, stringify
// it back into text form and then parse the buffer created; timing each
// step in turn for each file.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;
namespace chrono = std::chrono;

static constexpr size_t kMaxFileLengthToDisplay = 16 * 1024;

/// <summary>
/// Parse YAML file, stringify and parse YAML from buffer whilst timing each
/// step.
/// </summary>
/// <param name="fileName">YAML file name</param>
void processYAMLFile(const std::string &fileName)
{
  auto elapsedTime = [](const auto &start, const auto &stop) {
    return chrono::duration_cast<chrono::microseconds>(stop - start).count();
  };
  PLOG_INFO << "Processing " << fileName;
  const yl::YAML yaml;
  yl::BufferDestination yamlDestination;
  // Parse from file
  auto start = chrono::high_resolution_clock::now();
  yaml.parse(yl::FileSource{ fileName });
  auto stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to parse from file.";
  // Stringify to file
  start = chrono::high_resolution_clock::now();
  yaml.stringify(yl::FileDestination{ fileName + ".new" });
  stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to stringify to file.";
  // Stringify to buffer
  start = chrono::high_resolution_clock::now();
  yaml.stringify(yamlDestination);
  stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to stringify to buffer.";
  // Parse from buffer
  start = chrono::high_resolution_clock::now();
  yaml.parse(yl::BufferSource{ yamlDestination.toString() });
  stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to parse from buffer.";
  // Display contents
  if (yamlDestination.size() < kMaxFileLengthToDisplay) { PLOG_INFO << "[" << yamlDestination.toString() << "]"; }
  PLOG_INFO << "--------------------FILE PROCESSED OK--------------------";
  PLOG_INFO << "Finished " << fileName << ".";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  // Initialise logging.
  plog::init(plog::debug, "YAML_Parse_Files.log");
  PLOG_INFO << "YAML_Parse_Files started ...";
  PLOG_INFO << YAML_Lib::YAML::version();
  // For each yaml parse it, stringify it and display unless its to large.
  for (auto &fileName : Utility::createYAMLFileList()) {
    try {
      processYAMLFile(fileName);
    } catch (std::exception &ex) {
      PLOG_ERROR << "Error: " << ex.what();
    }
  }
  exit(EXIT_SUCCESS);
}