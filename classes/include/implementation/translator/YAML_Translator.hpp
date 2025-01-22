
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ITranslator.hpp"
#include "YAML_Converter.hpp"

namespace YAML_Lib {

// ========================
// Unicode surrogate ranges
// ========================

constexpr char16_t kHighSurrogatesBegin{0xD800};
constexpr char16_t kHighSurrogatesEnd{0xDBFF};
constexpr char16_t kLowSurrogatesBegin{0xDC00};
constexpr char16_t kLowSurrogatesEnd{0xDFFF};

class YAML_Translator final : public ITranslator {
public:
  // YAML translator error
  struct Error final : std::runtime_error {
    explicit Error(const std::string &message)
        : std::runtime_error("YAML Translator Error: " + message) {}
  };

  YAML_Translator();
  YAML_Translator(const YAML_Translator &other) = delete;
  YAML_Translator &operator=(const YAML_Translator &other) = delete;
  YAML_Translator(YAML_Translator &&other) = delete;
  YAML_Translator &operator=(YAML_Translator &&other) = delete;
  ~YAML_Translator() override = default;

  [[nodiscard]] std::string to(const std::string &rawString) const override;
  [[nodiscard]] std::string
  from([[maybe_unused]] const std::string &escapedString) const override;

private:
  // To/From escape sequence lookup maps
  inline static std::unordered_map<char, char16_t> fromEscape;
  inline static std::unordered_map<char16_t, char> toEscape;
};
} // namespace YAML_Lib
