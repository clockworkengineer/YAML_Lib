
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YNode parseString(ISource &source) {

  if (source.current() == '\'' || source.current() == '"') {
    std::string yamlString;
    const char quote = source.current();
    source.next();
    while (source.more() && source.current() != quote) {
      yamlString += source.current();
      source.next();
    }
    return YNode::make<String>(yamlString);
  }
  throw SyntaxError("String does not have and quote.");
}
/// <summary>
/// Has the end of a number been reached in source stream ?
/// </summary>
/// <param name="source">Source of YAML.</param>
/// <returns>true on end of number</returns>
bool endOfNumber(const ISource &source) {
  return source.isWS() || source.current() == ',' || source.current() == ']';
}

void parseIndentLevel(ISource &source) {
  while (source.more() && source.current() == ' ') {
    source.next();
  }
}
/// <summary>
/// Parse a number from a YAML source stream.
/// </summary>
/// <param name="source">Source of YAML.</param>
/// <returns>Number JNode.</returns>
YNode parseNumber(ISource &source) {
  std::string string;
  for (; source.more() && !endOfNumber(source); source.next()) {
    string += source.current();
  }
  if (Number number{string}; number.is<int>() || number.is<long>() ||
                             number.is<long long>() || number.is<float>() ||
                             number.is<double>() || number.is<long double>()) {
    return YNode::make<Number>(number);
  }
  throw SyntaxError(source.getPosition(), "Invalid numeric value.");
}

YNode parseArray(ISource &source) {
  auto yNode = YNode::make<Array>();
  source.nextLine();
  parseIndentLevel(source);
  YRef<Array>(yNode).add(YNode::make<Null>());
  while (source.match("- ")) {
    source.nextLine();
    parseIndentLevel(source);
    YRef<Array>(yNode).add(YNode::make<Null>());
  }
  return yNode;
}


void YAML_Impl::parseInternal(ISource &source) {
  if (source.match("- ")) {
    YRef<Array>(yamlDocuments.back()).add(parseArray(source));
  } else if (source.match("true")) {
    source.nextLine();
    YRef<Array>(yamlDocuments.back()).add(YNode::make<Boolean>(true));
  } else if (source.match("false")) {
    YRef<Array>(yamlDocuments.back()).add(YNode::make<Boolean>(false));
    source.nextLine();
  } else if ((source.current() >= '0' && source.current() <= '9') ||
             source.current() == '-' || source.current() == '+') {
    YRef<Array>(yamlDocuments.back()).add(parseNumber(source));
    source.nextLine();
  } else if ((source.current() == '\'') || (source.current() == '"')) {
    YRef<Array>(yamlDocuments.back()).add(parseString(source));
    source.nextLine();
  } else {
    throw SyntaxError("Incorrect YAML");
  }
}

void YAML_Impl::parseDocuments(ISource &source) {
  while (source.more()) {
    unsigned int startNumberOfDocuments = getNumberOfDocuments();
    while (source.more()) {
      parseIndentLevel(source);
      // Start of document
      if (source.match("---")) {
        source.nextLine();
        yamlDocuments.push_back(YNode::make<Array>());
        // End of document
      } else if (source.match("...")) {
        source.nextLine();
        if (startNumberOfDocuments == getNumberOfDocuments()) {
          yamlDocuments.push_back(YNode::make<Array>());
        }
        break;
      } else {
        if (yamlDocuments.empty()) {
          yamlDocuments.push_back(YNode::make<Array>());
        }
        parseInternal(source);
      }
    }
  }
}
void YAML_Impl::parse(ISource &source) { parseDocuments(source); }

} // namespace YAML_Lib
