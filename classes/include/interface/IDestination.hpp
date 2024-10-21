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
  struct Error final : std::runtime_error {
    explicit Error(const std::string &message)
        : std::runtime_error("IDestination Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IDestination() = default;
  // ========================
  // Add bytes to destination
  // ========================
  virtual void add(const std::string &bytes) = 0;
  // ============================
  // Add character to destination
  // ============================
  virtual void add(char ch) = 0;
  // ============================
  // Clear the current destination
  // ===========================
  virtual void clear() = 0;
  // =================================
  // Return the last character written
  // =================================
  virtual char last()  = 0;
};
} // namespace YAML_Lib