
#include "YAML_Impl.hpp"

namespace YAML_Lib {

void YAML_Impl::traverse(IAction &action) {
  if (yamlTree.empty()) {
    YAML_THROW(Error, "No YAML to traverse.");
  }
  traverseNodes(yamlTree[0][0], action);
}

void YAML_Impl::traverse(IAction &action) const {
  if (yamlTree.empty()) {
    YAML_THROW(Error, "No YAML to traverse.");
  }
  traverseNodes(yamlTree[0][0], action);
}

#ifdef YAML_LIB_SAX_API
void YAML_Impl::traverseEvents(IYAMLEvents &handler) const {
  if (yamlTree.empty()) {
    YAML_THROW(Error, "No YAML to traverse.");
  }
  for (const auto &docNode : yamlTree) {
    handler.onDocumentStart();
    emitEvents(docNode[0], handler);
    handler.onDocumentEnd();
  }
}
#endif // YAML_LIB_SAX_API

Node &YAML_Impl::operator[](const std::string_view &key) {
  if (getNumberOfDocuments() == 0) {
    BufferSource source("---\n...\n");
    parse(source);
    NRef<Document>(yamlTree[0]).add(Node::make<Dictionary>());
  }
  Node &root = document(0);
  if (isA<Hole>(root)) {
    root = Node::make<Dictionary>();
  }
  if (isA<Dictionary>(root)) {
    auto &dictionary = NRef<Dictionary>(root);
    if (dictionary.contains(key)) {
      return dictionary[key];
    }
    dictionary.add(Dictionary::Entry(key, Node::make<Hole>()));
    return dictionary[key];
  }
  YAML_THROW(Error, "Root document is not a dictionary for key access.");
}

const Node &YAML_Impl::operator[](const std::string_view &key) const {
  return document(0)[key];
}

Node &YAML_Impl::operator[](const std::size_t index) {
  if (getNumberOfDocuments() == 0) {
    BufferSource source("---\n...\n");
    parse(source);
    NRef<Document>(yamlTree[0]).add(Node::make<Array>());
  }
  Node &root = document(0);
  if (isA<Hole>(root)) {
    root = Node::make<Array>();
  }
  if (isA<Array>(root)) {
    auto &array = NRef<Array>(root);
    if (index >= array.size()) {
      array.resize(index);
    }
    return array[index];
  }
  if (isA<Document>(root)) {
    auto &document = NRef<Document>(root);
    if (index >= document.size()) {
      document.resize(index);
    }
    return document[index];
  }
  YAML_THROW(Error, "Root document is not an array or document for index access.");
}

const Node &YAML_Impl::operator[](const std::size_t index) const {
  return document(0)[index];
}

} // namespace YAML_Lib
