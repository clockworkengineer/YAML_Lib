#pragma once

namespace YAML_Lib {
// =======
// Alias
// =======
struct Alias : Variant {
  // Constructors/Destructors
  Alias() : Variant(Type::anchor) {}
  explicit Alias(const std::string &name, const std::string &unparsed, YNode &parsed)
      : Variant(Type::anchor), name(name), unparsed(unparsed), yNodeAlias(std::move(parsed))  {}
  Alias(const Alias &other) = default;
  Alias &operator=(const Alias &other) = default;
  Alias(Alias &&other) = default;
  Alias &operator=(Alias &&other) = default;
  ~Alias() = default;
  // Return reference boolean value
  [[nodiscard]] YNode &value() { return yNodeAlias; }
  [[nodiscard]] const YNode &value() const { return yNodeAlias; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return unparsed; }

private:
  std::string name;
  std::string unparsed;
  YNode yNodeAlias{};
};
} // namespace YAML_Lib