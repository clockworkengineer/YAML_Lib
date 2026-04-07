
//
// Class: YAML_Parser_util
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Returns true if str ends with substr.
/// </summary>
/// <param name="str">Target string.</param>
/// <param name="substr">Ends with string.</param>
/// <returns>If true, str then ends with substr.</returns>
bool Default_Parser::endsWith(const std::string_view &str,
                              const std::string_view &substr) {
  const auto strLen = str.size();
  const auto substrLen = substr.size();
  if (strLen < substrLen)
    return false;

  return str.compare(strLen - substrLen, substrLen, substr) == 0;
}
/// <summary>
/// Scan the entire source for forbidden YAML control characters (YAML 1.2
/// §5.1).  Forbidden bytes: U+0000-U+0008, U+000B, U+000C, U+000E-U+001F,
/// U+007F.  Allowed control bytes: U+0009 (TAB), U+000A (LF), U+000D (CR).
/// Bytes >= 0x80 are UTF-8 multi-byte sequence bytes and are allowed.
/// </summary>
/// <param name="source">Source stream.</param>
void Default_Parser::validateInputCharacters(ISource &source) {
  source.save();
  while (source.more()) {
    const auto ch = static_cast<unsigned char>(source.current());
    // Forbidden: C0 controls except TAB(9), LF(10), CR(13); also DEL(127)
    if ((ch <= 0x08) || (ch == 0x0B) || (ch == 0x0C) ||
        (ch >= 0x0E && ch <= 0x1F) || (ch == 0x7F)) {
      const auto pos = source.getPosition();
      source.restore();
      throw SyntaxError(pos,
                        "Disallowed control character U+" +
                            [ch]() {
                              char buf[5];
                              std::snprintf(buf, sizeof(buf), "%04X",
                                            static_cast<unsigned>(ch));
                              return std::string(buf);
                            }() +
                            " in YAML stream.");
    }
    source.next();
  }
  source.restore();
}
/// <summary>
/// Remove any spaces at the end of str.
/// </summary>
/// <param name="str">Target string.</param>
void Default_Parser::rightTrim(std::string &str) {
  str.erase(
      std::find_if(str.rbegin(), str.rend(),
                   [](const unsigned char ch) { return !std::isspace(ch); })
          .base(),
      str.end());
}
/// <summary>
/// Move to the next delimiter on source stream in a set.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Set of possible delimiter characters.</param>
void Default_Parser::moveToNext(ISource &source, const Delimiters &delimiters) {
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      source.next();
    }
  }
}
/// <summary>
/// Move to the next non-whitespace character in source stream; jumping over new
/// lines and stripping amy comments. Tabs used as block indentation (at the
/// start of a line) are rejected as per the YAML 1.2 specification.
/// </summary>
/// <param name="source">Source stream.</param>
void Default_Parser::moveToNextIndent(ISource &source) {
  bool indentFound{false};
  while (!indentFound) {
    bool afterNewline = (source.getPosition().second == 1);
    while (source.more() && (source.isWS() || source.current() == kLineFeed)) {
      if (source.current() == kLineFeed) {
        afterNewline = true;
      } else if (afterNewline && source.current() == '\t') {
        throw SyntaxError(
            source.getPosition(),
            "Tab character not allowed in YAML block indentation.");
      }
      source.next();
    }
    if (isComment(source)) {
      moveToNext(source, {kLineFeed});
      if (source.more()) {
        source.next();
      }
    } else {
      indentFound = true;
    }
  }
}
/// <summary>
/// Extract quoted from source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="quote"></param>
/// <returns>Extracted characters.</returns>
std::string Default_Parser::extractString(ISource &source, const char quote) {
  std::string extracted{quote};
  source.next();
  while (source.more() && source.current() != quote) {
    extracted += source.current();
    source.next();
  }
  extracted += quote;
  if (source.more()) {
    source.next();
  }
  source.ignoreWS();
  return extracted;
}
/// <summary>
/// Extract characters from source stream up to a delimiter.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns>Extracted characters.</returns>
std::string Default_Parser::extractToNext(ISource &source,
                                          const Delimiters &delimiters) {
  std::string extracted;
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      extracted += source.append();
    }
  }
  return extracted;
}
/// <summary>
/// Extract characters from source stream up to a last end character.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="start">Start character.</param>
/// <param name="end">End character.</param>
/// <returns>Extracted characters.</returns>
std::string Default_Parser::extractInLine(ISource &source, const char start,
                                          const char end) {
  std::string extracted;
  unsigned long depth{1};
  extracted += start;
  source.next();
  while (depth > 0 && source.more()) {
    if (source.current() == start) {
      depth++;
    } else if (source.current() == end) {
      depth--;
    }
    extracted += source.current();
    source.next();
  }
  source.ignoreWS();
  return extracted;
}
/// <summary>
/// Check that end character has been found if not throw an exception.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="end">End character.</param>
void Default_Parser::checkForEnd(ISource &source, const char end) {
  if (source.current() != end) {
    throw SyntaxError(source.getPosition(),
                      std::string("Missing closing ") + end + ".");
  }
  source.next();
}

} // namespace YAML_Lib