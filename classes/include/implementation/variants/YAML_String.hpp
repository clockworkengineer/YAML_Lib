
#pragma once

#include <string>

namespace YAML_Lib {

struct String : Variant
{
  // Constructors/Destructors
  String() : Variant(Type::string) {}
  explicit String(std::string string, char quote ='"', unsigned long indent=0) : Variant(Type::string, indent), yNodeString(std::move(string)), quote(quote) {}
  String(const String &other) = default;
  String &operator=(const String &other) = default;
  String(String &&other) = default;
  String &operator=(String &&other) = default;
  ~String() = default;
  // Return reference to string
  [[nodiscard]] std::string &value() { return yNodeString; }
  [[nodiscard]] const std::string &value() const { return yNodeString; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return yNodeString; }
  // Return string type/quote of value
  [[nodiscard]] char getQuote() const { return quote; }

private:
  std::string yNodeString;
  char quote{};
};
}// namespace YAML_Lib
