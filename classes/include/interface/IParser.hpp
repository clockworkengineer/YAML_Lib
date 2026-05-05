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
  YAML_MAKE_ERROR(Error, "IParser Error");
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