#pragma once

#include <cstdint>

namespace YAML_Lib {
// =======
// Boolean
// =======
struct Boolean {
  // Compile-time table of all twelve valid boolean literal spellings.
  // Indices 0-5 are true-forms; 6-11 are false-forms.
  // Must match the insertion order of isTrue / isFalse below.
  static constexpr const char *kForms[12] = {
      "True", "On",    "Yes",   "true",  "yes",  "on",
      "False","Off",   "No",    "false", "no",   "off"};

  // Constructors/Destructors
  Boolean() = default;
  Boolean(const bool boolean, const std::string_view &value)
      : yNodeBoolean(boolean) {
    for (uint8_t i = 0; i < 12; ++i) {
      if (value == kForms[i]) {
        originalForm = i;
        return;
      }
    }
    originalForm = boolean ? 3u : 9u; // fallback: "true" / "false"
  }
  Boolean(const Boolean &other) = default;
  Boolean &operator=(const Boolean &other) = default;
  Boolean(Boolean &&other) = default;
  Boolean &operator=(Boolean &&other) = default;
  ~Boolean() = default;
  // Return reference boolean value
  [[nodiscard]] bool &value() { return yNodeBoolean; }
  [[nodiscard]] const bool &value() const { return yNodeBoolean; }
  // Return string representation of value (original parsed form)
  [[nodiscard]] std::string toString() const { return kForms[originalForm]; }
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
  uint8_t originalForm{3}; // default index → "true"
};
} // namespace YAML_Lib