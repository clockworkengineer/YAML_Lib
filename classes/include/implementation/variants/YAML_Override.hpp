#include <utility>

#pragma once

namespace YAML_Lib {
// =======
// Override
// =======
struct Override : Variant {
  // Constructors/Destructors
  Override() : Variant(Type::override) {}
  explicit Override(std::string name, YNode &parsed )
      : Variant(Type::override), name(std::move(name)), yNodeOverride(std::move(parsed))  {}
  Override(const Override &other)  = delete;
  Override &operator=(const Override &other)  = delete;
  Override(Override &&other) = default;
  Override &operator=(Override &&other) = default;
  ~Override() = default;
  // Return parsed YNode root value
  [[nodiscard]] YNode &value() { return yNodeOverride; }
  [[nodiscard]] const YNode &value() const { return yNodeOverride; }
  // Return string representation of value
  [[nodiscard]] const std::string toString() const override { return static_cast<const String &>(yNodeOverride.getVariant()).value();}
  // Return string name of alias
  [[nodiscard]] std::string getName() const { return name; }
private:
  std::string name;
  YNode yNodeOverride;
};
} // namespace YAML_Lib