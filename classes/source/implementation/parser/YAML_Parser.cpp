
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"
#include <functional>

namespace YAML_Lib {

bool endsWith(const std::string &str, const std::string &sub) {
  int str_len = str.size();
  int sub_len = sub.size();
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

void moveToNext(ISource &source, const YAML_Parser::Delimeters &delimiters) {
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      source.next();
    }
  }
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

unsigned long currentIndentLevel(ISource &source) {
  return source.getPosition().second - 1;
}

bool isValidKey(const std::string &key) {
  if (!key.empty()) {
    if (!std::isalpha(key[0]) || key.back() == ' ') {
      return (false);
    }
    for (auto ch : key) {
      if ((ch != '-') && !(std::isalpha(ch) || std::isdigit(ch) || ch == ' ')) {
        return (false);
      }
    }
    return (true);
  }
  return (false);
}

bool isKey(ISource &source) {
  bool keyPresent{false};
  std::string key{extractToNext(source, {':', kLineFeed})};
  auto keyLength = key.size();
  if (source.current() == ':') {
    rightTrim(key);
    keyPresent = isValidKey(key);
  }
  source.backup(keyLength);
  return keyPresent;
}

bool isArray(ISource &source) {
  auto first = source.current();
  auto arrayPresent{false};
  if (source.more()) {
    source.next();
    arrayPresent = (first == '-') && source.current() == ' ';
    source.backup(1);
  }
  return (arrayPresent);
}

bool isBoolean(ISource &source) {
  return source.current() == 'T' || source.current() == 'F' ||
         source.current() == 'O' || source.current() == 'Y' ||
         source.current() == 'N';
}

bool isQuotedString(ISource &source) {
  return (source.current() == '\'') || (source.current() == '"');
}

bool isNumber(ISource &source) {
  return (source.current() >= '0' && source.current() <= '9') ||
         source.current() == '-' || source.current() == '+';
}

bool isNone(ISource &source) {
  return source.current() == 'n' || source.current() == '~';
}

bool isBlockString(ISource &source) { return source.current() == '>'; }

bool isPipedBlockString(ISource &source) { return source.current() == '|'; }

bool isComment(ISource &source) { return source.current() == '#'; }

bool isAnchor(ISource &source) { return source.current() == '&'; }

bool isAlias(ISource &source) { return source.current() == '*'; }

bool isInlineArray(ISource &source) { return source.current() == '['; }

bool isInlineDictionary(ISource &source) { return source.current() == '{'; }

bool isDictionary(ISource &source) { return isKey(source); }

YAML_Parser::BlockChomping YAML_Parser::parseBlockChomping(ISource &source) {
  source.next();
  if (source.current() == '-') {
    return BlockChomping::strip;
  } else if (source.current() == '+') {
    return BlockChomping::keep;
  } else {
    return BlockChomping::clip;
  }
}
std::string YAML_Parser::parseBlockString(ISource &source,
                                          const Delimeters &delimiters,
                                          char fillerDefault,
                                          unsigned long indentLevel,
                                          BlockChomping &chomping) {
  std::string yamlString{};
  do {
    char filler{fillerDefault};
    if (indentLevel < currentIndentLevel(source)) {
      if (yamlString.back() != '\n') {
        yamlString += '\n';
      }
      yamlString += std::string((currentIndentLevel(source) - 1), ' ');
      filler = '\n';
    }
    yamlString += extractToNext(source, delimiters);
    yamlString += filler;
    if (source.more()) {
      source.next();
    }
    while (source.more() && source.isWS()) {
      if (source.current() == '\n') {
        yamlString.pop_back();
        yamlString += "\n\n";
      }
      source.next();
    }
  } while (source.more() && indentLevel <= currentIndentLevel(source));
  if (chomping == BlockChomping::clip || chomping == BlockChomping::strip) {
    if (endsWith(yamlString, "\n\n\n")) {
      yamlString.pop_back();
    }
    yamlString.pop_back();
  }
  if (chomping == BlockChomping::strip && yamlString.back() == '\n') {
    yamlString.pop_back();
  }
  if (chomping == BlockChomping::keep && source.more() &&
      source.current() == '\n') {
    yamlString += '\n';
  }
  return yamlString;
}
std::string YAML_Parser::parseKey(ISource &source) {
  std::string key{extractToNext(source, {':'})};
  if (source.more()) {
    source.next();
  }
  rightTrim(key);
  if (!isValidKey(key)) {
    throw Error("Invalid key '" + key + "' specified.");
  }
  return key;
}

YNode YAML_Parser::parseFoldedBlockString(ISource &source,
                                          const Delimeters &delimiters) {
  BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  source.ignoreWS();
  auto indentLevel = currentIndentLevel(source);
  std::string yamlString{
      parseBlockString(source, delimiters, ' ', indentLevel, chomping)};
  return YNode::make<String>(yamlString, '>', indentLevel);
}

YNode YAML_Parser::parseLiteralBlockString(ISource &source,
                                           const Delimeters &delimiters) {
  BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  source.ignoreWS();
  auto indentLevel = currentIndentLevel(source);
  std::string yamlString{
      parseBlockString(source, delimiters, '\n', indentLevel, chomping)};
  return YNode::make<String>(yamlString, '|', indentLevel);
}

YNode YAML_Parser::parsePlainFlowString(ISource &source,
                                        const Delimeters &delimiters) {
  std::string yamlString{extractToNext(source, delimiters)};
  if (source.current() != kLineFeed) {
    return YNode::make<String>(yamlString, '\0');
  } else {
    while (source.more() &&
           !(isKey(source) || isArray(source) || isComment(source))) {
      if (source.current() == '\n') {
        yamlString += ' ';
        source.next();
        while (source.more() && source.current() == ' ') {
          source.next();
        }
        if (source.current() == '\n') {
          yamlString.pop_back();
          yamlString += "\n";
          source.next();
          while (source.more() && source.current() == ' ') {
            source.next();
          }
        }
      } else {
        yamlString += source.current();
        source.next();
      }
    }
    if (yamlString.back() == ' ' || yamlString.back() == '\n') {
      yamlString.pop_back();
    };
    return YNode::make<String>(yamlString, '\0');
  }
}

YNode YAML_Parser::parseQuotedFlowString(ISource &source,
                                         const Delimeters &delimiters) {
  const char quote = source.current();
  std::string yamlString;
  source.next();
  if (quote == '"') {
    while (source.more()) {
      if (source.current() == '\\') {
        yamlString += "\\";
        source.next();
        yamlString += source.current();
        source.next();
      } else if (source.current() == '\n') {
        yamlString += ' ';
        source.next();
        while (source.more() && source.current() == ' ') {
          source.next();
        }
        if (source.current() == '\n') {
          yamlString.pop_back();
          yamlString += "\n";
          source.next();
          while (source.more() && source.current() == ' ') {
            source.next();
          }
        }
      } else if (source.current() == '"') {
        break;
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
          yamlString += "\'";
          source.next();
        } else {
          break;
        }
      } else if (source.current() == '\n') {
        yamlString += ' ';
        source.next();
        while (source.more() && source.current() == ' ') {
          source.next();
        }
        if (source.current() == '\n') {
          yamlString.pop_back();
          yamlString += "\n";
          source.next();
          while (source.more() && source.current() == ' ') {
            source.next();
          }
        }
      } else {
        yamlString += source.current();
        source.next();
      }
    }
  }
  moveToNext(source, delimiters);
  source.ignoreWS();
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
    source.ignoreWS();
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
  const std::set<std::string> isTrue{"True", "On", "Yes"};
  const std::set<std::string> isFalse{"False", "Off", "No"};
  auto len = boolean.size();
  rightTrim(boolean);
  if (isTrue.contains(boolean)) {
    yNode = YNode::make<Boolean>(true);
  } else if (isFalse.contains(boolean)) {
    yNode = YNode::make<Boolean>(false);
  }
  if (yNode.isEmpty()) {
    source.backup(len);
  }
  return yNode;
}

YNode YAML_Parser::parseAnchor(ISource &source, const Delimeters &delimiters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, ' '})};
  source.next();
  std::string unparsed{extractToNext(source, {kLineFeed})};
  YAML_Parser::yamlAliasMap[name] = unparsed;
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, 0, delimiters);
  return (YNode::make<Anchor>(name, unparsed, parsed));
}

YNode YAML_Parser::parseAlias(ISource &source, const Delimeters &delimiters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, ' '})};
  source.next();
  std::string unparsed{YAML_Parser::yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, 0, delimiters);
  return (YNode::make<Alias>(name, parsed));
}

YNode YAML_Parser::parseArray(ISource &source, unsigned long indentLevel,
                              const Delimeters &delimiters) {
  YNode yNode = YNode::make<Array>(indentLevel);
  do {
    if (isArray(source)) {
      source.next();
      source.next();
      YRef<Array>(yNode).add(parseDocument(source, indentLevel, delimiters));
    } else {
      parseComment(source, delimiters);
    }
    source.ignoreWS();
    if (indentLevel > currentIndentLevel(source)) {
      return yNode;
    }
  } while (isArray(source) || isComment(source));

  return yNode;
}

YNode YAML_Parser::parseInlineArray(
    ISource &source, unsigned long indentLevel,
    [[maybe_unused]] const Delimeters &delimiters) {
  YNode yNode = YNode::make<Array>();
  do {
    source.next();
    source.ignoreWS();
    YRef<Array>(yNode).add(parseDocument(source, indentLevel, delimiters));
  } while (source.current() == ',');
  source.ignoreWS();
  if (source.current() != ']') {
    throw SyntaxError(source.getPosition(),
                      "Missing closing ']' in array definition.");
  }
  source.next();
  return yNode;
}
DictionaryEntry YAML_Parser::parseKeyValue(ISource &source,
                                           unsigned long indentLevel,
                                           const Delimeters &delimiters) {
  std::string key{parseKey(source)};
  source.ignoreWS();
  return DictionaryEntry(key, parseDocument(source, indentLevel, delimiters));
}

YNode YAML_Parser::parseDictionary(ISource &source, unsigned long indentLevel,
                                   const Delimeters &delimiters) {
  YNode yNode = YNode::make<Dictionary>(indentLevel);
  while (source.more() &&
         (std::isalpha(source.current()) || isComment(source))) {
    if (!isComment(source)) {
      YRef<Dictionary>(yNode).add(
          parseKeyValue(source, indentLevel, delimiters));
    } else {
      parseComment(source, delimiters);
    }
    source.ignoreWS();
    if (indentLevel > currentIndentLevel(source)) {
      return yNode;
    }
  }
  return (yNode);
}

YNode YAML_Parser::parseInlineDictionary(
    ISource &source, unsigned long indentLevel,
    [[maybe_unused]] const Delimeters &delimiters) {
  YNode yNode = YNode::make<Dictionary>();
  do {
    source.next();
    source.ignoreWS();
    YRef<Dictionary>(yNode).add(parseKeyValue(source, indentLevel, delimiters));

  } while (source.current() == ',');
  if (source.current() != '}') {
    throw SyntaxError(source.getPosition(),
                      "Missing closing '}' in object definition.");
  }
  source.next();
  return (yNode);
}

YNode YAML_Parser::parseDocument(ISource &source,
                                 [[maybe_unused]] unsigned long indentLevel,
                                 const Delimeters &delimiters) {

  using isAFunc = std::function<bool(void)>;
  using ParseFunc = std::function<YNode(void)>;
  std::vector<std::pair<isAFunc, ParseFunc>> parsers = {
      {[&source]() { return isBoolean(source); },
       [&source, &delimiters]() { return parseBoolean(source, delimiters); }},
      {[&source]() { return isQuotedString(source); },
       [&source, &delimiters]() {
         return parseQuotedFlowString(source, delimiters);
       }},
      {[&source]() { return isNumber(source); },
       [&source, &delimiters]() { return parseNumber(source, delimiters); }},
      {[&source]() { return isNone(source); },
       [&source, &delimiters]() { return parseNone(source, delimiters); }},
      {[&source]() { return isBlockString(source); },
       [&source, &delimiters]() {
         return parseFoldedBlockString(source, delimiters);
       }},
      {[&source]() { return isPipedBlockString(source); },
       [&source, &delimiters]() {
         return parseLiteralBlockString(source, delimiters);
       }},
      {[&source]() { return isComment(source); },
       [&source, &delimiters]() { return parseComment(source, delimiters); }},
      {[&source]() { return isAnchor(source); },
       [&source, &delimiters]() { return parseAnchor(source, delimiters); }},
      {[&source]() { return isAlias(source); },
       [&source, &delimiters]() { return parseAlias(source, delimiters); }},
      {[&source]() { return isArray(source); },
       [&source, &delimiters]() {
         return parseArray(source, currentIndentLevel(source), delimiters);
       }},
      {[&source]() { return isInlineArray(source); },
       [&source, &delimiters]() {
         return parseInlineArray(source, currentIndentLevel(source),
                                 {kLineFeed, ',', ']'});
       }},
      {[&source]() { return isDictionary(source); },
       [&source, &delimiters]() {
         return parseDictionary(source, currentIndentLevel(source), delimiters);
       }},
      {[&source]() { return isInlineDictionary(source); },
       [&source, &delimiters]() {
         return parseInlineDictionary(source, currentIndentLevel(source),
                                      {kLineFeed, ',', '}'});
       }},
      {[&source]() { return true; },
       [&source, &delimiters]() {
         return parsePlainFlowString(source, delimiters);
       }},
  };

  YNode yNode;
  source.ignoreWS();

  for (auto parser : parsers) {
    if (parser.first()) {
      yNode = parser.second();
      if (!yNode.isEmpty()) {
        return yNode;
      }
    }
  }

  throw SyntaxError("Invalid YAML.");
}

std::vector<YNode> YAML_Parser::parse(ISource &source) {
  std::vector<YNode> yNodeTree;
  while (source.more()) {
    for (bool inDocument = false; source.more();) {
      // Start of document
      if (source.match("---")) {
        inDocument = true;
        moveToNext(source, {kLineFeed, '|', '>'});
        source.ignoreWS();
        yNodeTree.push_back(YNode::make<Document>());
        // End of document
      } else if (source.match("...")) {
        moveToNext(source, {kLineFeed});
        source.ignoreWS();
        if (!inDocument) {
          yNodeTree.push_back(YNode::make<Document>());
        }
        break;
      } else if (isComment(source) && !inDocument) {
        yNodeTree.push_back(parseComment(source, {}));
      } else {
        if (yNodeTree.empty()) {
          yNodeTree.push_back(YNode::make<Document>());
        }
        YRef<Document>(yNodeTree.back())
            .add(parseDocument(source, 0, {kLineFeed}));
      }
    }
  }
  return yNodeTree;
}
} // namespace YAML_Lib