
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
/// <returns>==true, then ends with substr.</returns>
bool YAML_Parser::endsWith(const std::string &str, const std::string &substr) {
  const auto strLen = str.size();
  const auto substrLen = substr.size();
  if (strLen < substrLen)
    return false;

  return str.compare(strLen - substrLen, substrLen, substr) == 0;
}
/// <summary>
/// Remove any spaces at the end of str.
/// </summary>
/// <param name="str">Target string.</param>
void YAML_Parser::rightTrim(std::string &str) {
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
void YAML_Parser::moveToNext(ISource &source, const Delimiters &delimiters) {
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      source.next();
    }
  }
}
/// <summary>
/// Move to the next non-whitespace character in source stream; jumping over new
/// lines and stripping amy comments.
/// </summary>
/// <param name="source">Source stream.</param>
void YAML_Parser::moveToNextIndent(ISource &source) {
  bool indentFound{false};
  while (!indentFound) {
    while (source.more() && (source.isWS() || source.current() == kLineFeed)) {
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
std::string YAML_Parser::extractString(ISource &source, char quote) {
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
std::string YAML_Parser::extractToNext(ISource &source,
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
/// <param name="start character".></param>
/// <param name="end character".></param>
/// <returns>Extracted characters.</returns>
std::string YAML_Parser::extractInLine(ISource &source, char start, char end) {
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
void YAML_Parser::checkForEnd(ISource &source, const char end) {
  if (source.current() != end) {
    throw SyntaxError(source.getPosition(),
                      std::string("Missing closing ") + end + ".");
  }
  source.next();
}

} // namespace YAML_Lib