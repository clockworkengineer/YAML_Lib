
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YAML_Impl::YAML_Impl(IStringify *stringify, IParser *parser) {
  if (parser == nullptr) {
    yamlParser = std::make_unique<YAML_Parser>(yamlTranslator);
  } else {
    yamlParser.reset(parser);
  }
  if (stringify == nullptr) {
    yamlStringify = std::make_unique<YAML_Stringify>(yamlTranslator);
  } else {
    yamlStringify.reset(stringify);
  }
}

std::string YAML_Impl::version() {
  std::stringstream versionString;
  versionString << "YAML_Lib Version  " << YAML_VERSION_MAJOR << "."
                << YAML_VERSION_MINOR << "." << YAML_VERSION_PATCH;
  return versionString.str();
}

void YAML_Impl::parse(ISource &source) { yamlTree = yamlParser->parse(source); }

void YAML_Impl::stringify(IDestination &destination) const {
  yamlStringify->stringify(yamlTree, destination);
}

} // namespace YAML_Lib
