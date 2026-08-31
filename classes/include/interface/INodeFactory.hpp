#pragma once

#include <string_view>
#include "implementation/common/YAML_Error.hpp"

namespace YAML_Lib {

struct Node;
class Number;

/**
 * @brief Abstract Factory interface for Node creation.
 *
 * Implements Dependency Inversion Principle (DIP) by allowing parser components
 * to construct Node instances via abstraction rather than depending on concrete Node types.
 */
class INodeFactory {
public:
  YAML_MAKE_ERROR(Error, "INodeFactory Error");
  virtual ~INodeFactory() = default;

  [[nodiscard]] virtual Node createString(std::string_view value) const = 0;
  [[nodiscard]] virtual Node createNumber(const Number &value) const = 0;
  [[nodiscard]] virtual Node createNumber(std::string_view value) const = 0;
  [[nodiscard]] virtual Node createBoolean(bool value, std::string_view representation) const = 0;
  [[nodiscard]] virtual Node createNull() const = 0;
  [[nodiscard]] virtual Node createArray() const = 0;
  [[nodiscard]] virtual Node createDictionary() const = 0;
  [[nodiscard]] virtual Node createDocument() const = 0;
  [[nodiscard]] virtual Node createHole() const = 0;
  [[nodiscard]] virtual Node createTimestamp(std::string_view value) const = 0;
  [[nodiscard]] virtual Node createComment(std::string_view value) const = 0;
};

} // namespace YAML_Lib
