#pragma once

#include "IDOMParser.hpp"

namespace YAML_Lib {

// ====================
// Forward declarations
// ====================

class ISource;
struct Node;

// =========================
// Interface for YAML parser
// =========================
/**
 * @brief Interface for YAML parser implementations.
 *
 * Implement this interface to provide custom YAML parsing logic.
 *
 * @note The source object must outlive the call to `parse()`.
 * @note Implementations should return documents in the same order they appear in the input.
 */
class IParser : public IDOMParser {
public:
  /**
   * @brief Exception type for parser errors.
   */
  YAML_MAKE_ERROR(Error, "IParser Error");
  /**
   * @brief Virtual destructor.
   */
  ~IParser() override = default;
};
} // namespace YAML_Lib