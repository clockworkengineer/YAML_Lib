
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

bool parseValue(ISource &source) {
  if (source.match("true")||source.match("false")) {
    return(true);
  }
      return(false);
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
    } else if (source.match("-")) {
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
