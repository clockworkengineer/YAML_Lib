
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YNode parseDocument(ISource &source, unsigned long indentation);

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
      if ((ch != '-') && !std::isalpha(ch)) {
        return (false);
      }
    }
    return (true);
  }
  return (false);
}
bool isKey(ISource &source) {
  std::string key{};
  while (source.more() && source.current() != ':' &&
         source.current() != kLineFeed) {
    key += source.current();
    source.next();
  }
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
  std::string key{};
  while (source.more() && source.current() != ':') {
    key += source.current();
    source.next();
  }
  source.next();
  if (!key.empty()) {
    while (key.back() == ' ') {
      key.pop_back();
    }
  }
  if (!isValidKey(key)) {
    throw Error("Invalid key specified");
  }
  return key;
}

void moveToNextLine(ISource &source) {
  while (source.more() && source.current() != kLineFeed) {
    if (source.current() == '#') {
      return;
    }
    source.next();
  }
  if (source.more()) {
    source.next();
  }
}

bool endOfNumber(const ISource &source) {
  return source.isWS() || source.current() == ',' || source.current() == ']';
}

unsigned long currentIndentLevel(ISource &source) {
  return source.getPosition().second - 1;
}

YNode parseUnquotedString(ISource &source) {
  YNode yNode;
  std::string yamlString;
  while (source.more() && source.current() != kLineFeed) {
    yamlString += source.current();
    source.next();
  }
  source.next();
  yNode = YNode::make<String>(yamlString, ' ');
  return yNode;
}

YNode parseString(ISource &source) {
  YNode yNode;
  const char quote = source.current();
  if (quote == '\'' || quote == '"') {
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
    yamlString = translateEscapes(yamlString);
    moveToNextLine(source);
    yNode = YNode::make<String>(yamlString, quote);
  }
  return yNode;
}

YNode parseComment(ISource &source) {
  std::string comment;
  source.next();
  while (source.more() && source.current() != kLineFeed) {
    comment += source.current();
    source.next();
  }
  if (source.more()) {
    source.next();
  }
  return (YNode::make<Comment>(comment));
}

YNode parseNumber(ISource &source) {
  YNode yNode;
  std::string string;
  for (; source.more() && !endOfNumber(source); source.next()) {
    string += source.current();
  }
  if (Number number{string}; number.is<int>() || number.is<long>() ||
                             number.is<long long>() || number.is<float>() ||
                             number.is<double>() || number.is<long double>()) {
    moveToNextLine(source);
    yNode = YNode::make<Number>(number);
  } else {
    source.backup(1);
  }
  return yNode;
}

YNode parseBoolean(ISource &source) {
  YNode yNode;
  if (source.match("true")) {
    moveToNextLine(source);
    yNode = YNode::make<Boolean>(true);
  } else if (source.match("false")) {
    moveToNextLine(source);
    yNode = YNode::make<Boolean>(false);
  }
  return yNode;
}
YNode parseArray(ISource &source, unsigned long indentLevel) {
  YNode yNode;
  source.next();
  if (source.current() == ' ') {
    source.next();
    yNode = YNode::make<Array>();
    do {
      YRef<Array>(yNode).add(parseDocument(source, indentLevel));
      source.ignoreWS();
      if (indentLevel > currentIndentLevel(source)) {
        return yNode;
      }
    } while (source.match("- "));
    moveToNextLine(source);
  } else {
    source.backup(1);
  }
  return yNode;
}
DictionaryEntry parseKeyValue(ISource &source, unsigned long indentLevel) {
  std::string key{parseKey(source)};
  source.ignoreWS();
  return DictionaryEntry(key, parseDocument(source, indentLevel));
}
YNode parseDictionary(ISource &source, unsigned long indentLevel) {
  YNode yNode = YNode::make<Dictionary>();
  while (source.more() && std::isalpha(source.current())) {
    YRef<Dictionary>(yNode).add(parseKeyValue(source, indentLevel));
    source.ignoreWS();
    if (indentLevel > currentIndentLevel(source)) {
      return yNode;
    }
  }
  return (yNode);
}
YNode parseDocument(ISource &source, unsigned long indentLevel) {
  YNode yNode;
  source.ignoreWS();
  if (source.current() == 't' || source.current() == 'f') {
    yNode = parseBoolean(source);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if ((source.current() == '\'') || (source.current() == '"')) {
    yNode = parseString(source);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if ((source.current() >= '0' && source.current() <= '9') ||
      source.current() == '-' || source.current() == '+') {
    yNode = parseNumber(source);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '#') {
    yNode = parseComment(source);
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == '-') {
    yNode = parseArray(source, currentIndentLevel(source));
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (isKey(source)) {
    yNode = parseDictionary(source, currentIndentLevel(source));
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  yNode = parseUnquotedString(source);
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
        moveToNextLine(source);
        yamlYNodeTree.push_back(YNode::make<Document>());
        // End of document
      } else if (source.match("...")) {
        moveToNextLine(source);
        inDocument = false;
        if (startNumberOfDocuments == getNumberOfDocuments()) {
          yamlYNodeTree.push_back(YNode::make<Document>());
        }
        break;
      } else if (source.current() == '#' && !inDocument) {
        yamlYNodeTree.push_back(parseComment(source));
      } else {
        if (yamlYNodeTree.empty()) {
          yamlYNodeTree.push_back(YNode::make<Document>());
        }
        YRef<Document>(yamlYNodeTree.back()).add(parseDocument(source, 0));
      }
    }
  }
}

} // namespace YAML_Lib
