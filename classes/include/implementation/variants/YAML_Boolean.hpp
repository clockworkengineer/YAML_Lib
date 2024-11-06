#pragma once

namespace YAML_Lib {
// =======
// Boolean
// =======
struct Boolean : Variant
{
  // Constructors/Destructors
  Boolean() : Variant(Type::boolean) {}
  Boolean(const bool boolean, const std::string &value) : Variant(Type::boolean), yNodeBoolean(boolean), booleanString(value) {}
  Boolean(const Boolean &other) = default;
  Boolean &operator=(const Boolean &other) = default;
  Boolean(Boolean &&other) = default;
  Boolean &operator=(Boolean &&other) = default;
  ~Boolean() = default;
  // Return reference boolean value
  [[nodiscard]] bool &value() { return yNodeBoolean; }
  [[nodiscard]] const bool &value() const { return yNodeBoolean; }
  // Return string representation of value
  [[nodiscard]] const std::string &toString() const { return booleanString; }
  // Valid values for true/false
  inline static const std::set<std::string> isTrue{"True", "On", "Yes"};
  inline static const std::set<std::string> isFalse{"False", "Off", "No"};

private:
  bool yNodeBoolean{};
  std::string booleanString;
};
}// namespace YAML_Lib