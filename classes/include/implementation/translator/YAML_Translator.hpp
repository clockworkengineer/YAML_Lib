
#pragma once

#include "ITranslator.hpp"
#include "YAML_Converter.hpp"

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
  ~YAML_Translator() override = default;

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
          translated += static_cast<char>(std::stoi(hex, &end, 16));
        } else if (yamlString[idx] == 'u') {
          char hex[3];
          std::size_t end;
          [[maybe_unused]] char16_t utf16;
          std::u16string utf16string;
          std::string utf8string;
          hex[0] = yamlString[++idx];
          hex[1] = yamlString[++idx];
          utf16 = (static_cast<char>(std::stoi(hex, &end, 16)) << 8);
          hex[0] = yamlString[++idx];
          hex[1] = yamlString[++idx];
          utf16 |= static_cast<char>(std::stoi(hex, &end, 16));
          utf16string.push_back(utf16);
          translated += toUtf8(utf16string);
          continue;
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
};
} // namespace YAML_Lib
