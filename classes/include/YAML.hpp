#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <filesystem>
#include <set>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <cwctype>
#include <cstring>

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
};

} // namespace YAML_Lib