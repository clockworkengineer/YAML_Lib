#pragma once

namespace JSON_Lib {
[[nodiscard]] std::u16string toUtf16(const std::string &utf8);
[[nodiscard]] std::string toUtf8(const std::u16string &utf16);
}// namespace JSON_Lib
