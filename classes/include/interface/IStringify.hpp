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
class IStringify {
public:
  // ================
  // IStringify Error
  // ================
  struct Error final : std::runtime_error {
    explicit Error(const std::string &message)
        : std::runtime_error("IStringify Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IStringify() = default;
  // ====================
  // Stringify YNode tree
  // ====================
  virtual void stringify(const std::vector<YNode> &yamlTree,
                         IDestination &destination) const = 0;

};
// Make custom stringify to pass to JSON constructor: Note pointer is tidied up internally.
template <typename T> IStringify *makeStringify() {
    return std::make_unique<T>().release();
}
} // namespace YAML_Lib
