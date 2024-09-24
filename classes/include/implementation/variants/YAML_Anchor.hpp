#pragma once

namespace YAML_Lib {
// =======
// Anchor
// =======
struct Anchor : Variant {
  // Constructors/Destructors
  Anchor() : Variant(Type::anchor) {}
  explicit Anchor(const std::string &name, const std::string &unparsed, YNode &parsed)
      : Variant(Type::anchor), name(name), unparsed(unparsed), yNodeAnchor(std::move(parsed))  {}
  Anchor(const Anchor &other) = default;
  Anchor &operator=(const Anchor &other) = default;
  Anchor(Anchor &&other) = default;
  Anchor &operator=(Anchor &&other) = default;
  ~Anchor() = default;
  // Return reference boolean value
  [[nodiscard]] YNode &value() { return yNodeAnchor; }
  [[nodiscard]] const YNode &value() const { return yNodeAnchor; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return unparsed; }

private:
  std::string name;
  std::string unparsed;
  YNode yNodeAnchor{};
};
} // namespace YAML_Lib