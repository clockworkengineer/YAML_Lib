#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class XML_Translator final : public ITranslator
{
public:
  // XML translator error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("XML Translator Error: ").append(message)) {}
  };

  XML_Translator() = default;
  XML_Translator(const XML_Translator &other) = delete;
  XML_Translator &operator=(const XML_Translator &other) = delete;
  XML_Translator(XML_Translator &&other) = delete;
  XML_Translator &operator=(XML_Translator &&other) = delete;
  ~XML_Translator() override = default;

  // Convert to/from XML-escaped characters
  [[nodiscard]] std::string from([[maybe_unused]] const std::string_view &escapedString) const override { return std::string(escapedString); }

  [[nodiscard]] std::string to(const std::string_view &rawString) const override
  {
    std::string translated;
    for (const char16_t ch : toUtf16(std::string(rawString))) {
      if (isASCII(ch) && std::isprint(ch)) {
        if (ch == '&') {
          translated += "&amp;";
        } else if (ch == '<') {
          translated += "&lt;";
        } else if (ch == '>') {
          translated += "&gt;";
        } else if (ch == '\'') {
          translated += "&apos;";
        } else if (ch == '"') {
          translated += "&quot;";
        } else {
          translated += static_cast<char>(ch);
        }
      } else {
        translated += "&#x";
        const auto digits = "0123456789ABCDEF";
        translated += digits[ch >> 12 & 0x0f];
        translated += digits[ch >> 8 & 0x0f];
        translated += digits[ch >> 4 & 0x0f];
        translated += digits[ch&0x0f];
        translated += ";";
      }
    }
    return translated;
  }

private:

  /// <summary>
  /// Determine whether passed in character is valid ASCII
  /// </summary>
  /// <param name="utf16Char">UTF16 character.</param>
  /// <returns>True if valid ASCII.</returns>
  [[nodiscard]] static bool isASCII(const char16_t utf16Char) { return utf16Char > 0x001F && utf16Char < 0x0080; }
};
}// namespace YAML_Lib
