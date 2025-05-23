#pragma once

namespace YAML_Lib {

// =======================================================
// Interface for reading source stream during YAML parsing
// =======================================================
class ISource {
public:
  // =============
  // ISource Error
  // =============
  struct Error final : std::runtime_error {
    explicit Error(const std::string_view &message)
        : std::runtime_error(std::string("ISource Error: ").append(message)) {}
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
  [[nodiscard]] virtual std::size_t position() = 0;
  // ===================================
  // Is the current character whitespace
  // ===================================
  [[nodiscard]] bool isWS() const {
    const auto ch = current();
    return ch == ' ' || ch == '\t';
  }
  // ==================================
  // Ignore whitespace on source stream
  // ==================================
  void ignoreWS() {
    while (more() && isWS()) {
      next();
    }
  }
  // ===============================================================
  // Is current string a match at the current source stream position
  // ===============================================================
  [[nodiscard]] bool match(const std::string_view &targetString) {
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
  // =====================================
  // Return current character and move one
  // =====================================
  char append() {
    const auto ch = current();
    if (more()) {
      next();
    }
    return ch;
  }
  // ==================================
  // Get current source stream position
  // ==================================
  [[nodiscard]] std::pair<unsigned long, unsigned long> getPosition() const {
    return std::make_pair(lineNo, column);
  }
  // ====================
  // Save/Restore context
  // ====================
  virtual void save() = 0;
  virtual void restore() = 0;

protected:
  // ========================
  // Backup length characters
  // ========================
  virtual void backup(unsigned long length) = 0;
  // ========================================
  // Current line and column on source stream
  // ========================================
  unsigned long lineNo = 1;
  long column = 1;
  std::size_t bufferPosition{};
  // =============
  // Saved context
  // =============
  struct Context {
    Context(const unsigned long line, const unsigned long col, const std::size_t pos)
        : lineNo(line), column(col), bufferPosition(pos) {}
    unsigned long lineNo{};
    unsigned long column{};
    std::size_t bufferPosition{};
  };
  std::vector<Context> contexts;
};
} // namespace YAML_Lib