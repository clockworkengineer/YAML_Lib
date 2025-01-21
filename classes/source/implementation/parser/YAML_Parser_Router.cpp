
//
// Class: YAML_Parser
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Is YAML overrides on source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true value is an override.</returns>
bool YAML_Parser::isOverride(ISource &source) {
  source.save();
  const bool isOverride{source.match("<<:")};
  source.restore();
  return isOverride;
}
// <summary>
// Has a dictionary key been found in the source stream?
// </summary>
// <param name="source">Source stream.</param>
// <returns>== true if a dictionary key has been found.</returns>
bool YAML_Parser::isKey(ISource &source) {
  source.save();
  bool keyPresent{false};
  std::string key{extractKey(source)};
  if (source.current() == ':') {
    if (key[0] == '{' || key[0] == '[') {
      if (key.find('\n') != std::string::npos) {
        if (key[0] == '{') {
          throw SyntaxError(
              source.getPosition(),
              "Inline dictionary used as key is meant to be on one line.");
        } else {
          throw SyntaxError(
              source.getPosition(),
              "Inline array used as key is meant to be on one line.");
        }
      }
    }
    source.next();
    if (source.current() == ' ' || source.current() == kLineFeed) {
      rightTrim(key);
      keyPresent = isValidKey(key);
    }
  }
  source.restore();
  return keyPresent;
}
/// <summary>
/// Has an array element been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true if an array element has been found.</returns>
bool YAML_Parser::isArray(ISource &source) {
  source.save();
  auto ch = source.current();
  auto arrayPresent{false};
  if (source.more() && ch == '-') {
    source.next();
    ch = source.current();
    arrayPresent = ch == kSpace || ch == kLineFeed;
  }
  source.restore();
  return arrayPresent;
}
/// <summary>
/// Has a possible boolean value been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true if a boolean value has been found.</returns>
bool YAML_Parser::isBoolean(const ISource &source) {
  const auto ch = source.current();
  return ch == 'T' || ch == 'F' || ch == 'O' || ch == 'Y' || ch == 'N';
}
/// <summary>
/// Has a quoted string been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true then a quoted string has been found.</returns>
bool YAML_Parser::isQuotedString(const ISource &source) {
  const auto ch = source.current();
  return ch == '\'' || ch == '"';
}
/// <summary>
/// Has a possible number been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true then a number has been found.</returns>
bool YAML_Parser::isNumber(const ISource &source) {
  const auto ch = source.current();
  return (ch >= '0' && ch <= '9') || ch == '-' || ch == '+';
}
/// <summary>
/// Has a possible null value been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true a null (none) value has been found.</returns>
bool YAML_Parser::isNone(const ISource &source) {
  const auto second = source.current();
  return second == 'n' || second == '~';
}
/// <summary>
/// Has a folded block string been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true a founded block string has been found.</returns>
bool YAML_Parser::isFoldedBlockString(const ISource &source) {
  return source.current() == '>';
}
/// <summary>
/// Has a piped block string been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true a piped block string has been found.</returns>
bool YAML_Parser::isPipedBlockString(const ISource &source) {
  return source.current() == '|';
}
/// <summary>
/// Has a comment been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true a comment has been found.</returns>
bool YAML_Parser::isComment(const ISource &source) {
  return source.current() == '#';
}
/// <summary>
/// Has an anchor been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true an anchor has been found.</returns>
bool YAML_Parser::isAnchor(const ISource &source) {
  return source.current() == '&';
}
/// <summary>
/// Has an alias been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true an alias has been found.</returns>
bool YAML_Parser::isAlias(const ISource &source) {
  return source.current() == '*';
}
/// <summary>
/// Has an inline array been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true an inline array has been found.</returns>
bool YAML_Parser::isInlineArray(const ISource &source) {
  return source.current() == '[';
}
/// <summary>
/// Has an inline dictionary been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true an inline dictionary has been found./returns>
bool YAML_Parser::isInlineDictionary(const ISource &source) {
  return source.current() == '{';
}
/// <summary>
/// Has a dictionary been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true a dictionary has been found./returns>
bool YAML_Parser::isDictionary(ISource &source) { return isKey(source); }
/// <summary>
/// Has document start been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>== true a start document has been found.</returns>
bool YAML_Parser::isDocumentStart(ISource &source) {
  source.save();
  const bool isStart{source.match(kStartDocument)};
  source.restore();
  return isStart;
}
/// <summary>
/// Has the document end been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>== true an end document has been found.</returns>
bool YAML_Parser::isDocumentEnd(ISource &source) {
  source.save();
  const bool isEnd{source.match(kEndDocument)};
  source.restore();
  return isEnd;
}
/// <summary>
/// Last parser router table entry so return true.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>True</returns>
bool YAML_Parser::isDefault([[maybe_unused]] ISource &source) { return true; }

} // namespace YAML_Lib