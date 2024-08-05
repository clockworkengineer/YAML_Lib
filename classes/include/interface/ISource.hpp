#pragma once

namespace YAML_Lib {
// ===================
// Character constants
// ===================
constexpr char kCarriageReturn{ 0x0D };
constexpr char kLineFeed{ 0x0A };
// ==========================================================
// Interface for reading source stream during YAML/DTD_Validator parsing
// ==========================================================
class ISource
{
public:
  // =============
  // ISource Error
  // =============
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("ISource Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~ISource() = default;
  // =================
  // Current character
  // =================
  [[nodiscard]] virtual Char current() const = 0;
  // ======================
  // Move to next character
  // ======================
  virtual void next() = 0;
  // =========================================
  // Are there still more characters to read ?
  // =========================================
  [[nodiscard]] virtual bool more() const = 0;
  // ======================
  // Back length characters
  // ======================
  virtual void backup(long length) = 0;
  // ===============================================
  // Current character position within source stream
  // ===============================================
  [[nodiscard]] virtual long position() const = 0;
  // =========================================================
  // Return range of characters as a string from source stream
  // =========================================================
  virtual std::string getRange(long start, long end) = 0;
  // ===================================
  // Reset to beginning of source stream
  // ===================================
  virtual void reset() = 0;
  // =====================================
  // Is the current character whitespace ?
  // =====================================
  [[nodiscard]] bool isWS() const { return std::iswspace(current()) != 0; }
  // ==================================
  // Ignore whitespace on source stream
  // ==================================
  void ignoreWS()
  {
    while (more() && isWS()) { next(); }
  }
  // =================================================================
  // Is current string a match at the current source stream position ?
  // =================================================================
  bool match(const String &target)
  {
    long index = 0;
    while (more() && current() == target[index]) {
      next();
      if (++index == static_cast<long>(target.length())) { return true; }
    }
    backup(index);
    return false;
  }
  bool match(const char *target)
  {
    long index = 0;
    while (more() && current() == static_cast<Char>(target[index])) {
      next();
      if (++index == static_cast<long>(std::strlen(target))) { return true; }
    }
    backup(index);
    return false;
  }
  // ==================================
  // Get current source stream position
  // ==================================
  std::pair<long, long> getPosition() const { return std::make_pair(lineNo, columnNo); }

protected:
  long lineNo = 1;
  long columnNo = 1;
};
}// namespace YAML_Lib