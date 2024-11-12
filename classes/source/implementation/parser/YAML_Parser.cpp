
//
// Class: YAML_Parser
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

bool endsWith(const std::string &str, const std::string &sub) {
  auto str_len = str.size();
  auto sub_len = sub.size();
  if (str_len < sub_len)
    return false;

  return str.compare(str_len - sub_len, sub_len, sub) == 0;
}

void rightTrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

void leftTrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

void moveToNextIndent(ISource &source) {
  while (source.more() && (source.isWS() || source.current() == kLineFeed)) {
    source.next();
  }
}

void moveToNext(ISource &source, const YAML_Parser::Delimeters &delimiters) {
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      source.next();
    }
  }
  moveToNextIndent(source);
}

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

bool isValid(char ch) {
  if ((ch == '{') || (ch == '[') || (ch == '}') || (ch == ']') || (ch == '&') ||
      (ch == '*') || (ch == '#')) {
    return false;
  }
  return true;
}

bool YAML_Parser::isValidKey(const std::string &key) {
  try {
    BufferSource keyYAML{key + kLineFeed};
    YNode keyYNode = parseDocument(keyYAML, {kLineFeed});
    if (isA<String>(keyYNode) || isA<Null>(keyYNode) ||
        isA<Boolean>(keyYNode) || isA<Number>(keyYNode) ||
        isA<Array>(keyYNode)) {
      return true;
    }
    return false;
  } catch (std::exception &e) {
    return false;
  }
}

bool YAML_Parser::isKey(ISource &source) {
  bool keyPresent{false};
  std::string key;
  if (isInlineArray(source) || isInlineDictionary(source)) {
    key = extractToNext(source, {':', kLineFeed});
  } else {
    key = extractToNext(source, {':', ',', kLineFeed});
  }
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

bool YAML_Parser::isBoolean(ISource &source) {
  auto ch = source.current();
  return ch == 'T' || ch == 'F' || ch == 'O' || ch == 'Y' || ch == 'N';
}

bool YAML_Parser::isQuotedString(ISource &source) {
  auto ch = source.current();
  return (ch == '\'') || (ch == '"');
}

bool YAML_Parser::isNumber(ISource &source) {
  auto ch = source.current();
  return (ch >= '0' && ch <= '9') || ch == '-' || ch == '+';
}

bool YAML_Parser::isNone(ISource &source) {
  auto second = source.current();
  return second == 'n' || second == '~';
}

bool YAML_Parser::isBlockString(ISource &source) {
  return source.current() == '>';
}

bool YAML_Parser::isPipedBlockString(ISource &source) {
  return source.current() == '|';
}

bool YAML_Parser::isComment(ISource &source) { return source.current() == '#'; }

bool YAML_Parser::isAnchor(ISource &source) { return source.current() == '&'; }

bool YAML_Parser::isAlias(ISource &source) { return source.current() == '*'; }

bool YAML_Parser::isInlineArray(ISource &source) {
  return source.current() == '[';
}

bool YAML_Parser::isDefault([[maybe_unused]] ISource &source) { return true; }

bool YAML_Parser::isInlineDictionary(ISource &source) {
  return source.current() == '{';
}

bool YAML_Parser::isDictionary(ISource &source) { return isKey(source); }

bool YAML_Parser::isDocumentStart(ISource &source) {
  bool isStart{source.match("---")};
  if (isStart) {
    source.backup(3);
  }
  return isStart;
}

bool YAML_Parser::isDocumentEnd(ISource &source) {
  bool isEnd{source.match("...")};
  if (isEnd) {
    source.backup(3);
  }
  return isEnd;
}

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

YNode YAML_Parser::parseKey(ISource &source) {
  std::string key;
  if (isInlineArray(source) || isInlineDictionary(source)) {
    key = extractToNext(source, {':', kLineFeed});
  } else {
    key = extractToNext(source, {':', ',', kLineFeed});
  }
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
    keyString = YRef<Boolean>(keyYNode).toString();
  } else if (isA<Number>(keyYNode)) {
    keyString = YRef<Number>(keyYNode).toString();
  } else if (isA<Array>(keyYNode)) {
    BufferDestination destination;
    YAML_Stringify::setInlineMode(true);
    YAML_Stringify::stringifyToString(destination, keyYNode, 0);
    keyString = destination.toString();
    YAML_Stringify::setInlineMode(false);
  }
  return YNode::make<String>(keyString, quote, 0);
}

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

YNode YAML_Parser::parsePlainFlowString(ISource &source,
                                        const Delimeters &delimiters) {
  std::string yamlString{extractToNext(source, delimiters)};
  if (source.current() != kLineFeed) {
    rightTrim(yamlString);
    for (auto ch : yamlString) {
      if (!isValid(ch)) {
        throw SyntaxError("Yummy");
      }
    }
    return YNode::make<String>(yamlString, '\0');
  } else {
    for (auto ch : yamlString) {
      if (!isValid(ch)) {
        throw SyntaxError("Yummy");
      }
    }
    while (source.more() && isValid(source.current()) &&
           !(isKey(source) || isArray(source) || isComment(source) ||
             isDocumentStart(source) || isDocumentEnd(source))) {
      if (source.current() == kLineFeed) {
        foldCarriageReturns(source, yamlString);
      } else {
        yamlString += source.current();
        source.next();
      }
    }
    if (!isValid(source.current())) {
      SyntaxError("Yummy");
    }
    if (yamlString.back() == kSpace || yamlString.back() == kLineFeed) {
      yamlString.pop_back();
    }
    return YNode::make<String>(yamlString, '\0');
  }
}

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

YNode YAML_Parser::parseComment(ISource &source,
                                [[maybe_unused]] const Delimeters &delimiters) {
  source.next();
  std::string comment{extractToNext(source, {kLineFeed})};
  if (source.more()) {
    source.next();
  }
  return (YNode::make<Comment>(comment));
}

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

YNode YAML_Parser::parseAlias(ISource &source, const Delimeters &delimiters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  std::string unparsed{YAML_Parser::yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, delimiters);
  return (YNode::make<Alias>(name, parsed));
}

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
DictionaryEntry YAML_Parser::parseKeyValue(ISource &source,
                                           const Delimeters &delimiters,
                                           bool inlineDictionary) {
  unsigned long keyIndent = source.getIndentation();
  YNode keyYNode = parseKey(source);
  source.ignoreWS();
  if (!isInlineDictionary(source) && !(inlineDictionary)) {
    auto ch = source.current();
    if (isKey(source)) {
      throw SyntaxError("Only an inline/compact dictionary is allowed.");
    }
    ch++;
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