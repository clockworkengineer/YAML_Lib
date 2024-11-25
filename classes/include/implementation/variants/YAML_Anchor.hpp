#include <utility>

#pragma once

namespace YAML_Lib {
// =======
// Anchor
// =======
struct Anchor : Variant {
  // Constructors/Destructors
  Anchor() : Variant(Type::anchor) {}
  explicit Anchor(std::string name, std::string unparsed, YNode &parsed)
      : Variant(Type::anchor), name(std::move(name)), unparsed(std::move(unparsed)), yNodeAnchor(std::move(parsed))  {}
  Anchor(const Anchor &other)  = default;
  Anchor &operator=(const Anchor &other)  = default;
  Anchor(Anchor &&other) = default;
  Anchor &operator=(Anchor &&other) = default;
  ~Anchor() = default;
  // Return reference boolean value
  [[nodiscard]] YNode &value() { return yNodeAnchor; }
  [[nodiscard]] const YNode &value() const { return yNodeAnchor; }
  // Return string representation of value
  [[nodiscard]] const std::string toString() const override { return unparsed; }
private:
  std::string name;
  std::string unparsed;
  YNode yNodeAnchor{};
};
} // namespace YAML_Lib