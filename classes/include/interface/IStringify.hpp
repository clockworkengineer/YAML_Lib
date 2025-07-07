#pragma once

namespace YAML_Lib {

// ====================
// Forward declarations
// ====================
class IDestination;
struct Node;

// ==================================
// Interface for YAML stringification
// ==================================
class IStringify {
public:
  // ================
  // IStringify Error
  // ================
    struct Error final : std::runtime_error {
        explicit Error(const std::string_view &message)
            : std::runtime_error(std::string("IStringify Error: ").append(message)) {}
    };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IStringify() = default;
    // ====================
    // Stringify Node tree
    // ====================
    virtual void stringify(const Node &yNode, IDestination &destination,  unsigned long indent) const = 0;
    // =========================
    // Set/Get print indentation
    // =========================
    [[nodiscard]] virtual long getIndent() const { return 0; }
    virtual void setIndent([[maybe_unused]] long indent)  {}

};
// Make custom stringify to pass to JSON constructor: Pointer is tidied up internally.
template <typename T> IStringify *makeStringify() {
    return std::make_unique<T>().release();
}
} // namespace YAML_Lib
