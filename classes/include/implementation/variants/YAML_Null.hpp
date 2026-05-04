#pragma once

namespace YAML_Lib {

struct Null {
  // Constructors/Destructors
  Null() = default;
  Null(const Null &other) = default;
  Null &operator=(const Null &other) = default;
  Null(Null &&other) = default;
  Null &operator=(Null &&other) = default;
  ~Null() = default;
  // Return null value
  [[nodiscard]] static void *value() { return nullptr; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return "null"; }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const { return ""; }
};
} // namespace YAML_Lib