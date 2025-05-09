//
// Class: Default_Translator
//
// Description: Translate to/from YAML string escapes within source
// strings. This is the default translator but is possible to write
// a custom one and pass it to the YAML class constructor to be used.
//
// Dependencies: C++20 - Language standard features used.
//

#include "Default_Translator.hpp"

namespace YAML_Lib {

static const std::vector<std::pair<const char, const char>> escapeSequences{
    {'\\', '\\'}, {'t', '\t'}, {kDoubleQuote, '\"'}, {'b', '\b'},
    {'f', '\f'},  {'n', '\n'}, {'r', '\r'}};

/// <summary>
/// Convert \uxxxx escape sequences in a string to their correct sequence
///  of UTF-8 characters.
/// </summary>
/// <param name="current">Current character position.</param>
/// <param name="numberOfCharacters">Number of characters left in source
/// string.</param> <returns>UTF16 character for "\uxxxx".</returns>
char16_t decodeUTF16(std::string_view::const_iterator &current,
                     const ptrdiff_t numberOfCharacters) {
  if (numberOfCharacters >= 4) {
    char16_t utf16value{};
    // Hex digits will be ascii so can throw away high order byte of char
    const std::array hexDigits{(current[1]), (current[2]), (current[3]),
                               (current[4]), kNull};
    char *end;
    utf16value +=
        static_cast<char16_t>(std::strtol(hexDigits.data(), &end, 16));
    if (*end == kNull) {
      current += hexDigits.size();
      return utf16value;
    }
  }
  throw Default_Translator::Error("Syntax error detected.");
}
/// <summary>
/// Convert "\x00" escape sequences in a string to their correct sequence
/// of UTF-8 characters.
/// </summary>
/// <param name="current">Current character position.</param>
/// <param name="numberOfCharacters">Number of characters left in source
/// string.</param> <returns>UTF16 character for ""\x00"".</returns>
char16_t decodeUTF8(std::string_view::const_iterator &current,
                    const ptrdiff_t numberOfCharacters) {
  if (numberOfCharacters >= 4) {
    char16_t utf16value{};
    // Hex digits will be ascii so can throw away high order byte of char
    const std::array hexDigits{(current[1]), (current[2]), kNull};
    char *end;
    utf16value +=
        static_cast<char16_t>(std::strtol(hexDigits.data(), &end, 16));
    if (*end == kNull) {
      current += hexDigits.size();
      return utf16value;
    }
  }
  throw Default_Translator::Error("Syntax error detected.");
}

/// <summary>
/// Convert UTF16 character into its \uxxxx encoded escape sequence.
/// </summary>
/// <param name="utf16Char">UTF16 encode character.</param>
/// <returns>Escape sequence "\uxxxx" for utf16 character.</returns>
std::string encodeUTF16(const char16_t utf16Char) {
  std::string utf8Buffer;
  const auto digits = "0123456789ABCDEF";
  utf8Buffer += "\\u";
  utf8Buffer += digits[utf16Char >> 12 & 0x0f];
  utf8Buffer += digits[utf16Char >> 8 & 0x0f];
  utf8Buffer += digits[utf16Char >> 4 & 0x0f];
  utf8Buffer += digits[utf16Char & 0x0f];
  return utf8Buffer;
}

/// <summary>
/// Return true if a character is a valid upper surrogate.
/// </summary>
/// <param name="utf16Char">UTF16 character.</param>
/// <returns>true if valid upper surrogate.</returns>
bool isValidSurrogateUpper(const char16_t utf16Char) {
  return utf16Char >= static_cast<char16_t>(kHighSurrogatesBegin) &&
         utf16Char <= static_cast<char16_t>(kHighSurrogatesEnd);
}
/// <summary>
/// Return true if a character is a valid lower surrogate.
/// </summary>
/// <param name="utf16Char">UTF16 character.</param>
/// <returns>true if valid lower surrogate.</returns>
bool isValidSurrogateLower(const char16_t utf16Char) {
  return utf16Char >= static_cast<char16_t>(kLowSurrogatesBegin) &&
         utf16Char <= static_cast<char16_t>(kLowSurrogatesEnd);
}

/// <summary>
/// Check that there are no single unpaired UTF-16 surrogates.From what I see
/// this is meant to be an error but from searching the web I have not found a
/// definitive answer.
/// </summary>
/// <param name="utf16Buffer">UTF16 string.</param>
/// <returns>false if string contains an unpaired surrogate.</returns>
bool unpairedSurrogatesInBuffer(const std::u16string &utf16Buffer) {
  int index = 0;
  while (index <= static_cast<int>(utf16Buffer.size()) - 1) {
    if (isValidSurrogateUpper(utf16Buffer[index]) &&
        isValidSurrogateLower(utf16Buffer[index + 1])) {
      index++;
    } else if (isValidSurrogateUpper(utf16Buffer[index]) ||
               isValidSurrogateLower(utf16Buffer[index + 1])) {
      return true;
    }
    index++;
  }
  return false;
}

/// <summary>
/// Determine whether passed in character is vaid ASCII
/// </summary>
/// <param name="utf16Char">UTF16 character.</param>
/// <returns>true if valid ASCII.</returns>
bool isASCII(const char16_t utf16Char) {
  return utf16Char > 0x001F && utf16Char < 0x0080;
}

/// <summary>
/// YAML translator constructor.
/// </summary>
Default_Translator::Default_Translator() {
  // Initialise tables used to convert to/from single character
  // escape sequences within a YAML string.
  for (const auto &[key, value] : escapeSequences) {
    fromEscape[key] = value;
    toEscape[value] = key;
  }
}

/// <summary>
/// Convert any escape sequences in a string to their correct sequence
/// of UTF-8 characters. If input string contains any unpaired surrogates
/// then this is deemed as a syntax error and an error is duly thrown.
/// </summary>
/// <param name="escapedString">YAML string to process.</param>
/// <returns>String with escapes translated.</returns>
std::string Default_Translator::from(const std::string_view &escapedString) const {
  std::u16string utf16Buffer;
  for (auto current = escapedString.begin(); current != escapedString.end();) {
    // Normal character
    if (*current != '\\') {
      utf16Buffer += *current++;
      continue;
    }
    ++current;
    // Check an escape sequence if characters to process
    if (current != escapedString.end()) {
      // Single character
      if (fromEscape.contains(*current)) {
        utf16Buffer += fromEscape[static_cast<char>(*current)];
        ++current;
      }
      // UTF16 "\uxxxx"
      else if (*current == 'u') {
        utf16Buffer +=
            decodeUTF16(current, std::distance(current, escapedString.end()));
        // UTF8 "\x00"
      } else if (*current == 'x') {
        utf16Buffer +=
            decodeUTF8(current, std::distance(current, escapedString.end()));
      }
      // Escaped ASCII
      else if (isASCII(*current)) {
        utf16Buffer += *current;
        ++current;
      }
      // Invalid escaped character
      else {
        throw Error("Invalid escaped character.");
      }
    } else {
      throw Error("Premature and of character escape sequence.");
    }
  }
  if (unpairedSurrogatesInBuffer(utf16Buffer)) {
    throw Error("Unpaired surrogate found.");
  }
  return toUtf8(utf16Buffer);
}

/// <summary>
/// Convert a string from raw charater values (UTF8) so that it has character
/// escapes where applicable for its YAML form.
/// </summary>
/// <param name="rawString">String to convert.</param>
/// <returns>YAML string with escapes.</returns>
std::string Default_Translator::to(const std::string_view &rawString) const {
  std::string escapedString;
  for (char16_t utf16Char : toUtf16(std::string(rawString))) {
    // Control characters
    if (toEscape.contains(utf16Char)) {
      escapedString += '\\';
      escapedString += toEscape[utf16Char];
    }
    // ASCII
    else if (isASCII(utf16Char) && std::isprint(utf16Char)) {
      escapedString += static_cast<char>(utf16Char);
    }
    // UTF8 escaped
    else {
      escapedString += encodeUTF16(utf16Char);
    }
  }
  return escapedString;
}
} // namespace YAML_Lib
