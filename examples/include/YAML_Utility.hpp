#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <stdexcept>
#include <chrono>

#include "plog/Initializers/RollingFileInitializer.h"
#include "plog/Log.h"

#include "YAML.hpp"
#include "YAML_Core.hpp"

class Utility
{
public:
  static std::vector<std::string> createYAMLFileList()
  {
    std::vector<std::string> fileList;
    for (auto &file : std::filesystem::directory_iterator((std::filesystem::current_path() / "files"))) {
      if (file.path().extension() == ".yaml") { fileList.push_back(file.path().string()); }
    }
    return (fileList);
  }
};