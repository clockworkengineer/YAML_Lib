//
// Class: Converter
//
// Description: Convert characters to/from UTF8 and UTF16 (pure C++20, portable, thread-safe).
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "implementation/converter/YAML_Converter.hpp"

namespace YAML_Lib {

std::u16string toUtf16(const std::string_view& utf8) {
  if (utf8.find(kNull) != std::string::npos) {
    YAML_THROW(Error, "Tried to convert a null character.");
  }
  std::u16string result;
  result.reserve(utf8.size());

  std::size_t i = 0;
  const std::size_t n = utf8.size();
  while (i < n) {
    const auto c0 = static_cast<unsigned char>(utf8[i++]);
    char32_t codepoint = 0;

    if (c0 < 0x80) {
      codepoint = c0;
    } else if ((c0 & 0xE0) == 0xC0) {
      if (i >= n) {
        YAML_THROW(Error, "Truncated UTF-8 sequence.");
      }
      const auto c1 = static_cast<unsigned char>(utf8[i++]);
      if ((c1 & 0xC0) != 0x80) {
        YAML_THROW(Error, "Invalid UTF-8 continuation byte.");
      }
      codepoint = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
    } else if ((c0 & 0xF0) == 0xE0) {
      if (i + 1 >= n) {
        YAML_THROW(Error, "Truncated UTF-8 sequence.");
      }
      const auto c1 = static_cast<unsigned char>(utf8[i++]);
      const auto c2 = static_cast<unsigned char>(utf8[i++]);
      if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80) {
        YAML_THROW(Error, "Invalid UTF-8 continuation byte.");
      }
      codepoint = ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
    } else if ((c0 & 0xF8) == 0xF0) {
      if (i + 2 >= n) {
        YAML_THROW(Error, "Truncated UTF-8 sequence.");
      }
      const auto c1 = static_cast<unsigned char>(utf8[i++]);
      const auto c2 = static_cast<unsigned char>(utf8[i++]);
      const auto c3 = static_cast<unsigned char>(utf8[i++]);
      if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) {
        YAML_THROW(Error, "Invalid UTF-8 continuation byte.");
      }
      codepoint = ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
    } else {
      YAML_THROW(Error, "Invalid UTF-8 leading byte.");
    }

    if (codepoint <= 0xFFFF) {
      result.push_back(static_cast<char16_t>(codepoint));
    } else if (codepoint <= 0x10FFFF) {
      codepoint -= 0x10000;
      result.push_back(static_cast<char16_t>(0xD800 + (codepoint >> 10)));
      result.push_back(static_cast<char16_t>(0xDC00 + (codepoint & 0x3FF)));
    } else {
      YAML_THROW(Error, "Unicode codepoint exceeds maximum value.");
    }
  }

  return result;
}

std::string toUtf8(const std::u16string_view& utf16) {
  if (utf16.find(static_cast<char16_t>(kNull)) != std::u16string_view::npos) {
    YAML_THROW(Error, "Tried to convert a null character.");
  }
  std::string result;
  result.reserve(utf16.size() * 3 / 2);

  std::size_t i = 0;
  const std::size_t n = utf16.size();
  while (i < n) {
    char32_t codepoint = utf16[i++];
    if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
      if (i >= n) {
        YAML_THROW(Error, "Unpaired high surrogate in UTF-16.");
      }
      const char32_t low = utf16[i++];
      if (low < 0xDC00 || low > 0xDFFF) {
        YAML_THROW(Error, "Invalid low surrogate in UTF-16.");
      }
      codepoint = 0x10000 + (((codepoint - 0xD800) << 10) | (low - 0xDC00));
    } else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
      YAML_THROW(Error, "Unpaired low surrogate in UTF-16.");
    }

    if (codepoint < 0x80) {
      result.push_back(static_cast<char>(codepoint));
    } else if (codepoint < 0x800) {
      result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
      result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint < 0x10000) {
      result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
      result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
      result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x10FFFF) {
      result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
      result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
      result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
      result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else {
      YAML_THROW(Error, "Unicode codepoint exceeds maximum value.");
    }
  }

  return result;
}

std::string toUtf8(char16_t utf16) {
  const std::u16string_view sv(&utf16, 1);
  return toUtf8(sv);
}

}  // namespace YAML_Lib
