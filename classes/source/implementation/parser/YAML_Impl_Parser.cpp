
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
/// <param name="source">Source of JSON.</param>
/// <returns>true on end of number</returns>
bool endOfNumber(const ISource &source) {
  return source.isWS() || source.current() == ',' || source.current() == ']';
}

/// <summary>
/// Parse a number from a JSON source stream.
/// </summary>
/// <param name="source">Source of JSON.</param>
/// <returns>Number JNode.</returns>
bool parseNumber(ISource &source) {
  std::string string;
  for (; source.more() && !endOfNumber(source); source.next()) {
    string += source.current();
  }
  if (Number number{string}; number.is<int>() || number.is<long>() ||
                             number.is<long long>() || number.is<float>() ||
                             number.is<double>() || number.is<long double>()) {
    return (true);
  }
  return (false);
}

bool parseValue(ISource &source) {
  if (source.match("true") || source.match("false")) {
    return (true);
  } else if ((source.current() >= '0' && source.current() <= '9') ||
             source.current() == '-' || source.current() == '+') {
    return (parseNumber(source));
  }
  return (false);
}

void parseIndentLevel(ISource &source) {
  while (source.more() && source.current() == ' ') {
    source.next();
  }
}

void YAML_Impl::parseDocument(ISource &source) {
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
      return;
    } else if (source.match("- ")) {
      source.nextLine();
    } else if (parseValue(source)) {
      source.nextLine();
    } else {
      throw SyntaxError("Incorrect YAML");
    }
  }
}

void YAML_Impl::parse(ISource &source) {
  while (source.more()) {
    parseDocument(source);
  }
}

} // namespace YAML_Lib
