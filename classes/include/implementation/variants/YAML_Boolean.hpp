#pragma once

namespace YAML_Lib {
// =======
// Boolean
// =======
struct Boolean : Variant
{
  // Constructors/Destructors
  Boolean() : Variant(Type::boolean) {}
  explicit Boolean(const bool boolean) : Variant(Type::boolean), yNodeBoolean(boolean) {}
  Boolean(const Boolean &other) = default;
  Boolean &operator=(const Boolean &other) = default;
  Boolean(Boolean &&other) = default;
  Boolean &operator=(Boolean &&other) = default;
  ~Boolean() = default;
  // Return reference boolean value
  [[nodiscard]] bool &value() { return yNodeBoolean; }
  [[nodiscard]] const bool &value() const { return yNodeBoolean; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return yNodeBoolean ? "True" : "False"; }

private:
  bool yNodeBoolean{};
};
}// namespace YAML_Lib