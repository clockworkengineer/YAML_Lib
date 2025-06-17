#pragma once

namespace YAML_Lib {

struct Null final : Variant {
  // Constructors/Destructors
  Null() : Variant(Type::null) {}
  Null(const Null &other) = default;
  Null &operator=(const Null &other) = default;
  Null(Null &&other) = default;
  Null &operator=(Null &&other) = default;
  ~Null() override = default;
  // Return null value
  [[nodiscard]] static void *value() { return nullptr; }
  // Return string representation of value
  [[nodiscard]]  std::string toString() const override { return "null"; }
  // Convert variant to a key
  [[nodiscard]]  std::string toKey() const override { return ""; }
};
} // namespace YAML_Lib