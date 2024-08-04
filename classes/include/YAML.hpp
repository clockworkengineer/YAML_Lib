#pragma once

#include <string>

namespace YAML_Lib {

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