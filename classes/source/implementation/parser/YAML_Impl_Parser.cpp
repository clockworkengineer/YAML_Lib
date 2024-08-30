
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YNode parseDocument(ISource &source);

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

void parseWS(ISource &source) {
  while (source.more() && source.isWS()) {
    source.next();
  }
}

void parseNext(ISource &source) {
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
  parseWS(source);
  return source.getPosition().first;
}

YNode parseString(ISource &source) {

  const char quote = source.current();
  if (quote == '\'' || quote == '"') {
    std::string yamlString;
    source.next();
    while (source.more() && source.current() != quote) {
      yamlString += source.current();
      source.next();
    }
    parseNext(source);
    return YNode::make<String>(yamlString);
  }
  throw SyntaxError("String does not have and quote.");
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
  std::string string;
  for (; source.more() && !endOfNumber(source); source.next()) {
    string += source.current();
  }
  if (Number number{string}; number.is<int>() || number.is<long>() ||
                             number.is<long long>() || number.is<float>() ||
                             number.is<double>() || number.is<long double>()) {
    parseNext(source);
    return YNode::make<Number>(number);
  }
  throw SyntaxError(source.getPosition(), "Invalid numeric value.");
}

YNode parseBoolean(ISource &source) {
  YNode yNode;
  if (source.match("true")) {
    parseNext(source);
    yNode = YNode::make<Boolean>(true);
  } else if (source.match("false")) {
    parseNext(source);
    yNode = YNode::make<Boolean>(false);
  } else {
    throw SyntaxError("Expected boolean value.");
  }
  return yNode;
}
YNode parseArray(ISource &source) {
  auto yNode = YNode::make<Array>();
  YRef<Array>(yNode).add(parseDocument(source));
  YRef<Array>(yNode).setIndentation(currentIndentLevel(source));
  while (source.match("- ")) {
    YRef<Array>(yNode).add(parseDocument(source));
    YRef<Array>(yNode).setIndentation(currentIndentLevel(source));
  }
  parseNext(source);
  return yNode;
}

ObjectEntry parseKeyValue(ISource &source) {
  std::string key{parseKey(source)};
  parseWS(source);
  if (source.current() == kLineFeed) {
    parseNext(source);
  }
  return ObjectEntry(key, parseDocument(source));
}

YNode parseObject(ISource &source) {
  YNode yNode = YNode::make<Object>();
  while (source.more() && std::isalpha(source.current())) {
    YRef<Object>(yNode).add(parseKeyValue(source));
    YRef<Object>(yNode).setIndentation(currentIndentLevel(source));
  }
  return (yNode);
}

YNode parseDocument(ISource &source) {

  YNode yNode;
  YAML_Impl::identations.push(currentIndentLevel(source));
  if (source.match("- ")) {
    yNode = parseArray(source);
  } else if (source.current() == 't' || source.current() == 'f') {
    yNode = parseBoolean(source);
  } else if ((source.current() >= '0' && source.current() <= '9') ||
             source.current() == '-' || source.current() == '+') {
    yNode = parseNumber(source);
  } else if ((source.current() == '\'') || (source.current() == '"')) {
    yNode = parseString(source);
  } else if (source.current() == '#') {
    yNode = parseComment(source);
  } else {
    yNode = parseObject(source);
  }
  YAML_Impl::identations.pop();
  return yNode;
}

void YAML_Impl::parse(ISource &source) {
  while (source.more()) {
    unsigned int startNumberOfDocuments = getNumberOfDocuments();
    while (source.more()) {
      // Start of document
      if (source.match("---")) {
        parseNext(source);
        yamlDocuments.push_back(YNode::make<Document>());
        // End of document
      } else if (source.match("...")) {
        parseNext(source);
        if (startNumberOfDocuments == getNumberOfDocuments()) {
          yamlDocuments.push_back(YNode::make<Document>());
        }
        break;
      } else {
        if (yamlDocuments.empty()) {
          yamlDocuments.push_back(YNode::make<Document>());
        }
        YRef<Document>(yamlDocuments.back()).add(parseDocument(source));
      }
    }
  }
}

} // namespace YAML_Lib
