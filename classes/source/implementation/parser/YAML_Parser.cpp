
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
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

void moveToNext(ISource &source, const YAML_Parser::Delimeters &delimiters) {
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      source.next();
    }
  }
  source.ignoreWS();
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

bool YAML_Parser::isValidKey(const std::string &key) {
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

bool YAML_Parser::isKey(ISource &source) {
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

bool YAML_Parser::isArray(ISource &source) {
  auto first = source.current();
  auto arrayPresent{false};
  if (source.more()) {
    source.next();
    arrayPresent = (first == '-') && source.current() == ' ';
    source.backup(1);
  }
  return (arrayPresent);
}

bool YAML_Parser::isBoolean(ISource &source) {
  return source.current() == 'T' || source.current() == 'F' ||
         source.current() == 'O' || source.current() == 'Y' ||
         source.current() == 'N';
}

bool YAML_Parser::isQuotedString(ISource &source) {
  return (source.current() == '\'') || (source.current() == '"');
}

bool YAML_Parser::isNumber(ISource &source) {
  return (source.current() >= '0' && source.current() <= '9') ||
         source.current() == '-' || source.current() == '+';
}

bool YAML_Parser::isNone(ISource &source) {
  return source.current() == 'n' || source.current() == '~';
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

bool YAML_Parser::isDefault([[
    maybe_unused]]ISource &source) { return true; }

bool YAML_Parser::isInlineDictionary(ISource &source) {
  return source.current() == '{';
}

bool YAML_Parser::isDictionary(ISource &source) { return isKey(source); }

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
                                          BlockChomping &chomping) {
  unsigned long blockIndent = currentIndentLevel(source);
  std::string yamlString{};
  do {
    char filler{fillerDefault};
    if (blockIndent < currentIndentLevel(source)) {
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
  } while (source.more() && blockIndent <= currentIndentLevel(source));
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
  source.ignoreWS();
  return key;
}

YNode YAML_Parser::parseFoldedBlockString(ISource &source,
                                          const Delimeters &delimiters) {
  BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  auto blockIndent = currentIndentLevel(source);
  std::string yamlString{parseBlockString(source, delimiters, ' ', chomping)};
  return YNode::make<String>(yamlString, '>', blockIndent);
}

YNode YAML_Parser::parseLiteralBlockString(ISource &source,
                                           const Delimeters &delimiters) {
  BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  auto blockIndent = currentIndentLevel(source);
  std::string yamlString{parseBlockString(source, delimiters, '\n', chomping)};
  return YNode::make<String>(yamlString, '|', blockIndent);
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
    }
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
  YNode parsed = parseDocument(anchor, delimiters);
  return (YNode::make<Anchor>(name, unparsed, parsed));
}

YNode YAML_Parser::parseAlias(ISource &source, const Delimeters &delimiters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, ' '})};
  source.next();
  std::string unparsed{YAML_Parser::yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, delimiters);
  return (YNode::make<Alias>(name, parsed));
}

YNode YAML_Parser::parseArray(ISource &source, const Delimeters &delimiters) {
  unsigned long arrayIndent = currentIndentLevel(source);
  YNode yNode = YNode::make<Array>(arrayIndent);
  do {
    if (isArray(source)) {
      source.next();
      source.next();
      YRef<Array>(yNode).add(parseDocument(source, delimiters));
    } else {
      parseComment(source, delimiters);
    }
    source.ignoreWS();
    if (arrayIndent > currentIndentLevel(source)) {
      return yNode;
    }
  } while (isArray(source) || isComment(source));

  return yNode;
}

YNode YAML_Parser::parseInlineArray(
    ISource &source, [[maybe_unused]] const Delimeters &delimiters) {
  Delimeters inLineArrayDelimiters = {delimiters};
  inLineArrayDelimiters.insert({',', ']'});
  YNode yNode = YNode::make<Array>();
  do {
    source.next();
    source.ignoreWS();
    YRef<Array>(yNode).add(parseDocument(source, inLineArrayDelimiters));
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
                                           const Delimeters &delimiters) {
  std::string key{parseKey(source)};
  return DictionaryEntry(key, parseDocument(source, delimiters));
}

YNode YAML_Parser::parseDictionary(ISource &source,
                                   const Delimeters &delimiters) {
  unsigned long dictionaryIndent = currentIndentLevel(source);
  YNode yNode = YNode::make<Dictionary>(dictionaryIndent);
  while (source.more() &&
         (std::isalpha(source.current()) || isComment(source))) {
    if (!isComment(source)) {
      YRef<Dictionary>(yNode).add(parseKeyValue(source, delimiters));
    } else {
      parseComment(source, delimiters);
    }
    source.ignoreWS();
    if (dictionaryIndent > currentIndentLevel(source)) {
      return yNode;
    }
  }
  return (yNode);
}

YNode YAML_Parser::parseInlineDictionary(
    ISource &source, [[maybe_unused]] const Delimeters &delimiters) {
  Delimeters inLineDictionaryDelimiters = {delimiters};
  inLineDictionaryDelimiters.insert({',', '}'});
  YNode yNode = YNode::make<Dictionary>();
  do {
    source.next();
    source.ignoreWS();
    YRef<Dictionary>(yNode).add(
        parseKeyValue(source, inLineDictionaryDelimiters));

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
  source.ignoreWS();

  for (const auto& parser : parsers) {
    if (parser.first(source)) {
      yNode = parser.second(source, delimiters);
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
        yNodeTree.push_back(YNode::make<Document>());
        // End of document
      } else if (source.match("...")) {
        moveToNext(source, {kLineFeed});
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
            .add(parseDocument(source, {kLineFeed}));
      }
    }
  }
  return yNodeTree;
}
} // namespace YAML_Lib