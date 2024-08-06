#pragma once

#include <algorithm>
#include <cstring>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace YAML_Lib {

// ====================
// Internal String Type
// ====================
using String = std::u16string;
using Char = String::value_type;

class YAML {
public:
  YAML() = default;
  YAML(const YAML &other) = delete;
  YAML &operator=(const YAML &other) = delete;
  YAML(YAML &&other) = delete;
  YAML &operator=(YAML &&other) = delete;
  ~YAML() = default;

  static std::string fromFile(const std::string &yamlFileName) {
    std::ifstream yamlFile{yamlFileName, std::ios_base::binary};
    std::ostringstream yamlFileBuffer;
    yamlFileBuffer << yamlFile.rdbuf();
    return yamlFileBuffer.str();
  }
};

} // namespace YAML_Lib