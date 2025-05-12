
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
  // auto yamlTranslator = std::make_unique<><Default_Translator>();
  if (parser == nullptr) {
    yamlParser = std::make_unique<Default_Parser>( std::make_unique<Default_Translator>());
  } else {
    yamlParser.reset(parser);
  }
  if (stringify == nullptr) {
    yamlStringify = std::make_unique<Default_Stringify>( std::make_unique<Default_Translator>());
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
  for (auto &document : yamlTree) {
    yamlStringify->stringify(document ,destination, 0);
  }
}

void YAML_Impl::traverse(IAction &action) {
  if (yamlTree.empty()) {
    throw Error("No YAML to traverse.");
  }
  traverseYNodes(yamlTree[0][0], action);
}
void YAML_Impl::traverse(IAction &action) const {
  if (yamlTree.empty()) {
    throw Error("No YAML to traverse.");
  }
  traverseYNodes(yamlTree[0][0], action);
}

YNode &YAML_Impl::operator[](const std::string_view &key) {
  try {
    if (getNumberOfDocuments() == 0) {
      BufferSource source("---\n...\n");
      parse(source);
      YRef<Document>(yamlTree[0]).add(YNode::make<Dictionary>());
    }
    return document(0)[key];
  } catch ([[maybe_unused]] Dictionary::Error &error) {
    YRef<Dictionary>(document(0))
        .add(Dictionary::Entry(key, YNode::make<Hole>()));
    return document(0)[key];
  }
}
const YNode &YAML_Impl::operator[](const std::string_view &key) const {
  return document(0)[0][key];
}

YNode &YAML_Impl::operator[](const std::size_t index) {
  try {
    if (getNumberOfDocuments() == 0) {
      BufferSource source("---\n...\n");
      parse(source);
      YRef<Document>(yamlTree[0]).add(YNode::make<Array>());
    }
    return document(0)[index];
  } catch ([[maybe_unused]] YNode::Error &error) {
    YRef<Array>(document(0)).resize(index);
    return document(0)[index];
  }
}

const YNode & YAML_Impl::operator[](const std::size_t index) const {
  return document(0)[index];
}
} // namespace YAML_Lib
