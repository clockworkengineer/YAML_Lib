
#pragma once

#include <string>

namespace YAML_Lib {

struct String : Variant
{
  // Constructors/Destructors
  String() : Variant(Type::string) {}
  explicit String(std::string string) : Variant(Type::string), yNodeString(std::move(string)) {}
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

private:
  std::string yNodeString;
};
}// namespace YAML_Lib
