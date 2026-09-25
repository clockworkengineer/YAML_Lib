#pragma once

#include <iostream>

#include "YAML.hpp"
#include "YAML_Core.hpp"

class Utility {
 public:
  static std::vector<std::string> createYAMLFileList() {
    std::vector<std::string> fileList;
    std::filesystem::path dir = "files";
    if (!std::filesystem::exists(dir)) {
      if (std::filesystem::exists("examples/files")) {
        dir = "examples/files";
      } else if (std::filesystem::exists("../examples/files")) {
        dir = "../examples/files";
      }
    }
    if (std::filesystem::exists(dir)) {
      for (const auto& file : std::filesystem::directory_iterator(dir)) {
        if (file.path().extension() == ".yaml") {
          fileList.push_back(file.path().string());
        }
      }
    }
    return fileList;
  }
  static std::string createFileName(const std::string& torrentFileName,
                                    const std::string newExtension) {
    std::string newFileName = torrentFileName;
    return (newFileName.erase(newFileName.find(".yaml"), newFileName.length()) + newExtension);
  }
};