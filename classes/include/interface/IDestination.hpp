#pragma once

namespace YAML_Lib {

// ====================================================================
// Interface for writing destination stream during YAML stringification
// ====================================================================
/**
 * @brief Interface for writing destination stream during YAML stringification.
 *
 * Implement this interface to provide custom output destinations for the stringifier.
 */
class IDestination {
public:
  /**
   * @brief Exception type for destination errors.
   */
  YAML_MAKE_ERROR(Error, "IDestination Error");
  /**
   * @brief Virtual destructor.
   */
  virtual ~IDestination() = default;
  /**
   * @brief Add a character to the destination.
   * @param ch Character to add.
   */
  virtual void add(char ch) = 0;
  /**
   * @brief Add bytes to the destination (default: delegates to add(char)).
   * @param bytes String view to add.
   */
  virtual void add(const std::string_view &bytes) {
    for (const char ch : bytes) add(ch);
  }
  /**
   * @brief Add bytes to the destination (default: delegates to add(char)).
   * @param bytes String to add.
   */
  virtual void add(const std::string &bytes) {
    for (const char ch : bytes) add(ch);
  }
  /**
   * @brief Add bytes to the destination (default: delegates to add(char)).
   * @param bytes C-string to add.
   */
  virtual void add(const char *bytes) {
    while (*bytes) add(*bytes++);
  }
  /**
   * @brief Clear the current destination.
   */
  virtual void clear() = 0;
  /**
   * @brief Return the last character written.
   * @return Last character written.
   */
  virtual char last() = 0;
  /**
   * @brief Reserve capacity (no-op for non-buffer destinations).
   * @param n Number of bytes to reserve.
   */
  virtual void reserve([[maybe_unused]] std::size_t n) {}
};
} // namespace YAML_Lib