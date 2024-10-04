
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

void moveToNext(ISource &source, const std::set<char> &delimeters) {
  if (!delimeters.empty()) {
    while (source.more() && !delimeters.contains(source.current())) {
      source.next();
    }
  }
  source.ignoreWS();
}

std::string extractToNext(ISource &source, const std::set<char> &delimeters) {
  std::string extracted;
  if (!delimeters.empty()) {
    while (source.more() && !delimeters.contains(source.current())) {
      extracted += source.current();
      source.next();
    }
  }
  return (extracted);
}

bool endOfNumber(const ISource &source) {
  return source.isWS() || source.current() == ',' || source.current() == ']' ||
         source.current() == '}';
}

unsigned long currentIndentLevel(ISource &source) {
  return source.getPosition().second - 1;
}

std::string translateEscapes(const std::string &yamlString) {
  std::string translated;
  for (std::size_t idx = 0; idx < yamlString.size(); idx++) {
    if (yamlString[idx] == '\\') {
      idx++;
      if (yamlString[idx] == 't') {
        translated += "\t";
      } else if (yamlString[idx] == 'n') {
        translated += "\n";
      } else if (yamlString[idx] == '"') {
        translated += "\"";
      } else if (yamlString[idx] == 'b') {
        translated += "\b";
      } else if (yamlString[idx] == 'r') {
        translated += "\r";
      } else if (yamlString[idx] == 'f') {
        translated += "\f";
      } else if (yamlString[idx] == '\\') {
        translated += "\\";
      } else {
        translated += yamlString[idx];
      }
    } else {
      translated += yamlString[idx];
    }
  }
  return (translated);
}
bool isValidKey(const std::string &key) {
  if (!key.empty()) {
    if (!std::isalpha(key[0])) {
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
  std::string key{extractToNext(source, {':', kLineFeed})};
  if (source.current() == ':') {
    source.backup(key.size());
    if (!key.empty()) {
      while (key.back() == ' ') {
        key.pop_back();
      }
    }
    return isValidKey(key);
  }
  source.backup(key.size());
  return false;
}

bool isArray(ISource &source) {
  if (source.match("- ")) {
    source.backup(2);
    return (true);
  }
  return (false);
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

std::string YAML_Parser::parseKey(ISource &source) {
  std::string key{extractToNext(source, {':'})};
  if (source.more()) {
    source.next();
  }
  if (!key.empty()) {
    while (key.back() == ' ') {
      key.pop_back();
    }
  }
  if (!isValidKey(key)) {
    throw Error("Invalid key '" + key + "' specified.");
  }
  return key;
}

YNode YAML_Parser::parseBlockString(ISource &source,
                                    const std::set<char> &delimeters) {
  YNode yNode;
  moveToNext(source, {kLineFeed});
  auto indentLevel = currentIndentLevel(source);
  std::string yamlString{};
  while (indentLevel == currentIndentLevel(source)) {
    yamlString += extractToNext(source, delimeters);
    moveToNext(source, delimeters);
    if (indentLevel == currentIndentLevel(source)) {
      yamlString += " ";
    }
  }
  yNode = YNode::make<String>(yamlString, '>');
  YRef<String>(yNode).setIndentation(indentLevel);
  return yNode;
}

YNode YAML_Parser::parsePipedBlockString(ISource &source,
                                         const std::set<char> &delimeters) {
  YNode yNode;
  moveToNext(source, delimeters);
  auto indentLevel = currentIndentLevel(source);
  std::string yamlString;
  while (indentLevel == currentIndentLevel(source)) {
    yamlString += extractToNext(source, {kLineFeed});
    moveToNext(source, delimeters);
    if (indentLevel == currentIndentLevel(source)) {
      yamlString += kLineFeed;
    }
  }
  yNode = YNode::make<String>(yamlString, '|');
  YRef<String>(yNode).setIndentation(indentLevel);
  return yNode;
}

YNode YAML_Parser::parseString(ISource &source,
                               const std::set<char> &delimeters) {
  std::string yamlString{extractToNext(source, delimeters)};
  if (source.current() != kLineFeed) {
    return YNode::make<String>(yamlString, '\0');
  } else {
    source.ignoreWS();
    while (source.more() && !isKey(source) && !isArray(source) &&
           !isComment(source)) {
      yamlString += " ";
      yamlString += extractToNext(source, delimeters);
      if (source.more()) {
        source.next();
      }
      source.ignoreWS();
    }
    return YNode::make<String>(yamlString, '\0');
  }
}

YNode YAML_Parser::parseQuotedString(ISource &source,
                                     const std::set<char> &delimeters) {
  const char quote = source.current();
  std::string yamlString;
  source.next();
  while (source.more() && source.current() != quote) {
    if (source.current() == '\\') {
      yamlString += "\\";
      source.next();
    }
    if (source.more()) {
      yamlString += source.current();
      source.next();
    }
  }
  if (quote != '\'') {
    yamlString = translateEscapes(yamlString);
  }
  moveToNext(source, delimeters);
  return YNode::make<String>(yamlString, quote);
}

YNode YAML_Parser::parseComment(ISource &source,
                                const std::set<char> &delimeters) {
  source.next();
  std::string comment{extractToNext(source, {kLineFeed})};
  if (source.more()) {
    source.next();
  }
  return (YNode::make<Comment>(comment));
}

YNode YAML_Parser::parseNumber(ISource &source,
                               const std::set<char> &delimeters) {
  YNode yNode;
  std::string string{extractToNext(source, delimeters)};
  unsigned long len = string.size();
  while (string.back() == ' ') {
    string.pop_back();
  }
  if (Number number{string}; number.is<int>() || number.is<long>() ||
                             number.is<long long>() || number.is<float>() ||
                             number.is<double>() || number.is<long double>()) {
    moveToNext(source, delimeters);
    yNode = YNode::make<Number>(number);
  } else {
    source.backup(len);
  }
  return yNode;
}

YNode YAML_Parser::parseNone(ISource &source,
                             const std::set<char> &delimeters) {
  YNode yNode;
  std::string none{extractToNext(source, delimeters)};
  int len = none.size();
  while (none.back() == ' ') {
    none.pop_back();
  }
  if (none == "null" || none == "~") {
    yNode = YNode::make<Null>();
  }
  if (yNode.isEmpty()) {
    source.backup(len);
  }
  return yNode;
}

YNode YAML_Parser::parseBoolean(ISource &source,
                                const std::set<char> &delimeters) {
  YNode yNode;
  std::string boolean{extractToNext(source, delimeters)};
  int len = boolean.size();
  while (boolean.back() == ' ') {
    boolean.pop_back();
  }
  if (boolean == "True" || boolean == "On" || boolean == "Yes") {
    yNode = YNode::make<Boolean>(true);
  } else if (boolean == "False" || boolean == "Off" || boolean == "No") {
    yNode = YNode::make<Boolean>(false);
  }
  if (yNode.isEmpty()) {
    source.backup(len);
  }
  return yNode;
}

YNode YAML_Parser::parseAnchor(ISource &source,
                               const std::set<char> &delimeters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, ' '})};
  source.next();
  std::string unparsed{extractToNext(source, {kLineFeed})};
  YAML_Parser::yamlAliasMap[name] = unparsed;
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, 0, delimeters);
  return (YNode::make<Anchor>(name, unparsed, parsed));
}

YNode YAML_Parser::parseAlias(ISource &source,
                              const std::set<char> &delimeters) {
  source.next();
  std::string name{extractToNext(source, {kLineFeed, ' '})};
  source.next();
  std::string unparsed{YAML_Parser::yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, 0, delimeters);
  return (YNode::make<Alias>(name, parsed));
}

YNode YAML_Parser::parseArray(ISource &source, unsigned long indentLevel,
                              const std::set<char> &delimeters) {
  YNode yNode;
  source.next();
  if (source.current() == ' ') {
    source.next();
    yNode = YNode::make<Array>();
    YRef<Array>(yNode).setIndentation(indentLevel);
    do {
      if (source.current() != '#') {
        YRef<Array>(yNode).add(parseDocument(source, indentLevel, delimeters));
      } else {
        parseComment(source, delimeters);
      }
      source.ignoreWS();
      if (indentLevel > currentIndentLevel(source)) {
        return yNode;
      }
    } while (source.match("- ") || source.current() == '#');
  } else {
    source.backup(1);
  }
  return yNode;
}

YNode YAML_Parser::parseInlineArray(ISource &source, unsigned long indentLevel,
                                    const std::set<char> &delimeters) {
  YNode yNode = YNode::make<Array>();
  if (source.current() != ']') {
    do {
      source.next();
      source.ignoreWS();
      YRef<Array>(yNode).add(
          parseDocument(source, indentLevel, {kLineFeed, ',', ']'}));
    } while (source.current() == ',');
    source.ignoreWS();
  }
  if (source.current() != ']') {
    throw SyntaxError(source.getPosition(),
                      "Missing closing ']' in array definition.");
  }
  source.next();
  return yNode;
}
DictionaryEntry YAML_Parser::parseKeyValue(ISource &source,
                                           unsigned long indentLevel,
                                           const std::set<char> &delimeters) {
  std::string key{parseKey(source)};
  source.ignoreWS();
  return DictionaryEntry(key, parseDocument(source, indentLevel, delimeters));
}

YNode YAML_Parser::parseDictionary(ISource &source, unsigned long indentLevel,
                                   const std::set<char> &delimeters) {
  YNode yNode = YNode::make<Dictionary>();
  YRef<Dictionary>(yNode).setIndentation(indentLevel);
  while (source.more() &&
         (std::isalpha(source.current()) || source.current() == '#')) {
    if (source.current() != '#') {
      YRef<Dictionary>(yNode).add(
          parseKeyValue(source, indentLevel, delimeters));
    } else {
      parseComment(source, delimeters);
    }
    source.ignoreWS();
    if (indentLevel > currentIndentLevel(source)) {
      return yNode;
    }
  }
  return (yNode);
}

YNode YAML_Parser::parseInlineDictionary(ISource &source,
                                         unsigned long indentLevel,
                                         const std::set<char> &delimeters) {
  YNode yNode = YNode::make<Dictionary>();
  if (source.current() != '}') {
    do {
      source.next();
      source.ignoreWS();
      YRef<Dictionary>(yNode).add(
          parseKeyValue(source, indentLevel, {kLineFeed, ',', '}'}));

    } while (source.current() == ',');
  }
  if (source.current() != '}') {
    throw SyntaxError(source.getPosition(),
                      "Missing closing '}' in object definition.");
  }
  source.next();
  return (yNode);
}

YNode YAML_Parser::parseDocument(ISource &source, unsigned long indentLevel,
                                 const std::set<char> &delimeters) {
  YNode yNode;
  source.ignoreWS();

  // Parse scalars

  if (isBoolean(source)) {
    yNode = parseBoolean(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isQuotedString(source)) {
    yNode = parseQuotedString(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isNumber(source)) {
    yNode = parseNumber(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isNone(source)) {
    yNode = parseNone(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isBlockString(source)) {
    yNode = parseBlockString(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isPipedBlockString(source)) {
    yNode = parsePipedBlockString(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isComment(source)) {
    yNode = parseComment(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isAnchor(source)) {
    yNode = parseAnchor(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isAlias(source)) {
    yNode = parseAlias(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isArray(source)) {
    yNode = parseArray(source, currentIndentLevel(source), delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isInlineArray(source)) {
    yNode = parseInlineArray(source, currentIndentLevel(source), {','});
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isDictionary(source)) {
    yNode = parseDictionary(source, currentIndentLevel(source), delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isInlineDictionary(source)) {
    yNode = parseInlineDictionary(source, currentIndentLevel(source), {','});
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  yNode = parseString(source, delimeters);
  if (!yNode.isEmpty()) {
    return yNode;
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
        inDocument = false;
        break;
      } else if (source.current() == '#' && !inDocument) {
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
