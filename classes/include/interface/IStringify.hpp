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
/**
 * @brief Interface for YAML stringifier implementations.
 *
 * Implement this interface to provide custom YAML stringification logic.
 */
class IStringify {
public:
  /**
   * @brief Exception type for stringifier errors.
   */
  YAML_MAKE_ERROR(Error, "IStringify Error");
  /**
   * @brief Virtual destructor.
   */
  virtual ~IStringify() = default;
  /**
   * @brief Stringify a Node tree to a destination.
   * @param yNode Root node to stringify.
   * @param destination Output destination implementing IDestination.
   * @param indent Indentation level for pretty-printing.
   */
  virtual void stringify(const Node &yNode, IDestination &destination,  unsigned long indent) const = 0;
  /**
   * @brief Get the current print indentation level.
   * @return Indentation level.
   */
  [[nodiscard]] virtual long getIndent() const { return 0; }
  /**
   * @brief Set the print indentation level.
   * @param indent Indentation level.
   */
  virtual void setIndent([[maybe_unused]] long indent)  {}

  /**
   * @brief Throw an error for unknown node types (used by all stringifiers).
   */
  [[noreturn]] static void throwUnknownNodeType() {
    throw IStringify::Error{"Unknown Node type encountered during stringification."};
  }

};
// Make custom stringify to pass to JSON constructor: Pointer is tidied up internally.
template <typename T> IStringify *makeStringify() {
    return std::make_unique<T>().release();
}
} // namespace YAML_Lib
