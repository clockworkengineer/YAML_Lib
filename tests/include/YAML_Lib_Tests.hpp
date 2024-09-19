#pragma once

#include "catch2/catch_all.hpp"

#include "YAML.hpp"
#include "YAML_Core.hpp"

#include <filesystem>
#include <fstream>

// Unit test constants
constexpr char kGeneratedYAMLFile[] = "generated.yaml";
constexpr char kSingleYAMLFile[] = "testfile001.yaml";
constexpr char kNonExistantYAMLFile[] = "doesntexist.yaml";

const std::string prefixPath(const std::string &yamlFileName);

// Test files macro
#define TEST_FILE_LIST(file)                                                   \
  auto file = GENERATE(values<std::string>(                                    \
      {"testfile001.yaml", "testfile002.yaml", "testfile003.yaml",             \
       "testfile004.yaml", "testfile005.yaml", "testfile006.yaml",             \
       "testfile007.yaml", "testfile008.yaml"}))

using namespace YAML_Lib;