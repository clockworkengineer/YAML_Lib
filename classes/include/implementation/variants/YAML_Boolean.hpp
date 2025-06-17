#pragma once

namespace YAML_Lib {
// =======
// Boolean
// =======
struct Boolean final : Variant {
  // Constructors/Destructors
  Boolean() : Variant(Type::boolean) {}
  Boolean(const bool boolean, const std::string_view & value)
      : Variant(Type::boolean), yNodeBoolean(boolean),
        booleanString(std::move(value)) {}
  Boolean(const Boolean &other) = default;
  Boolean &operator=(const Boolean &other) = default;
  Boolean(Boolean &&other) = default;
  Boolean &operator=(Boolean &&other) = default;
  ~Boolean() override = default;
  // Return reference boolean value
  [[nodiscard]] bool &value() { return yNodeBoolean; }
  [[nodiscard]] const bool &value() const { return yNodeBoolean; }
  // Return string representation of value
  [[nodiscard]]  std::string toString() const override {
    return booleanString;
  }
  // Convert variant to a key
  [[nodiscard]]  std::string toKey() const override {
    return yNodeBoolean ? "true" : "false";
  }
  // Valid values for true/false
  inline static const std::set<std::string_view> isTrue{"True", "On", "Yes"};
  inline static const std::set<std::string_view> isFalse{"False", "Off", "No"};

private:
  bool yNodeBoolean{};
  std::string booleanString;
};
} // namespace YAML_Lib