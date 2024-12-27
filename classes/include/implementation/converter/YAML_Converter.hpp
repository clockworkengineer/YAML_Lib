#pragma once
#include <codecvt>
#include <locale>
#include <string>
namespace YAML_Lib {
[[nodiscard]] std::u16string toUtf16(const std::string &utf8);
[[nodiscard]] std::string toUtf8(const std::u16string &utf16);
} // namespace YAML_Lib
