
#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class YAML_Impl
{
public:
  // Constructors/Destructors
  YAML_Impl() = default;
  YAML_Impl(const YAML_Impl &other) = delete;
  YAML_Impl &operator=(const YAML_Impl &other) = delete;
  YAML_Impl(YAML_Impl &&other) = delete;
  YAML_Impl &operator=(YAML_Impl &&other) = delete;
  ~YAML_Impl() = default;
  // Get YAML_Lib version
  static std::string version();
  // Parse YAML into JNode tree
  void parse(ISource &source);
  // Create YAML text string (no white space) from JNode tree
  void stringify(IDestination &destination) const;
};
}// namespace YAML_Lib
