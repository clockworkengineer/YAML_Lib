#pragma once

namespace YAML_Lib {

struct Hole final : Variant {
  // Constructors/Destructors
  Hole() : Variant(Type::hole) {}
  Hole(const Hole &other) = default;
  Hole &operator=(const Hole &other) = default;
  Hole(Hole &&other) = default;
  Hole &operator=(Hole &&other) = default;
  ~Hole() override = default;
  // Return string representation of value
  [[nodiscard]]  std::string toString() const override { return "null"; }
};
} // namespace YAML_Lib