#pragma once

#include "catch2/catch_all.hpp"

#include "YAML.hpp"
#include "YAML_Core.hpp"

#include <fstream>
#include <filesystem>

// Unit test constants
constexpr char kGeneratedYAMLFile[] = "generated.yaml";
constexpr char kSingleYAMLFile[] = "testfile001.yaml";
constexpr char kNonExistantYAMLFile[] = "doesntexist.yaml";

const std::string prefixPath(const std::string &yamlFileName);

using namespace YAML_Lib;