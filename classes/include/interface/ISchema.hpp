#pragma once

#include <string_view>

namespace YAML_Lib {

struct Node;

/**
 * @brief Interface for YAML tag and scalar schema resolution.
 *
 * Provides extensible strategy hooks for coercing untyped YAML scalars
 * and tag handles into typed Node representations (e.g., Core Schema, JSON Schema).
 */
class ISchema {
public:
  virtual ~ISchema() = default;

  /**
   * @brief Exception type for schema errors.
   */
  YAML_MAKE_ERROR(Error, "ISchema Error");

  /**
   * @brief Resolve an unquoted or quoted scalar string to a typed Node.
   * @param scalarText Raw text content of the scalar.
   * @param isQuoted True if the scalar was single- or double-quoted.
   * @return Typed Node (e.g. String, Number, Boolean, Null, Timestamp).
   */
  [[nodiscard]] virtual Node resolveScalar(std::string_view scalarText, bool isQuoted) const = 0;

  /**
   * @brief Resolve explicit tag handle and suffix to a typed Node.
   * @param tagHandle Directive-resolved tag prefix/handle (e.g. "tag:yaml.org,2002:").
   * @param tagSuffix Tag suffix (e.g. "str", "int", "map").
   * @param node Un-tagged base node.
   * @return Tag-resolved Node.
   */
  [[nodiscard]] virtual Node resolveTag(std::string_view tagHandle, std::string_view tagSuffix, const Node &node) const = 0;
};

} // namespace YAML_Lib
