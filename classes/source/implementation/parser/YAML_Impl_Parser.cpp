
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

bool validKey(const std::string &key) {
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
  if (!validKey(key)) {
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

YNode parseString(ISource &source) {
  YNode yNode;
  const char quote = source.current();
  if (quote == '\'' || quote == '"') {
    std::string yamlString;
    source.next();
    while (source.more() && source.current() != quote) {
      yamlString += source.current();
      source.next();
    }
    moveToNextLine(source);
    yNode = YNode::make<String>(yamlString);
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
    source.backup(string.size());
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
  if (source.current() == '-') {
    yNode = parseArray(source, currentIndentLevel(source));
    if (!yNode.isEmpty()) {
      return yNode;
    }
  }
  if (source.current() == 't' || source.current() == 'f') {
    yNode = parseBoolean(source);
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
  if ((source.current() == '\'') || (source.current() == '"')) {
    yNode = parseString(source);
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
  yNode = parseDictionary(source, currentIndentLevel(source));
  if (!yNode.isEmpty()) {
    return yNode;
  }
  throw SyntaxError("Invalid YAML.");
}

void YAML_Impl::parse(ISource &source) {
  while (source.more()) {
    unsigned int startNumberOfDocuments = getNumberOfDocuments();
    while (source.more()) {
      // Start of document
      if (source.match("---")) {
        moveToNextLine(source);
        yamlDocuments.push_back(YNode::make<Document>());
        // End of document
      } else if (source.match("...")) {
        moveToNextLine(source);
        if (startNumberOfDocuments == getNumberOfDocuments()) {
          yamlDocuments.push_back(YNode::make<Document>());
        }
        break;
      } else {
        if (yamlDocuments.empty()) {
          yamlDocuments.push_back(YNode::make<Document>());
        }
        YRef<Document>(yamlDocuments.back()).add(parseDocument(source, 0));
      }
    }
  }
}

} // namespace YAML_Lib
