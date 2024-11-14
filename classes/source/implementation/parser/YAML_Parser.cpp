
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
/// <returns>==true,then ends with substr.</returns>
bool endsWith(const std::string &str, const std::string &substr) {
  auto strLen = str.size();
  auto substrLen = substr.size();
  if (strLen < substrLen)
    return false;

  return str.compare(strLen - substrLen, substrLen, substr) == 0;
}
/// <summary>
/// Remove any spaces at the end of str.
/// </summary>
/// <param name="str">Target string.</param>
void rightTrim(std::string &str) {
  str.erase(std::find_if(str.rbegin(), str.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          str.end());
}
/// <summary>
/// Remove any spaces at the front of str.
/// </summary>
/// <param name="str">Target string.</param>
void leftTrim(std::string &str) {
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}
/// <summary>
/// Move to the next non-whitespace character in source stream; jumping over new lines.
/// </summary>
/// <param name="source">Source stream.</param>
void moveToNextIndent(ISource &source) {
  while (source.more() && (source.isWS() || source.current() == kLineFeed)) {
    source.next();
  }
}
/// <summary>
/// Move to next delimeter on source stream in a setand then to next indent.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Set of possible delimeter characters.</param>
void moveToNext(ISource &source, const YAML_Parser::Delimeters &delimiters) {
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      source.next();
    }
  }
  moveToNextIndent(source);
}
/// <summary>
/// Extract charaters from source stream up to a delimeter.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
std::string extractToNext(ISource &source,
                          const YAML_Parser::Delimeters &delimiters) {
  std::string extracted;
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      extracted += source.current();
      source.next();
    }
  }
  return (extracted);
}
/// <summary>
/// 
/// </summary>
/// <param name="key">YAML sequence to be converted to be used as key.</param>
/// <returns>==true value is a valid key.</returns>
bool YAML_Parser::isValidKey(const std::string &key) {
  try {
    BufferSource keyYAML{key + kLineFeed};
    YNode keyYNode = parseDocument(keyYAML, {kLineFeed});
    if (isA<String>(keyYNode) || isA<Null>(keyYNode) ||
        isA<Boolean>(keyYNode) || isA<Number>(keyYNode) ||
        isA<Array>(keyYNode) || isA<Dictionary>(keyYNode)) {
      return true;
    }
    return false;
  } catch ([[maybe_unused]]std::exception &e) {
    return false;
  }
}
/// <summary>
/// Extract YAML used in possible key value from source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>YAML for key value.</returns>
std::string YAML_Parser::extractKey(ISource &source) {
  std::string key;
  if (isInlineArray(source)) {
    key = extractToNext(source, {':', kLineFeed});
  } else if (isInlineDictionary(source)) {
    key = extractToNext(source, {'}', kLineFeed});
    if (source.current() == '}') {
      key += '}';
      source.next();
      source.ignoreWS();
    }
  } else {
    key = extractToNext(source, {':', ',', kLineFeed});
  }
  return key;
}
/// <summary>
/// Has a dictionary key been found in the source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>== true if a dictionary key has been found.</returns>
bool YAML_Parser::isKey(ISource &source) {
  bool keyPresent{false};
  std::string key{extractKey(source)};
  auto keyLength = key.size();
  if (source.current() == ':') {
    source.next();
    if ((source.current() == ' ') || (source.current() == kLineFeed)) {
      rightTrim(key);
      keyPresent = isValidKey(key);
    }
    keyLength++;
  }
  source.backup(keyLength);
  return keyPresent;
}
/// <summary>
/// Has an array element been found in the source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>==true if an array element has been found.</returns>
bool YAML_Parser::isArray(ISource &source) {
  auto ch = source.current();
  auto arrayPresent{false};
  if (source.more() && ch == '-') {
    source.next();
    ch = source.current();
    arrayPresent = ch == kSpace || ch == kLineFeed;
    source.backup(1);
  }
  return (arrayPresent);
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isBoolean(ISource &source) {
  auto ch = source.current();
  return ch == 'T' || ch == 'F' || ch == 'O' || ch == 'Y' || ch == 'N';
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isQuotedString(ISource &source) {
  auto ch = source.current();
  return (ch == '\'') || (ch == '"');
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isNumber(ISource &source) {
  auto ch = source.current();
  return (ch >= '0' && ch <= '9') || ch == '-' || ch == '+';
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isNone(ISource &source) {
  auto second = source.current();
  return second == 'n' || second == '~';
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isBlockString(ISource &source) {
  return source.current() == '>';
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isPipedBlockString(ISource &source) {
  return source.current() == '|';
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isComment(ISource &source) { return source.current() == '#'; }
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isAnchor(ISource &source) { return source.current() == '&'; }
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isAlias(ISource &source) { return source.current() == '*'; }
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isInlineArray(ISource &source) {
  return source.current() == '[';
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isDefault([[maybe_unused]] ISource &source) { return true; }

bool YAML_Parser::isInlineDictionary(ISource &source) {
  return source.current() == '{';
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isDictionary(ISource &source) { return isKey(source); }

bool YAML_Parser::isDocumentStart(ISource &source) {
  bool isStart{source.match("---")};
  if (isStart) {
    source.backup(3);
  }
  return isStart;
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
bool YAML_Parser::isDocumentEnd(ISource &source) {
  bool isEnd{source.match("...")};
  if (isEnd) {
    source.backup(3);
  }
  return isEnd;
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="yamlString"></param>
void YAML_Parser::foldCarriageReturns(ISource &source,
                                      std::string &yamlString) {
  yamlString += kSpace;
  source.next();
  source.ignoreWS();
  if (source.current() == kLineFeed) {
    yamlString.pop_back();
    yamlString += kLineFeed;
    source.next();
    source.ignoreWS();
  }
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
YAML_Parser::BlockChomping YAML_Parser::parseBlockChomping(ISource &source) {
  source.next();
  auto ch = source.current();
  if (ch == '-') {
    return BlockChomping::strip;
  } else if (ch == '+') {
    return BlockChomping::keep;
  } else {
    return BlockChomping::clip;
  }
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <param name="fillerDefault"></param>
/// <param name="chomping"></param>
/// <returns></returns>
std::string YAML_Parser::parseBlockString(ISource &source,
                                          const Delimeters &delimiters,
                                          char fillerDefault,
                                          BlockChomping &chomping) {
  unsigned long blockIndent = source.getIndentation();
  std::string yamlString{};
  do {
    char filler{fillerDefault};
    if (blockIndent < source.getIndentation()) {
      if (yamlString.back() != kLineFeed) {
        yamlString += kLineFeed;
      }
      yamlString += std::string((source.getIndentation() - 1), kSpace);
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
  } while (source.more() && blockIndent <= source.getIndentation());
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
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
YNode YAML_Parser::parseKey(ISource &source) {
  std::string key{extractKey(source)};
  if (source.more()) {
    source.next();
  }
  rightTrim(key);
  if (!isValidKey(key)) {
    throw SyntaxError(source.getPosition(),
                      "Invalid key '" + key + "' specified.");
  }
  BufferSource keyYAML{key + kLineFeed};
  auto keyYNode = parseDocument(keyYAML, {kLineFeed});
  std::string keyString;
  char quote = '\"';
  if (isA<String>(keyYNode)) {
    keyString = YRef<String>(keyYNode).value();
    quote = YRef<String>(keyYNode).getQuote();
  } else if (isA<Null>(keyYNode)) {
    keyString = "";
  } else if (isA<Boolean>(keyYNode)) {
    keyString = YRef<Boolean>(keyYNode).value() ? "true" : "false";
  } else if (isA<Number>(keyYNode)) {
    keyString = YRef<Number>(keyYNode).toString();
  } else if (isA<Array>(keyYNode)) {
    BufferDestination destination;
    YAML_Stringify::setInlineMode(true);
    YAML_Stringify::stringifyToString(destination, keyYNode, 0);
    keyString = destination.toString();
    YAML_Stringify::setInlineMode(false);
  } else if (isA<Dictionary>(keyYNode)) {
    BufferDestination destination;
    YAML_Stringify::setInlineMode(true);
    YAML_Stringify::stringifyToString(destination, keyYNode, 0);
    keyString = destination.toString();
    YAML_Stringify::setInlineMode(false);
  } else {
    throw SyntaxError("Invalid key specified.");
  }
  return YNode::make<String>(keyString, quote, 0);
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseFoldedBlockString(ISource &source,
                                          const Delimeters &delimiters) {
  BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  if (source.current() == '#') {
    parseComment(source, delimiters);
    moveToNextIndent(source);
  }
  auto blockIndent = source.getIndentation();
  std::string yamlString{
      parseBlockString(source, delimiters, kSpace, chomping)};
  return YNode::make<String>(yamlString, '>', blockIndent);
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseLiteralBlockString(ISource &source,
                                           const Delimeters &delimiters) {
  BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  if (source.current() == '#') {
    parseComment(source, delimiters);
    moveToNextIndent(source);
  }
  auto blockIndent = source.getIndentation();
  std::string yamlString{
      parseBlockString(source, delimiters, kLineFeed, chomping)};
  return YNode::make<String>(yamlString, '|', blockIndent);
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parsePlainFlowString(ISource &source,
                                        const Delimeters &delimiters) {
  std::string yamlString{extractToNext(source, delimiters)};
  if (source.current() != kLineFeed) {
    rightTrim(yamlString);
    return YNode::make<String>(yamlString, '\0');
  } else {
    while (source.more() &&
           !(isKey(source) || isArray(source) || isComment(source) ||
             isDocumentStart(source) || isDocumentEnd(source))) {
      if (source.current() == kLineFeed) {
        foldCarriageReturns(source, yamlString);
      } else {
        yamlString += source.current();
        source.next();
      }
    }
    if (yamlString.back() == kSpace || yamlString.back() == kLineFeed) {
      yamlString.pop_back();
    }
    return YNode::make<String>(yamlString, '\0');
  }
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseQuotedFlowString(ISource &source,
                                         const Delimeters &delimiters) {
  YAML_Translator translator;
  const char quote = source.current();
  std::string yamlString;
  source.next();
  if (quote == '"') {
    while (source.more() && source.current() != '"') {
      if (source.current() == '\\') {
        yamlString += "\\";
        source.next();
        yamlString += source.current();
        source.next();
      } else if (source.current() == kLineFeed) {
        foldCarriageReturns(source, yamlString);
      } else {
        yamlString += source.current();
        source.next();
      }
    }
    yamlString = translator.from(yamlString);
  } else {
    while (source.more()) {
      if (source.current() == '\'') {
        source.next();
        if (source.current() == '\'') {
          yamlString += '\'';
          source.next();
        } else {
          break;
        }
      } else if (source.current() == kLineFeed) {
        foldCarriageReturns(source, yamlString);
      } else {
        yamlString += source.current();
        source.next();
      }
    }
  }
  moveToNext(source, delimiters);
  return YNode::make<String>(yamlString, quote);
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseComment(ISource &source,
                                [[maybe_unused]] const Delimeters &delimiters) {
  source.next();
  std::string comment{extractToNext(source, {kLineFeed})};
  if (source.more()) {
    source.next();
  }
  return (YNode::make<Comment>(comment));
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseNumber(ISource &source, const Delimeters &delimiters) {
  YNode yNode;
  std::string numeric{extractToNext(source, delimiters)};
  unsigned long len = numeric.size();
  rightTrim(numeric);
  if (Number number{numeric}; number.is<int>() || number.is<long>() ||
                              number.is<long long>() || number.is<float>() ||
                              number.is<double>() || number.is<long double>()) {
    moveToNext(source, delimiters);
    yNode = YNode::make<Number>(number);
  }
  if (yNode.isEmpty()) {
    source.backup(len);
  }
  return yNode;
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseNone(ISource &source, const Delimeters &delimiters) {
  YNode yNode;
  std::string none{extractToNext(source, delimiters)};
  auto len = none.size();
  rightTrim(none);
  if (none == "null" || none == "~") {
    yNode = YNode::make<Null>();
  }
  if (yNode.isEmpty()) {
    source.backup(len);
  }
  return yNode;
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseBoolean(ISource &source, const Delimeters &delimiters) {
  YNode yNode;
  std::string boolean{extractToNext(source, delimiters)};
  auto len = boolean.size();
  rightTrim(boolean);
  if (Boolean::isTrue.contains(boolean)) {
    yNode = YNode::make<Boolean>(true, boolean);
  } else if (Boolean::isFalse.contains(boolean)) {
    yNode = YNode::make<Boolean>(false, boolean);
  }
  if (yNode.isEmpty()) {
    source.backup(len);
  }
  return yNode;
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseAnchor(ISource &source, const Delimeters &delimiters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  std::string unparsed{extractToNext(source, {kLineFeed})};
  YAML_Parser::yamlAliasMap[name] = unparsed;
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, delimiters);
  return (YNode::make<Anchor>(name, unparsed, parsed));
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseAlias(ISource &source, const Delimeters &delimiters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  std::string unparsed{YAML_Parser::yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, delimiters);
  return (YNode::make<Alias>(name, parsed));
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseArray(ISource &source, const Delimeters &delimiters) {
  unsigned long arrayIndent = source.getIndentation();
  YNode yNode = YNode::make<Array>(arrayIndent);
  while (source.more()) {
    if (isArray(source)) {
      source.next();
      source.next();
      YRef<Array>(yNode).add(parseDocument(source, delimiters));
    } else if (isComment(source)) {
      parseComment(source, delimiters);
    } else {
      break;
    }
    moveToNextIndent(source);
    if (arrayIndent > source.getIndentation()) {
      break;
    }
  }

  return yNode;
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseInlineArray(
    ISource &source, [[maybe_unused]] const Delimeters &delimiters) {
  unsigned long arrayIndent = source.getIndentation();
  Delimeters inLineArrayDelimiters = {delimiters};
  inLineArrayDelimiters.insert({',', ']'});
  YNode yNode = YNode::make<Array>(arrayIndent);
  do {
    source.next();
    moveToNextIndent(source);
    YRef<Array>(yNode).add(parseDocument(source, inLineArrayDelimiters));
  } while (source.current() == ',');
  moveToNextIndent(source);
  if (source.current() != ']') {
    throw SyntaxError(source.getPosition(),
                      "Missing closing ']' in array definition.");
  }
  source.next();
  return yNode;
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <param name="inlineDictionary"></param>
/// <returns></returns>
DictionaryEntry YAML_Parser::parseKeyValue(ISource &source,
                                           const Delimeters &delimiters,
                                           bool inlineDictionary) {
  unsigned long keyIndent = source.getIndentation();
  YNode keyYNode = parseKey(source);
  source.ignoreWS();
  if (!isInlineDictionary(source) && !(inlineDictionary)) {
    if (isKey(source)) {
      throw SyntaxError("Only an inline/compact dictionary is allowed.");
    }
    moveToNextIndent(source);
    while (isComment(source)) {
      parseComment(source, delimiters);
      moveToNextIndent(source);
    }
  }
  if (source.getIndentation() > keyIndent) {
    return DictionaryEntry(keyYNode, parseDocument(source, delimiters));
  } else {
    return DictionaryEntry(YRef<String>(keyYNode).value(), YNode::make<Null>());
  }
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseDictionary(ISource &source,
                                   const Delimeters &delimiters) {
  unsigned long dictionaryIndent = source.getIndentation();
  YNode yNode = YNode::make<Dictionary>(dictionaryIndent);
  while (source.more()) {
    if (isKey(source)) {
      auto entry = parseKeyValue(source, delimiters, false);
      if (YRef<Dictionary>(yNode).contains(entry.getKey())) {
        throw SyntaxError(source.getPosition(),
                          "Dictionary already contains key '" + entry.getKey() +
                              "'.");
      }
      YRef<Dictionary>(yNode).add(std::move(entry));
    } else if (isComment(source)) {
      parseComment(source, delimiters);
    } else if (isDocumentStart(source) || isDocumentEnd(source)) {
      break;
    } else {
      if (dictionaryIndent == source.getIndentation()) {
        throw SyntaxError(source.getPosition(),
                          "Missing key/value pair from indentation level.");
      }
      break;
    }
    moveToNextIndent(source);
    if (dictionaryIndent > source.getIndentation()) {
      return yNode;
    }
  }
  return (yNode);
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseInlineDictionary(
    ISource &source, [[maybe_unused]] const Delimeters &delimiters) {
  Delimeters inLineDictionaryDelimiters = {delimiters};
  inLineDictionaryDelimiters.insert({',', '}'});
  unsigned long dictionaryIndent = source.getIndentation();
  YNode yNode = YNode::make<Dictionary>(dictionaryIndent);
  do {
    source.next();
    moveToNextIndent(source);
    YRef<Dictionary>(yNode).add(
        parseKeyValue(source, inLineDictionaryDelimiters, true));

  } while (source.current() == ',');
  if (source.current() != '}') {
    throw SyntaxError(source.getPosition(),
                      "Missing closing '}' in object definition.");
  }
  source.next();
  return (yNode);
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns></returns>
YNode YAML_Parser::parseDocument(ISource &source,
                                 const Delimeters &delimiters) {
  YNode yNode;
  moveToNextIndent(source);
  for (const auto &parser : parsers) {
    if (parser.first(source)) {
      yNode = parser.second(source, delimiters);
      if (!yNode.isEmpty()) {
        return yNode;
      }
    }
  }
  throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
}
/// <summary>
/// 
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns></returns>
std::vector<YNode> YAML_Parser::parse(ISource &source) {
  std::vector<YNode> yNodeTree;
  for (bool inDocument = false; source.more();) {
    // Start of document
    if (isDocumentStart(source)) {
      inDocument = true;
      moveToNext(source, {kLineFeed, '|', '>'});
      yNodeTree.push_back(YNode::make<Document>());
      // End of document
    } else if (isDocumentEnd(source)) {
      moveToNext(source, {kLineFeed});
      if (!inDocument) {
        yNodeTree.push_back(YNode::make<Document>());
      }
      inDocument = false;
      // Inter document comment
    } else if (isComment(source) && !inDocument) {
      yNodeTree.push_back(parseComment(source, {}));
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