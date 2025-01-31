
#pragma once

#include <string>

namespace YAML_Lib {

struct String final : Variant {
  // Constructors/Destructors
  String() : Variant(Type::string) {}
  explicit String(std::string string, const char quotes = kDoubleQuote)
      : Variant(Type::string), yNodeString(std::move(string)), quote(quotes) {}
  String(const String &other) = default;
  String &operator=(const String &other) = default;
  String(String &&other) = default;
  String &operator=(String &&other) = default;
  ~String() override = default;
  // Return reference to string
  [[nodiscard]] std::string &value() { return yNodeString; }
  [[nodiscard]] const std::string &value() const { return yNodeString; }
  // Return string representation of value
  [[nodiscard]] const std::string toString() const override {
    return yNodeString;
  }
  // Convert variant to a key
  [[nodiscard]] const std::string toKey() const override { return yNodeString; }
  // Return string type/quote of value
  [[nodiscard]] char getQuote() const { return quote; }

private:
  std::string yNodeString;
  char quote{};
};
} // namespace YAML_Lib
