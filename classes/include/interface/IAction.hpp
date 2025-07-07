
#pragma once

namespace YAML_Lib {

// ====================
// Forward declarations
// ====================
struct Node;

// ==========================================================
// Interface for the action events during YAML tree traversal
// ==========================================================
class IAction {
public:
  // =============
  // IAction Error
  // =============
  struct Error final : std::runtime_error {
    explicit Error(const std::string_view &message)
        : std::runtime_error(std::string("IAction Error: ").append(message)) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IAction() = default;
  // ============================
  // Node encountered so process
  // ============================
  virtual void onNode([[maybe_unused]] Node &yNode) {}
  virtual void onNode([[maybe_unused]] const Node &yNode) {}
  // =============================
  // String encountered so process
  // =============================
  virtual void onString([[maybe_unused]] Node &yNode) {}
  virtual void onString([[maybe_unused]] const Node &yNode) {}
  // =============================
  // Number encountered so process
  // =============================
  virtual void onNumber([[maybe_unused]] Node &yNode) {}
  virtual void onNumber([[maybe_unused]] const Node &yNode) {}
  // ==============================
  // Boolean encountered so process
  // ==============================
  virtual void onBoolean([[maybe_unused]] Node &yNode) {}
  virtual void onBoolean([[maybe_unused]] const Node &yNode) {}
  // ===========================
  // Null encountered so process
  // ===========================
  virtual void onNull([[maybe_unused]] Node &yNode) {}
  virtual void onNull([[maybe_unused]] const Node &yNode) {}
  // ============================
  // Array encountered so process
  // ============================
  virtual void onArray([[maybe_unused]] Node &yNode) {}
  virtual void onArray([[maybe_unused]] const Node &yNode) {}
  // =============================
  // Dictionary encountered so process
  // =============================
  virtual void onDictionary([[maybe_unused]] Node &yNode) {}
  virtual void onDictionary([[maybe_unused]] const Node &yNode) {}
};
} // namespace YAML_Lib
