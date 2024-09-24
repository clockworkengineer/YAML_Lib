#pragma once

namespace YAML_Lib {
// =======
// Alias
// =======
struct Alias : Variant {
  // Constructors/Destructors
  Alias() : Variant(Type::alias) {}
  explicit Alias(const std::string &name)
      : Variant(Type::alias), name(name)  {}
  Alias(const Alias &other) = default;
  Alias &operator=(const Alias &other) = default;
  Alias(Alias &&other) = default;
  Alias &operator=(Alias &&other) = default;
  ~Alias() = default;
  // Return reference boolean value
  [[nodiscard]] std::string &value() { return name; }
  [[nodiscard]] const std::string &value() const { return name; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return ""; }

private:
  std::string name;
};
} // namespace YAML_Lib