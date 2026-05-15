#pragma once

namespace YAML_Lib {

// =======================================================
// Interface for reading source stream during YAML parsing
// =======================================================
/**
 * @brief Interface for reading source stream during YAML parsing.
 *
 * Implement this interface to provide custom input sources for the parser.
 */
class ISource {
public:
  /**
   * @brief Exception type for source errors.
   */
  YAML_MAKE_ERROR(Error, "ISource Error");
  /**
   * @brief Virtual destructor.
   */
  virtual ~ISource() = default;
  /**
   * @brief Get the current character from the source.
   * @return Current character.
   */
  [[nodiscard]] virtual char current() const = 0;
  /**
   * @brief Advance to the next character in the source.
   */
  virtual void next() = 0;
  /**
   * @brief Check if there are more characters to read.
   * @return True if more characters are available.
   */
  [[nodiscard]] virtual bool more() const = 0;
  /**
   * @brief Reset to the beginning of the source stream.
   */
  virtual void reset() = 0;
  /**
   * @brief Get the current character position within the source stream.
   * @return Byte offset position.
   */
  [[nodiscard]] virtual std::size_t position() = 0;
  /**
   * @brief Check if the current character is whitespace.
   * @return True if whitespace.
   */
  [[nodiscard]] bool isWS() const {
    const auto ch = current();
    return ch == ' ' || ch == '\t';
  }
  /**
   * @brief Ignore whitespace on the source stream.
   */
  void ignoreWS() {
    while (more() && isWS()) {
      next();
    }
  }
  /**
   * @brief Check if the current string matches the target at the current position.
   * @param targetString String to match.
   * @return True if matched.
   */
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
  virtual void discardSave() = 0;

protected:
  // ========================
  // Backup length characters
  // ========================
  virtual void backup(unsigned long length) = 0;
  // =========================================================
  // Forbidden-character table (YAML 1.2 §5.1).
  // Forbidden: U+0000-U+0008, U+000B, U+000C, U+000E-U+001F,
  // U+007F.  Allowed controls: TAB(9), LF(10), CR(13).
  // Bytes >= 0x80 are UTF-8 continuation bytes — always allowed.
  // =========================================================
  static constexpr bool kForbiddenChar[256] = {
    // 0x00-0x08: forbidden
    true,true,true,true,true,true,true,true,true,
    // 0x09 TAB: allowed
    false,
    // 0x0A LF: allowed
    false,
    // 0x0B,0x0C: forbidden
    true,true,
    // 0x0D CR: allowed
    false,
    // 0x0E-0x1F: forbidden (18 bytes)
    true,true,true,true,true,true,true,true,true,true,
    true,true,true,true,true,true,true,true,
    // 0x20-0x7E: allowed (95 bytes)
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,
    // 0x7F DEL: forbidden
    true,
    // 0x80-0xFF: UTF-8 multi-byte bytes — allowed (128 bytes)
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false,false,false,
    false,false,false,false,false,false,false,false
  };
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