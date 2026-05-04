#pragma once

namespace YAML_Lib {

struct Hole {
  // Constructors/Destructors
  Hole() = default;
  Hole(const Hole &other) = default;
  Hole &operator=(const Hole &other) = default;
  Hole(Hole &&other) = default;
  Hole &operator=(Hole &&other) = default;
  ~Hole() = default;
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return "null"; }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const { return "null"; }
};
} // namespace YAML_Lib