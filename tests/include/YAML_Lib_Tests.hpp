#pragma once

#include "catch2/catch_all.hpp"

#include "YAML.hpp"
#include "YAML_Core.hpp"

// Unit test constants
constexpr char kSingleSmallYAMLFile[] = "testfile000.yaml";
constexpr char kSingleYAMLFile[] = "testfile001.yaml";
constexpr char kNonExistantYAMLFile[] = "doesntexist.yaml";

std::string prefixTestDataPath(const std::string &yamlFileName);
void compareYAML(const YAML_Lib::YAML &yaml,
                 const std::string &destinationYAML);
bool compareFile(const std::string &str, const std::string &fileName);
std::string generateEscapes(unsigned char first, unsigned char last);
std::string generateRandomFileName(void );

// Test files macro
#define TEST_FILE_LIST(file)                                                   \
  auto file = GENERATE(values<std::string>(                                    \
      {"testfile001.yaml", "testfile002.yaml", "testfile003.yaml",             \
       "testfile004.yaml", "testfile005.yaml", "testfile006.yaml",             \
       "testfile007.yaml", "testfile008.yaml", "testfile009.yaml",             \
       "testfile010.yaml", "testfile011.yaml", "testfile012.yaml",             \
       "testfile013.yaml", "testfile014.yaml", "testfile015.yaml",             \
       "testfile016.yaml", "testfile017.yaml", "testfile018.yaml",             \
       "testfile019.yaml", "testfile020.yaml", "testfile021.yaml",             \
       "testfile022.yaml", "testfile023.yaml", "testfile024.yaml",             \
       "testfile025.yaml", "testfile026.yaml", "testfile027.yaml",             \
       "testfile028.yaml", "testfile029.yaml", "testfile030.yaml",             \
       "testfile031.yaml"}))
// Floating point comparison (accurate to within an epsilon)
template <typename T> bool equalFloatingPoint(T a, T b, double epsilon) {
  return (std::fabs(a - b) <= epsilon);
}
using namespace YAML_Lib;