#pragma once

namespace YAML_Lib {

// ====================
// Forward declarations
// ====================

class ISource;
struct YNode;

// =========================
// Interface for YAML parser
// =========================
class IParser {
public:
  // =============
  // IParser Error
  // =============
  struct Error final : std::runtime_error {
    explicit Error(const std::string &message)
        : std::runtime_error("IParser Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IParser() = default;
  // =================
  // Parse YAML source
  // =================
  virtual std::vector<YNode> parse(ISource &source) = 0;
};
} // namespace YAML_Lib