
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

void parseIndentLevel(ISource &source) {
  while (source.more() && source.current() == ' ') {
    source.next();
  }
}

void YAML_Impl::parseDocument(ISource &source) {
  while (source.more()) {
    // Start of document
    if (source.match("---")) {
      numberOfDocuments++;
      source.nextLine();
      parseIndentLevel(source);
      // End of document
    } else if (source.match("...")) {
      source.nextLine();
      return;
    } else {
      numberOfDocuments++;
      source.nextLine();
      parseIndentLevel(source);
    }
  }
}

void YAML_Impl::parse(ISource &source) {
  while (source.more()) {
    parseDocument(source);
  }
}

} // namespace YAML_Lib
