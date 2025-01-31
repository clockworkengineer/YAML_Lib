//
// Class: Converter
//
// Description: Convert characters to/from UTF8 and UTF16.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML.hpp"
#include "YAML_Converter.hpp"
#include "YAML_Error.hpp"

namespace YAML_Lib {

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>
    utf16Converter;

/// <summary>
/// Convert utf8 <-> utf16 strings.
/// </summary>
std::u16string toUtf16(const std::string &utf8) {
  if (utf8.find(kNull) != std::string::npos) {
    throw Error("Tried to convert a null character.");
  }
  return utf16Converter.from_bytes(utf8);
}
std::string toUtf8(const std::u16string &utf16) {
  if (utf16.find(static_cast<char16_t>(kNull)) != std::string::npos) {
    throw Error("Tried to convert a null character.");
  }
  return utf16Converter.to_bytes(utf16);
}

} // namespace YAML_Lib
