
#pragma once

#include <string>
#include <stdexcept>
#include <array>

namespace YAML_Lib {

// ====================================================
// Interface for translation to/from escaped characters
// ====================================================
class ITranslator
{
public:
  // =============
  // ITranslator Error
  // =============
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("ITranslator Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~ITranslator() = default;
  // =====================================================================
  // Convert any escape sequences in a string to their correct sequence
  // of UTF-8 characters.
  // =====================================================================
  [[nodiscard]] virtual std::string from(const std::string &escapedString) const = 0;
  // =========================================================================
  // Convert a string from raw character values (UTF8) so that it has character
  // escapes where applicable for its form.
  // =========================================================================
  [[nodiscard]] virtual std::string to(const std::string &rawString) const = 0;
};
}// namespace YAML_Lib
