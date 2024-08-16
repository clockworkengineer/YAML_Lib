
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

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

void YAML_Impl::parseDocuments(ISource &source) {
  auto yNode = YNode::make<Array>();
  while (source.more()) {
    unsigned int startNumberOfDocuments = getNumberOfDocuments();
    while (source.more()) {
      parseIndentLevel(source);
      // Start of document
      if (source.match("---")) {
        numberOfDocuments++;
        source.nextLine();
        // End of document
      } else if (source.match("...")) {
        source.nextLine();
        if (startNumberOfDocuments == getNumberOfDocuments()) {
          numberOfDocuments++;
        }
        break;
      } else if (source.match("- ")) {
        YRef<Array>(yNode).add(parseArray(source));
      } else if (source.match("true")) {
        source.nextLine();
        YRef<Array>(yNode).add(YNode::make<Boolean>(true));
      } else if (source.match("false")) {
        YRef<Array>(yNode).add(YNode::make<Boolean>(false));
        source.nextLine();
      } else if ((source.current() >= '0' && source.current() <= '9') ||
                 source.current() == '-' || source.current() == '+') {
        YRef<Array>(yNode).add(parseNumber(source));
        source.nextLine();
      } else {
        throw SyntaxError("Incorrect YAML");
      }
    }
  }
}
void YAML_Impl::parse(ISource &source) {
    parseDocuments(source);
}

} // namespace YAML_Lib
