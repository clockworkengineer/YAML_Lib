#pragma once
namespace YAML_Lib {
[[nodiscard]] std::u16string toUtf16(const std::string_view &utf8);
[[nodiscard]] std::string toUtf8(const std::u16string_view &utf16);
} // namespace YAML_Lib
