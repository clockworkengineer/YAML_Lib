
#pragma once

#include "ITranslator.hpp"

namespace YAML_Lib {

class YAML_Translator final : public ITranslator {
public:
  // YAML translator error
  struct Error final : std::runtime_error {
    explicit Error(const std::string &message)
        : std::runtime_error("YAML Translator Error: " + message) {}
  };

  YAML_Translator() = default;
  YAML_Translator(const YAML_Translator &other) = delete;
  YAML_Translator &operator=(const YAML_Translator &other) = delete;
  YAML_Translator(YAML_Translator &&other) = delete;
  YAML_Translator &operator=(YAML_Translator &&other) = delete;
  ~YAML_Translator() = default;

  // Convert to/from YAML escaped characters
  [[nodiscard]] std::string
  from([[maybe_unused]] const std::string &yamlString) const override {
    std::string translated;
    for (std::size_t idx = 0; idx < yamlString.size(); idx++) {
      if (yamlString[idx] == '\\') {
        idx++;
        if (yamlString[idx] == 't') {
          translated += "\t";
        } else if (yamlString[idx] == 'n') {
          translated += "\n";
        } else if (yamlString[idx] == '"') {
          translated += "\"";
        } else if (yamlString[idx] == 'b') {
          translated += "\b";
        } else if (yamlString[idx] == 'r') {
          translated += "\r";
        } else if (yamlString[idx] == 'f') {
          translated += "\f";
        } else if (yamlString[idx] == '\\') {
          translated += "\\";
        } else if (yamlString[idx] == 'x') {
          char hex[3];
          std::size_t end;
          hex[0] = yamlString[++idx];
          hex[1] = yamlString[++idx];
          translated += std::stoi(hex, &end, 16);
        } else if (yamlString[idx] == 'u') {
          char hex[3];
          std::size_t end;
          hex[0] = yamlString[++idx];
          hex[1] = yamlString[++idx];
          translated += std::stoi(hex, &end, 16);
          hex[0] = yamlString[++idx];
          hex[1] = yamlString[++idx];
          translated += std::stoi(hex, &end, 16);
        } else {
          translated += yamlString[idx];
        }
      } else {
        translated += yamlString[idx];
      }
    }
    return (translated);
  }

  [[nodiscard]] std::string to(const std::string &rawString) const override {
    std::string translated;
    return translated;
  }

private:
  /// <summary>
  /// Determine whether passed in character is vaid ASCII
  /// </summary>
  /// <param name="utf16Char">UTF16 character.</param>
  /// <returns>true if valid ASCII.</returns>
  [[nodiscard]] static bool isASCII(const char16_t utf16Char) {
    return utf16Char > 0x001F && utf16Char < 0x0080;
  }
};
} // namespace YAML_Lib
