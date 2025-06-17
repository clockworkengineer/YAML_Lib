#pragma once

namespace YAML_Lib {

struct String final : Variant {
  // Constructors/Destructors
  String() : Variant(Type::string) {}
  explicit String(const std::string_view &string, const char quotes = kDoubleQuote)
      : Variant(Type::string), yNodeString(std::move(string)), quote(quotes) {}
  String(const String &other) = default;
  String &operator=(const String &other) = default;
  String(String &&other) = default;
  String &operator=(String &&other) = default;
  ~String() override = default;
  // Return reference to string
  [[nodiscard]] std::string_view value() { return yNodeString; }
  [[nodiscard]] std::string_view value() const { return yNodeString; }
  // Return string representation of value
  [[nodiscard]]  std::string toString() const override {
    return yNodeString;
  }
  // Convert variant to a key
  [[nodiscard]]  std::string toKey() const override { return yNodeString; }
  // Return string type/quote of value
  [[nodiscard]] char getQuote() const { return quote; }

private:
  std::string yNodeString;
  char quote{};
};
} // namespace YAML_Lib
