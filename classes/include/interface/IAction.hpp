
#pragma once

namespace YAML_Lib {

// ====================
// Forward declarations
// ====================
struct YNode;

// ==========================================================
// Interface for the action events during YAML tree traversal
// ==========================================================
class IAction
{
public:
  // =============
  // IAction Error
  // =============
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("IAction Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IAction() = default;
  // ============================
  // YNode encountered so process
  // ============================
  virtual void onYNode([[maybe_unused]] YNode &yNode) { }
  virtual void onYNode([[maybe_unused]] const YNode &yNode) { }
  // =============================
  // String encountered so process
  // =============================
  virtual void onString([[maybe_unused]] YNode &yNode) { }
  virtual void onString([[maybe_unused]] const YNode &yNode) { }
  // =============================
  // Number encountered so process
  // =============================
  virtual void onNumber([[maybe_unused]] YNode &yNode) { }
  virtual void onNumber([[maybe_unused]] const YNode &yNode) { }
  // ==============================
  // Boolean encountered so process
  // ==============================
  virtual void onBoolean([[maybe_unused]] YNode &yNode) { }
  virtual void onBoolean([[maybe_unused]] const YNode &yNode) { }
  // ===========================
  // Null encountered so process
  // ===========================
  virtual void onNull([[maybe_unused]] YNode &yNode) { }
  virtual void onNull([[maybe_unused]] const YNode &yNode) { }
  // ============================
  // Array encountered so process
  // ============================
  virtual void onArray([[maybe_unused]] YNode &yNode) { }
  virtual void onArray([[maybe_unused]] const YNode &yNode) { }
  // =============================
  // Dictionary encountered so process
  // =============================
  virtual void onDictionary([[maybe_unused]] YNode &yNode) { }
  virtual void onDictionary([[maybe_unused]] const YNode &yNode) { }
};
}// namespace YAML_Lib
