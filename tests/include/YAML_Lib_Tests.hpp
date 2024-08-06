#pragma once

#include "catch2/catch_all.hpp"

#include "YAML.hpp"
#include "YAML_Core.hpp"

#include <fstream>
#include <filesystem>

std::string prefixPath(const std::string &file);
void checkStringify(const std::string &YAMLString);
void verifyCRLFCount(YAML_Lib::ISource &source, long lfFinal, long crFinal);

using namespace YAML_Lib;