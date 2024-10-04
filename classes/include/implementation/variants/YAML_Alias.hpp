#include <utility>

#pragma once

namespace YAML_Lib {
// =======
// Alias
// =======
struct Alias : Variant {
  // Constructors/Destructors
  Alias() : Variant(Type::alias) {}
  explicit Alias(std::string name, YNode &parsed )
      : Variant(Type::alias), name(std::move(name)), yNodeAlias(std::move(parsed))  {}
  Alias(const Alias &other) = delete;
  Alias &operator=(const Alias &other) = delete;
  Alias(Alias &&other) = default;
  Alias &operator=(Alias &&other) = default;
  ~Alias() = default;
  // Return parsed YNode root value
  [[nodiscard]] YNode &value() { return yNodeAlias; }
  [[nodiscard]] const YNode &value() const { return yNodeAlias; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return static_cast<const String &>(yNodeAlias.getVariant()).value();}
  // Return string name of alias
  [[nodiscard]] std::string getName() const { return name; }
private:
  std::string name;
  YNode yNodeAlias;
};
} // namespace YAML_Lib