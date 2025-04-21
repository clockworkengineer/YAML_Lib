
#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Converter.hpp"

namespace YAML_Lib {

// ========================
// Unicode surrogate ranges
// ========================

constexpr char16_t kHighSurrogatesBegin{0xD800};
constexpr char16_t kHighSurrogatesEnd{0xDBFF};
constexpr char16_t kLowSurrogatesBegin{0xDC00};
constexpr char16_t kLowSurrogatesEnd{0xDFFF};

class Default_Translator final : public ITranslator {
public:
  // YAML translator error
  struct Error final : std::runtime_error {
    explicit Error(const std::string &message)
        : std::runtime_error("YAML Translator Error: " + message) {}
  };

  Default_Translator();
  Default_Translator(const Default_Translator &other) = delete;
  Default_Translator &operator=(const Default_Translator &other) = delete;
  Default_Translator(Default_Translator &&other) = delete;
  Default_Translator &operator=(Default_Translator &&other) = delete;
  ~Default_Translator() override = default;

  [[nodiscard]] std::string to(const std::string &rawString) const override;
  [[nodiscard]] std::string
  from([[maybe_unused]] const std::string &escapedString) const override;

private:
  // To/From escape sequence lookup maps
  inline static std::unordered_map<char, char16_t> fromEscape;
  inline static std::unordered_map<char16_t, char> toEscape;
};
} // namespace YAML_Lib
