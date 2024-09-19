
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YNode parseDocument(ISource &source, unsigned long indentation,
                    const std::set<char> &delimeters);

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
  return source.isWS() || source.current() == ',' || source.current() == ']';
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
std::string parseKey(ISource &source) {
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

YNode parseBlockString(ISource &source, const std::set<char> &delimeters) {
  moveToNext(source, {kLineFeed});
  auto indentLevel = currentIndentLevel(source);
  std::string yamlString;
  while (indentLevel == currentIndentLevel(source)) {
    yamlString += extractToNext(source, delimeters);
    moveToNext(source, delimeters);
    if (indentLevel == currentIndentLevel(source)) {
      yamlString += " ";
    }
  }
  return YNode::make<String>(yamlString, ' ');
}

YNode parsePipedBlockString(ISource &source, const std::set<char> &delimeters) {
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
  return YNode::make<String>(yamlString, ' ');
}

YNode parseUnquotedString(ISource &source, const std::set<char> &delimeters) {
  std::string yamlString{extractToNext(source, delimeters)};
  return YNode::make<String>(yamlString, ' ');
}

YNode parseString(ISource &source, const std::set<char> &delimeters) {
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

YNode parseComment(ISource &source, const std::set<char> &delimeters) {
  source.next();
  std::string comment{extractToNext(source, {kLineFeed})};
  if (source.more()) {
    source.next();
  }
  return (YNode::make<Comment>(comment));
}

YNode parseNumber(ISource &source, const std::set<char> &delimeters) {
  YNode yNode;
  std::string string;
  for (; source.more() && !endOfNumber(source); source.next()) {
    string += source.current();
  }
  if (Number number{string}; number.is<int>() || number.is<long>() ||
                             number.is<long long>() || number.is<float>() ||
                             number.is<double>() || number.is<long double>()) {
    moveToNext(source, delimeters);
    yNode = YNode::make<Number>(number);
  } else {
    source.backup(1);
  }
  return yNode;
}

YNode parseNone(ISource &source, const std::set<char> &delimeters) {
  YNode yNode;
  if (source.match("null") || source.current() == '~') {
    moveToNext(source, delimeters);
    yNode = YNode::make<Null>();
  }
  return yNode;
}

YNode parseBoolean(ISource &source, const std::set<char> &delimeters) {
  YNode yNode;
  if (source.match("True") || source.match("On") || source.match("Yes")) {
    yNode = YNode::make<Boolean>(true);
  } else if (source.match("False") || source.match("Off") ||
             source.match("No")) {
    moveToNext(source, delimeters);
    yNode = YNode::make<Boolean>(false);
  }
  return yNode;
}

YNode parseArray(ISource &source, unsigned long indentLevel,
                 const std::set<char> &delimeters) {
  YNode yNode;
  source.next();
  if (source.current() == ' ') {
    source.next();
    yNode = YNode::make<Array>();
    do {
      YRef<Array>(yNode).add(parseDocument(source, indentLevel, delimeters));
      source.ignoreWS();
      if (indentLevel > currentIndentLevel(source)) {
        return yNode;
      }
    } while (source.match("- "));
  } else {
    source.backup(1);
  }
  return yNode;
}

YNode parseFlatArray(ISource &source, unsigned long indentLevel,
                     const std::set<char> &delimeters) {
  YNode yNode = YNode::make<Array>();
  if (source.current() != ']') {
    do {
      source.next();
      source.ignoreWS();
      YRef<Array>(yNode)
          .add(parseDocument(source, indentLevel, {',', ']'}));
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
DictionaryEntry parseKeyValue(ISource &source, unsigned long indentLevel,
                              const std::set<char> &delimeters) {
  std::string key{parseKey(source)};
  source.ignoreWS();
  return DictionaryEntry(key, parseDocument(source, indentLevel, delimeters));
}

YNode parseDictionary(ISource &source, unsigned long indentLevel,
                      const std::set<char> &delimeters) {
  YNode yNode = YNode::make<Dictionary>();
  while (source.more() && std::isalpha(source.current())) {
    YRef<Dictionary>(yNode).add(parseKeyValue(source, indentLevel, delimeters));
    source.ignoreWS();
    if (indentLevel > currentIndentLevel(source)) {
      return yNode;
    }
  }
  return (yNode);
}

YNode parseFlatDictionary(ISource &source, unsigned long indentLevel,
                          const std::set<char> &delimeters) {
  YNode yNode = YNode::make<Dictionary>();
  if (source.current() != '}') {
    do {
      source.next();
      source.ignoreWS();
      YRef<Dictionary>(yNode).add(
          parseKeyValue(source, indentLevel, {',', '}'}));
    } while (source.current() == ',');
  }
  if (source.current() != '}') {
    throw SyntaxError(source.getPosition(),
                      "Missing closing '}' in object definition.");
  }
  source.next();
  return (yNode);
}

YNode parseDocument(ISource &source, unsigned long indentLevel,
                    const std::set<char> &delimeters) {
  YNode yNode;
  source.ignoreWS();
  if (source.current() == 'T' || source.current() == 'F' ||
      source.current() == 'O' || source.current() == 'Y' ||
      source.current() == 'N') {
    yNode = parseBoolean(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if ((source.current() == '\'') || (source.current() == '"')) {
    yNode = parseString(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if ((source.current() >= '0' && source.current() <= '9') ||
      source.current() == '-' || source.current() == '+') {
    yNode = parseNumber(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '#') {
    yNode = parseComment(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '-') {
    yNode = parseArray(source, currentIndentLevel(source), delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '[') {
    yNode = parseFlatArray(source, currentIndentLevel(source), {','});
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == 'n' || source.current() == '~') {
    yNode = parseNone(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isKey(source)) {
    yNode = parseDictionary(source, currentIndentLevel(source), delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '{') {
    yNode = parseFlatDictionary(source, currentIndentLevel(source), {','});
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '>') {
    yNode = parseBlockString(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '|') {
    yNode = parsePipedBlockString(source, delimeters);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  yNode = parseUnquotedString(source, delimeters);
  if (!yNode.isEmpty()) {
    return yNode;
  }
  throw SyntaxError("Invalid YAML.");
}

void YAML_Impl::parse(ISource &source) {
  while (source.more()) {
    bool inDocument = false;
    unsigned int startNumberOfDocuments = getNumberOfDocuments();
    while (source.more()) {
      // Start of document
      if (source.match("---")) {
        inDocument = true;
        moveToNext(source, {kLineFeed});
        yamlYNodeTree.push_back(YNode::make<Document>());
        // End of document
      } else if (source.match("...")) {
        moveToNext(source, {kLineFeed});
        inDocument = false;
        if (startNumberOfDocuments == getNumberOfDocuments()) {
          yamlYNodeTree.push_back(YNode::make<Document>());
        }
        break;
      } else if (source.current() == '#' && !inDocument) {
        yamlYNodeTree.push_back(parseComment(source, {}));
      } else {
        if (yamlYNodeTree.empty()) {
          yamlYNodeTree.push_back(YNode::make<Document>());
        }
        YRef<Document>(yamlYNodeTree.back())
            .add(parseDocument(source, 0, {kLineFeed}));
      }
    }
  }
}

} // namespace YAML_Lib
