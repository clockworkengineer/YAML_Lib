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
       "testfile007.yaml", "testfile008.yaml", "testfile009.yaml",             \
       "testfile010.yaml", "testfile011.yaml", "testfile012.yaml",             \
       "testfile013.yaml", "testfile014.yaml", "testfile015.yaml",             \
       "testfile016.yaml", /*"testfile017.yaml",*/ "testfile018.yaml",             \
       "testfile019.yaml", "testfile020.yaml", "testfile021.yaml",             \
       "testfile022.yaml", "testfile023.yaml", "testfile024.yaml",             \
       "testfile025.yaml", "testfile026.yaml", "testfile027.yaml",             \
       "testfile028.yaml"}))

using namespace YAML_Lib;