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
class IParser {
public:
  // =============
  // IParser Error
  // =============
    struct Error final : std::runtime_error {
        explicit Error(const std::string_view &message)
            : std::runtime_error(std::string("IParser Error: ").append(message)) {}
    };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IParser() = default;
  // =================
  // Parse YAML source
  // =================
  virtual std::vector<Node> parse(ISource &source) = 0;
};
} // namespace YAML_Lib