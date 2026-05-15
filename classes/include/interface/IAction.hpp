
#pragma once

namespace YAML_Lib {

// ====================
// Forward declarations
// ====================
struct Node;

// ==========================================================
// Interface for the action events during YAML tree traversal
// ==========================================================
/**
 * @brief Interface for action events during YAML tree traversal.
 *
 * Implement this interface to visit every node and type during tree traversal.
 */
class IAction {
public:
  /**
   * @brief Exception type for action errors.
   */
  YAML_MAKE_ERROR(Error, "IAction Error");
  /**
   * @brief Virtual destructor.
   */
  virtual ~IAction() = default;
  /**
   * @brief Called when a node is encountered during traversal.
   * @param yNode Node reference.
   */
  virtual void onNode([[maybe_unused]] Node &yNode) {}
  virtual void onNode([[maybe_unused]] const Node &yNode) {}
  /**
   * @brief Called when a string node is encountered.
   * @param yNode Node reference.
   */
  virtual void onString([[maybe_unused]] Node &yNode) {}
  virtual void onString([[maybe_unused]] const Node &yNode) {}
  /**
   * @brief Called when a number node is encountered.
   * @param yNode Node reference.
   */
  virtual void onNumber([[maybe_unused]] Node &yNode) {}
  virtual void onNumber([[maybe_unused]] const Node &yNode) {}
  /**
   * @brief Called when a boolean node is encountered.
   * @param yNode Node reference.
   */
  virtual void onBoolean([[maybe_unused]] Node &yNode) {}
  virtual void onBoolean([[maybe_unused]] const Node &yNode) {}
  /**
   * @brief Called when a null node is encountered.
   * @param yNode Node reference.
   */
  virtual void onNull([[maybe_unused]] Node &yNode) {}
  virtual void onNull([[maybe_unused]] const Node &yNode) {}
  /**
   * @brief Called when an array node is encountered.
   * @param yNode Node reference.
   */
  virtual void onArray([[maybe_unused]] Node &yNode) {}
  virtual void onArray([[maybe_unused]] const Node &yNode) {}
  /**
   * @brief Called when a dictionary node is encountered.
   * @param yNode Node reference.
   */
  virtual void onDictionary([[maybe_unused]] Node &yNode) {}
  virtual void onDictionary([[maybe_unused]] const Node &yNode) {}
};
} // namespace YAML_Lib
