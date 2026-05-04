#pragma once

namespace YAML_Lib {
// =======
// Boolean
// =======
struct Boolean {
  // Constructors/Destructors
  Boolean() = default;
  Boolean(const bool boolean, const std::string_view &value)
      : yNodeBoolean(boolean), booleanString(std::string(value)) {}
  Boolean(const Boolean &other) = default;
  Boolean &operator=(const Boolean &other) = default;
  Boolean(Boolean &&other) = default;
  Boolean &operator=(Boolean &&other) = default;
  ~Boolean() = default;
  // Return reference boolean value
  [[nodiscard]] bool &value() { return yNodeBoolean; }
  [[nodiscard]] const bool &value() const { return yNodeBoolean; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return booleanString; }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const {
    return yNodeBoolean ? "true" : "false";
  }
  // Valid values for true/false
  // YAML 1.1 capitalised forms and YAML 1.2 lowercase canonical forms.
  inline static const std::set<std::string_view> isTrue{"True", "On", "Yes",
                                                        "true", "yes", "on"};
  inline static const std::set<std::string_view> isFalse{"False", "Off", "No",
                                                         "false", "no", "off"};

private:
  bool yNodeBoolean{};
  std::string booleanString;
};
} // namespace YAML_Lib