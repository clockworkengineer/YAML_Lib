#pragma once

namespace YAML_Lib {

// ====================================================================
// Interface for writing destination stream during YAML stringification
// ====================================================================
class IDestination {
public:
  // ==================
  // IDestination Error
  // ==================
  YAML_MAKE_ERROR(Error, "IDestination Error");
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IDestination() = default;
  // ============================
  // Add character to destination
  // ============================
  virtual void add(char ch) = 0;
  // ========================
  // Add bytes to destination
  // (default: delegates to add(char); batch destinations may override)
  // ========================
  virtual void add(const std::string_view &bytes) {
    for (const char ch : bytes) add(ch);
  }
  virtual void add(const std::string &bytes) {
    for (const char ch : bytes) add(ch);
  }
  virtual void add(const char *bytes) {
    while (*bytes) add(*bytes++);
  }
  // ============================
  // Clear the current destination
  // ===========================
  virtual void clear() = 0;
  // =================================
  // Return the last character written
  // =================================
  virtual char last() = 0;
  // ===========================================
  // Reserve capacity (no-op for non-buffer destinations)
  // ===========================================
  virtual void reserve([[maybe_unused]] std::size_t n) {}
};
} // namespace YAML_Lib