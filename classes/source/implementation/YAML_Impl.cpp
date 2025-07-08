
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
  traverseNodes(yamlTree[0][0], action);
}
void YAML_Impl::traverse(IAction &action) const {
  if (yamlTree.empty()) {
    throw Error("No YAML to traverse.");
  }
  traverseNodes(yamlTree[0][0], action);
}

Node &YAML_Impl::operator[](const std::string_view &key) {
  try {
    if (getNumberOfDocuments() == 0) {
      BufferSource source("---\n...\n");
      parse(source);
      NRef<Document>(yamlTree[0]).add(Node::make<Dictionary>());
    }
    return document(0)[key];
  } catch ([[maybe_unused]] Node::Error &error) {
    NRef<Dictionary>(document(0))
        .add(Dictionary::Entry(key, Node::make<Hole>()));
    return document(0)[key];
  }
}
const Node &YAML_Impl::operator[](const std::string_view &key) const {
  return document(0)[0][key];
}

Node &YAML_Impl::operator[](const std::size_t index) {
  try {
    if (getNumberOfDocuments() == 0) {
      BufferSource source("---\n...\n");
      parse(source);
      NRef<Document>(yamlTree[0]).add(Node::make<Array>());
    }
    return document(0)[index];
  } catch ([[maybe_unused]] Node::Error &error) {
    NRef<Array>(document(0)).resize(index);
    return document(0)[index];
  }
}

const Node & YAML_Impl::operator[](const std::size_t index) const {
  return document(0)[index];
}
} // namespace YAML_Lib
