#pragma once

namespace YAML_Lib {

// ====================
// Forward declarations
// ====================
class IDestination;
struct YNode;

// ==================================
// Interface for YAML stringification
// ==================================
class IStringify
{
public:
  // ================
  // IStringify Error
  // ================
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("IStringify Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IStringify() = default;
  // ====================
  // Stringify YNode tree
  // ====================
  virtual void stringify(const std::vector<YNode> &yamlTree, IDestination &destination) const = 0;
};
}// namespace YAML_Lib