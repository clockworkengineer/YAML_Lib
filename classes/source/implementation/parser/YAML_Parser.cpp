
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
  moveToNextIndent(source);
}
/// <summary>
/// Check for the end of a plain flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="end">== true  then and of string found.</param>
bool YAML_Parser::endOfPlainFlowString(ISource &source) {
  return isKey(source) || isArray(source) || isComment(source) ||
         isDocumentStart(source) || isDocumentEnd(source);
}
/// <summary>
/// Merge overrides/extensions in dictionary. Overrides will have
/// "<<" keys; this function edits them into the YAML.
/// </summary>
YNode YAML_Parser::mergeOverrides(YNode &overrideRoot) {
  if (isA<Dictionary>(overrideRoot) &&
      YRef<Dictionary>(overrideRoot).contains(kOverride)) {
    auto &dictionary = YRef<Dictionary>(overrideRoot);
    std::set<std::string> overrideKeys;
    for (auto &entry : dictionary.value()) {
      if (entry.getKey() != kOverride) {
        overrideKeys.insert(entry.getKey());
      }
    }
    auto &innerDictionary = YRef<Dictionary>(dictionary[kOverride]);
    for (auto &entry : overrideKeys) {
      auto overrideEntry = mergeOverrides(dictionary[entry]);
      if (innerDictionary.contains(entry)) {
        innerDictionary[entry] = std::move(overrideEntry);
      } else {
        innerDictionary.add(DictionaryEntry(entry, overrideEntry));
      }
    }
    overrideRoot = std::move(overrideRoot[kOverride]);
  }
  return std::move(overrideRoot);
}
/// <summary>
/// Convert YAML key to a string YNode
/// </summary>
/// <param name="yamlString">YAML string.</param>
YNode YAML_Parser::convertYAMLToStringYNode(const std::string &yamlString) {
  BufferSource yamlKey{yamlString + kLineFeed};
  auto keyYNode = parseDocument(yamlKey, {kLineFeed});
  std::string keyString{YRef<Variant>(keyYNode).toKey()};
  char quote = '\"';
  if (isA<String>(keyYNode)) {
    quote = YRef<String>(keyYNode).getQuote();
    if (keyString.empty()) {
      quote = '"';
    }
  }
  return YNode::make<String>(keyString, quote, 0);
}
/// <summary>
/// Does YAML that is  passed in constitute a valid dictionary key?
/// </summary>
/// <param name="key">YAML sequence to be converted to be used as the
/// key.</param> <returns>==true value is a valid key.</returns>
bool YAML_Parser::isValidKey(const std::string &key) {
  try {
    BufferSource yamlKey{key + kLineFeed};
    const YNode keyYNode = parseDocument(yamlKey, {kLineFeed});
    return !keyYNode.isEmpty() && !isA<Comment>(keyYNode);
  } catch ([[maybe_unused]] const std::exception &e) {
    return false;
  }
}
/// <summary>
/// Extract YAML used in possible key value from source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>YAML for key value.</returns>
std::string YAML_Parser::extractKey(ISource &source) {
  Delimiters delimiters;
  if (isInlineDictionary(source)) {
    delimiters = {'}', kLineFeed};
  } else if (source.current() == '?') {
    source.next();
    delimiters = {':'};
  } else if (source.current() == '[')
    delimiters = {':', kLineFeed};
  else {
    delimiters = {':', ',', '}', kLineFeed};
  }
  std::string key = extractToNext(source, delimiters);
  if (delimiters.contains('}') && source.current() == '}' && key[0] == '{') {
    key += source.append();
    source.ignoreWS();
  }
  return key;
}
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
/// <summary>
/// Has a dictionary key been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>== true if a dictionary key has been found.</returns>
bool YAML_Parser::isKey(ISource &source) {
  source.save();
  bool keyPresent{false};
  std::string key{extractKey(source)};
  if (source.current() == ':') {
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
/// <summary>
/// Append character to YAML string performing any necessary newline folding.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="yamlString">YAML string appended too.</param>
void YAML_Parser::appendCharacterToString(ISource &source,
                                          std::string &yamlString) {
  if (source.current() == kLineFeed) {
    source.next();
    source.ignoreWS();
    if (source.current() == kLineFeed) {
      yamlString += source.append();
      source.ignoreWS();
    } else {
      yamlString += kSpace;
    }
  } else {
    yamlString += source.append();
  }
}
/// <summary>
/// Parse any block string chomping.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Specified block chomping.</returns>
YAML_Parser::BlockChomping YAML_Parser::parseBlockChomping(ISource &source) {
  source.next();
  if (const auto ch = source.current(); ch == '-') {
    return BlockChomping::strip;
  } else {
    if (ch == '+') {
      return BlockChomping::keep;
    }
    return BlockChomping::clip;
  }
}
/// <summary>
/// Parse a block string.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used in parsing.</param>
/// <param name="fillerDefault">Default filler.</param>
/// <param name="chomping">Comping mode.</param>
/// <returns>Block string parsed.</returns>
std::string YAML_Parser::parseBlockString(ISource &source,
                                          const Delimiters &delimiters,
                                          const char fillerDefault,
                                          const BlockChomping &chomping) {
  const unsigned long blockIndent = source.getPosition().second;
  std::string yamlString{};
  do {
    char filler{fillerDefault};
    if (blockIndent < source.getPosition().second) {
      if (yamlString.back() != kLineFeed) {
        yamlString += kLineFeed;
      }
      yamlString += std::string(source.getPosition().second - 1, kSpace);
      filler = kLineFeed;
    }
    yamlString += extractToNext(source, delimiters);
    yamlString += filler;
    if (source.more()) {
      source.next();
    }
    while (source.more() && (source.isWS() || source.current() == kLineFeed)) {
      if (source.current() == kLineFeed) {
        yamlString.pop_back();
        yamlString += "\n\n";
      }
      source.next();
    }
  } while (source.more() && blockIndent <= source.getPosition().second);
  if (chomping == BlockChomping::clip || chomping == BlockChomping::strip) {
    if (endsWith(yamlString, "\n\n\n")) {
      yamlString.pop_back();
    }
    yamlString.pop_back();
  }
  if (chomping == BlockChomping::strip && yamlString.back() == kLineFeed) {
    yamlString.pop_back();
  }
  if (chomping == BlockChomping::keep && source.more() &&
      source.current() == kLineFeed) {
    yamlString += kLineFeed;
  }
  return yamlString;
}
/// <summary>
/// Parse dictionary key on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>String YNode for dictionary key.</returns>
YNode YAML_Parser::parseKey(ISource &source) {
  std::string key{extractKey(source)};
  if (source.more() && source.current() != '}' && source.current() != ',') {
    source.next();
  }
  rightTrim(key);
  if (!isValidKey(key)) {
    throw SyntaxError(source.getPosition(),
                      "Invalid key '" + key + "' specified.");
  }
  return convertYAMLToStringYNode(key);
}
/// <summary>
/// Parse folded block string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parseFoldedBlockString(ISource &source,
                                          const Delimiters &delimiters) {
  const BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  moveToNextIndent(source);
  auto blockIndent = source.getPosition().second;
  std::string yamlString{
      parseBlockString(source, delimiters, kSpace, chomping)};
  return YNode::make<String>(yamlString, '>', blockIndent);
}
/// <summary>
/// Parse literal block string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parseLiteralBlockString(ISource &source,
                                           const Delimiters &delimiters) {
  const BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  moveToNextIndent(source);
  auto blockIndent = source.getPosition().second;
  std::string yamlString{
      parseBlockString(source, delimiters, kLineFeed, chomping)};
  return YNode::make<String>(yamlString, '|', blockIndent);
}
/// <summary>
/// Parse plain flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parsePlainFlowString(ISource &source,
                                        const Delimiters &delimiters) {
  std::string yamlString{extractToNext(source, delimiters)};
  if (source.current() != kLineFeed) {
    rightTrim(yamlString);
  } else {
    while (source.more() && !endOfPlainFlowString(source)) {
      appendCharacterToString(source, yamlString);
    }
    if (yamlString.back() == kSpace || yamlString.back() == kLineFeed) {
      yamlString.pop_back();
    }
  }
  return YNode::make<String>(yamlString, '\0');
}
/// <summary>
/// Parse quoted flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <returns>String YNode.</returns>
YNode YAML_Parser::parseQuotedFlowString(ISource &source,
                                         const Delimiters &delimiters) {
  const char quote = source.append();
  std::string yamlString;
  if (quote == '"') {
    while (source.more() && source.current() != quote) {
      if (source.current() == '\\') {
        yamlString += source.append();
        yamlString += source.append();
      } else {
        appendCharacterToString(source, yamlString);
      }
    }
    yamlString = yamlTranslator->from(yamlString);
  } else {
    while (source.more()) {
      if (source.current() == quote) {
        source.next();
        if (source.current() == quote) {
          yamlString += source.append();
        } else {
          break;
        }
      } else {
        appendCharacterToString(source, yamlString);
      }
    }
  }
  moveToNext(source, delimiters);
  moveToNextIndent(source);
  return YNode::make<String>(yamlString, quote);
}
/// <summary>
/// Parse a comment on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse comment.<</param>
/// <returns>Comment YNode.</returns>
YNode YAML_Parser::parseComment(ISource &source,
                                [[maybe_unused]] const Delimiters &delimiters) {
  source.next();
  std::string comment{extractToNext(source, {kLineFeed})};
  if (source.more()) {
    source.next();
  }
  return YNode::make<Comment>(comment);
}
/// <summary>
/// Parse a numeric value on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse number./param>
/// <returns>Number YNode.</returns>
YNode YAML_Parser::parseNumber(ISource &source, const Delimiters &delimiters) {
  YNode yNode;
  source.save();
  std::string numeric{extractToNext(source, delimiters)};
  rightTrim(numeric);
  if (Number number{numeric}; number.is<int>() || number.is<long>() ||
                              number.is<long long>() || number.is<float>() ||
                              number.is<double>() || number.is<long double>()) {
    moveToNext(source, delimiters);
    moveToNextIndent(source);
    yNode = YNode::make<Number>(number);
  }
  if (yNode.isEmpty()) {
    source.restore();
  }
  return yNode;
}
/// <summary>
/// Parse None/Null on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse None.</param>
/// <returns>None YNode.</returns>
YNode YAML_Parser::parseNone(ISource &source, const Delimiters &delimiters) {
  YNode yNode;
  source.save();
  std::string none{extractToNext(source, delimiters)};
  rightTrim(none);
  if (none == "null" || none == "~") {
    yNode = YNode::make<Null>();
  }
  if (yNode.isEmpty()) {
    source.restore();
  }
  return yNode;
}
/// <summary>
/// Parse boolean value on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse boolean.</param>
/// <returns>Boolean YNode.</returns>
YNode YAML_Parser::parseBoolean(ISource &source, const Delimiters &delimiters) {
  YNode yNode;
  source.save();
  std::string boolean{extractToNext(source, delimiters)};
  rightTrim(boolean);
  if (Boolean::isTrue.contains(boolean)) {
    yNode = YNode::make<Boolean>(true, boolean);
  } else if (Boolean::isFalse.contains(boolean)) {
    yNode = YNode::make<Boolean>(false, boolean);
  }
  if (yNode.isEmpty()) {
    source.restore();
  }
  return yNode;
}
/// <summary>
/// Parse anchor on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse anchor.</param>
/// <returns>Anchor YNode.</returns>
YNode YAML_Parser::parseAnchor(ISource &source, const Delimiters &delimiters) {
  source.next();
  const std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.ignoreWS();
  std::string unparsed{};
  if (source.current() != kLineFeed) {
    unparsed += extractToNext(source, {kLineFeed});
    moveToNextIndent(source);
  } else {
    moveToNextIndent(source);
    const auto anchorIndent = source.getPosition().second;
    do {
      std::string indent(source.getPosition().second, kSpace);
      unparsed += indent + extractToNext(source, {kLineFeed}) + "\n";
      moveToNextIndent(source);
    } while (source.getPosition().second >= anchorIndent);
  }
  yamlAliasMap[name] = unparsed;
  BufferSource anchor{unparsed};
  return parseDocument(anchor, delimiters);
}
/// <summary>
/// Parse alias on source stream and substitute alias.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse alias.</param>
/// <returns>Alias anchor.</returns>
YNode YAML_Parser::parseAlias(ISource &source, const Delimiters &delimiters) {
  source.next();
  const std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  const std::string unparsed{yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  return parseDocument(anchor, delimiters);
}
/// <summary>
/// Parse alias on source stream, substitute alias, and any overrides.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse alias.</param>
/// <returns>Alias anchor with overrides.</returns>
YNode YAML_Parser::parseOverride(ISource &source,
                                 const Delimiters &delimiters) {
  source.next();
  source.next();
  source.next();
  source.ignoreWS();
  if (source.current() != '*') {
    throw SyntaxError(source.getPosition(), "Missing '*' from alias.");
  }
  source.next();
  const std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  const std::string unparsed{yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, delimiters);
  return parsed;
}
/// <summary>
/// Parse array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the array.</param>
/// <returns>Array YNode.</returns>
YNode YAML_Parser::parseArray(ISource &source, const Delimiters &delimiters) {
  unsigned long arrayIndent = source.getPosition().second;
  indentLevel++;
  YNode yNode = YNode::make<Array>(arrayIndent);
  while (source.more() && isArray(source) &&
         arrayIndent == source.getPosition().second) {
    source.next();
    YRef<Array>(yNode).add(parseDocument(source, delimiters));
    moveToNextIndent(source);
  }
  if (isArray(source) && indentLevel == 1 &&
      arrayIndent > source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Invalid indentation for array element.");
  }
  indentLevel--;
  return yNode;
}
/// <summary>
/// Parse inline array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the inline array.</param>
/// <returns>Array YNode.</returns>
YNode YAML_Parser::parseInlineArray(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters) {
  unsigned long arrayIndent = source.getPosition().second;
  Delimiters inLineArrayDelimiters = {delimiters};
  inLineArrayDelimiters.insert({',', ']'});
  YNode yNode = YNode::make<Array>(arrayIndent);
  do {
    source.next();
    YRef<Array>(yNode).add(parseDocument(source, inLineArrayDelimiters));
    auto &element = YRef<Array>(yNode).value().back();
    if (isA<String>(element)) {
      if (YRef<String>(element).value().empty() &&
          YRef<String>(element).getQuote() == '\0') {
        if (source.current() != ']') {
          throw SyntaxError("Unexpected ',' in in-line array.");
        } else {
          YRef<Array>(yNode).value().pop_back();
        }
      }
    }
  } while (source.current() == ',');
  checkForEnd(source, ']');
  return yNode;
}
/// <summary>
/// Parse dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry YAML_Parser::parseKeyValue(ISource &source,
                                           const Delimiters &delimiters) {
  const unsigned long keyIndent = source.getPosition().second;
  YNode keyYNode = parseKey(source);
  source.ignoreWS();
  if (isKey(source)) {
    throw SyntaxError(source.getPosition(),
                      "Only an inline/compact dictionary is allowed.");
  }
  moveToNextIndent(source);
  YNode yNode{YNode::make<Null>()};
  if ((source.getPosition().second > keyIndent || isInlineArray(source) ||
       isInlineDictionary(source))) {
    yNode = parseDocument(source, delimiters);
  }
  return {keyYNode, yNode};
}
/// <summary>
/// Parse inline dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry YAML_Parser::parseInlineKeyValue(ISource &source,
                                                 const Delimiters &delimiters) {
  YNode keyYNode = parseKey(source);
  // source.ignoreWS();
  // moveToNextIndent(source);
  YNode yNode{YNode::make<Null>()};
  if (source.current() != ',') {
    yNode = parseDocument(source, delimiters);
  }
  if (isA<String>(yNode)) {
    if (YRef<String>(yNode).value().empty() &&
        YRef<String>(yNode).getQuote() == '\0') {
      yNode = YNode::make<Null>();
    }
  }
  return {keyYNode, yNode};
}
/// <summary>
/// Parse a dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse dictionary.</param>
/// <returns>Dictionary YNode.</returns>
YNode YAML_Parser::parseDictionary(ISource &source,
                                   const Delimiters &delimiters) {

  if (delimiters.contains('}')) {
    return {};
  }
  unsigned long dictionaryIndent = source.getPosition().second;
  YNode yNode = YNode::make<Dictionary>(dictionaryIndent);
  while (source.more() && dictionaryIndent == source.getPosition().second) {
    if (isKey(source)) {
      auto entry = parseKeyValue(source, delimiters);
      if (YRef<Dictionary>(yNode).contains(entry.getKey())) {
        throw SyntaxError(source.getPosition(),
                          "Dictionary already contains key '" + entry.getKey() +
                              "'.");
      }
      YRef<Dictionary>(yNode).add(std::move(entry));
    } else if (isDocumentStart(source) || isDocumentEnd(source)) {
      break;
    } else {
      if (dictionaryIndent == source.getPosition().second) {
        throw SyntaxError(source.getPosition(),
                          "Missing key/value pair from indentation level.");
      }
    }
    moveToNextIndent(source);
  }
  if (isKey(source) && dictionaryIndent < source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Mapping key has the incorrect indentation.");
  }
  return mergeOverrides(yNode);
}
/// <summary>
/// Parse inline dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse inline
/// dictionary.</param> <returns>Dictionary YNode.</returns>
YNode YAML_Parser::parseInlineDictionary(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters) {
  Delimiters inLineDictionaryDelimiters = {delimiters};
  inLineDictionaryDelimiters.insert({',', '}'});
  unsigned long dictionaryIndent = source.getPosition().second;
  YNode yNode = YNode::make<Dictionary>(dictionaryIndent);
  do {
    source.next();
    moveToNextIndent(source);
    if (source.current() == ',') {
      throw SyntaxError("Unexpected ',' in in-line dictionary.");
    } else if (source.current() != '}') {
      YRef<Dictionary>(yNode).add(
          parseInlineKeyValue(source, inLineDictionaryDelimiters));
    }

  } while (source.current() == ',');
  checkForEnd(source, '}');
  if (source.current() == ':') {
    throw SyntaxError(
        source.getPosition(),
        "Inline dictionary used as key is meant to be on one line.");
  }
  return yNode;
}
/// <summary>
/// Parse YAML document on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse document.</param>
/// <returns>Document root YNode.</returns>
YNode YAML_Parser::parseDocument(ISource &source,
                                 const Delimiters &delimiters) {
  moveToNextIndent(source);
  for (const auto &[fst, snd] : parsers) {
    if (fst(source)) {
      if (YNode yNode = snd(source, delimiters); !yNode.isEmpty()) {
        return yNode;
      }
    }
  }
  throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
}
/// <summary>
/// Parse YAML documents on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Array of YAML documents.</returns>
std::vector<YNode> YAML_Parser::parse(ISource &source) {
  std::vector<YNode> yNodeTree;
  for (bool inDocument = false; source.more();) {
    // Start of a document
    if (isDocumentStart(source)) {
      inDocument = true;
      moveToNext(source, {kLineFeed, '|', '>'});
      moveToNextIndent(source);
      yNodeTree.push_back(YNode::make<Document>());
      // End of a document
    } else if (isDocumentEnd(source)) {
      moveToNext(source, {kLineFeed});
      moveToNextIndent(source);
      if (!inDocument) {
        yNodeTree.push_back(YNode::make<Document>());
      }
      inDocument = false;
      // Inter document comment
    } else if (isComment(source) && !inDocument) {
      parseComment(source, {kLineFeed});
      // Parse document contents
    } else {
      if (!inDocument) {
        yNodeTree.push_back(YNode::make<Document>());
      }
      inDocument = true;
      YRef<Document>(yNodeTree.back())
          .add(parseDocument(source, {kLineFeed, '#'}));
    }
  }

  return yNodeTree;
}
} // namespace YAML_Lib