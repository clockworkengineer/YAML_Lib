#pragma once

namespace YAML_Lib {

// ========================
// YAML character constants
// ========================
constexpr char kCarriageReturn{0x0D};
constexpr char kLineFeed{0x0A};
constexpr char kSpace{0x20};

// =======================================================
// Interface for reading source stream during YAML parsing
// =======================================================
class ISource {
public:
  // =============
  // ISource Error
  // =============
  struct Error final : std::runtime_error {
    explicit Error(const std::string &message)
        : std::runtime_error("ISource Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~ISource() = default;
  // =================
  // Current character
  // =================
  [[nodiscard]] virtual char current() const = 0;
  // ======================
  // Move to next character
  // ======================
  virtual void next() = 0;
  // ========================
  // Backup length characters
  // ========================
  virtual void backup(unsigned long length) = 0;
  // =======================================
  // Are there still more characters to read
  // ========================================
  [[nodiscard]] virtual bool more() const = 0;
  // ===================================
  // Reset to beginning of source stream
  // ===================================
  virtual void reset() = 0;
  // ===============================================
  // Current character position within source stream
  // ===============================================
  [[nodiscard]] virtual std::size_t position() const = 0;
  // ===================================
  // Is the current character whitespace
  // ===================================
  [[nodiscard]] bool isWS() const {
    auto ch = current();
    return ch == kSpace || ch == '\t';
  }
  // ==================================
  // Ignore whitespace on source stream
  // ==================================
  void ignoreWS() {
    while (more() && isWS()) {
      next();
    }
  }
  // ======================
  // Get source indentation
  // ======================
  [[nodiscard]] unsigned long getIndentation() { return column; }
  // ===============================================================
  // Is current string a match at the current source stream position
  // ===============================================================
  [[nodiscard]] bool match(const std::string &targetString) {
    long index = 0;
    while (more() && current() == targetString[index]) {
      next();
      if (++index == static_cast<long>(targetString.length())) {
        return true;
      }
    }
    backup(index);
    return false;
  }
  bool match(const char *target) {
    long index = 0;
    while (more() && current() == static_cast<char>(target[index])) {
      next();
      if (++index == static_cast<long>(std::strlen(target))) {
        return true;
      }
    }
    backup(index);
    return false;
  }
  // ==================================
  // Get current source stream position
  // ==================================
  [[nodiscard]] std::pair<long, long> getPosition() const {
    return std::make_pair(lineNo, column);
  }

protected:
  // ========================================
  // Current line and column on source stream
  // ========================================
  long lineNo = 1;
  long column = 1;
};
} // namespace YAML_Lib