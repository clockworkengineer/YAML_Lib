#pragma once

namespace YAML_Lib {

struct String {
  // Constructors/Destructors
  String() = default;
  explicit String(const std::string_view &string,
                  const char quotes = kDoubleQuote)
      : yNodeString(std::string(string)), quote(quotes) {}
  String(const String &other) = default;
  String &operator=(const String &other) = default;
  String(String &&other) = default;
  String &operator=(String &&other) = default;
  ~String() = default;
  // Return a string_view into the stored string; const-only since string_view
  // does not allow mutation of the underlying data.
  [[nodiscard]] std::string_view value() const { return yNodeString; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return yNodeString; }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const { return yNodeString; }
  // Return string type/quote of value
  [[nodiscard]] char getQuote() const { return quote; }

private:
  std::string yNodeString;
  char quote{};
};
} // namespace YAML_Lib
