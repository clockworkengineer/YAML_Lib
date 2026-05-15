#pragma once

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
 */
class IParser {
public:
  /**
   * @brief Exception type for parser errors.
   */
  YAML_MAKE_ERROR(Error, "IParser Error");
  /**
   * @brief Virtual destructor.
   */
  virtual ~IParser() = default;
  /**
   * @brief Parse a YAML source stream into a vector of Node objects.
   * @param source Input source implementing ISource.
   * @return Vector of parsed Node objects (one per document).
   */
  virtual std::vector<Node> parse(ISource &source) = 0;
};
} // namespace YAML_Lib