#pragma once

namespace YAML_Lib {

struct Null : Variant {
  // Constructors/Destructors
  Null() : Variant(Type::null) {}
  Null(const Null &other) = default;
  Null &operator=(const Null &other) = default;
  Null(Null &&other) = default;
  Null &operator=(Null &&other) = default;
  ~Null() = default;
  // Return null value
  [[nodiscard]] void *value() { return nullptr; }
  // Return string representation of value
  [[nodiscard]] const std::string toString() const override { return "null"; }
  // Convert variant to a key
  [[nodiscard]] const std::string toKey() const override { return ""; }
};
} // namespace YAML_Lib