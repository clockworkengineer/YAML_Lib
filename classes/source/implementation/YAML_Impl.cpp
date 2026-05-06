
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YAML_Impl::YAML_Impl(IStringify *stringify, IParser *parser,
                     std::pmr::memory_resource *mr)
    : memoryResource{mr} {
  if (parser == nullptr) {
    yamlParser = std::make_unique<Default_Parser>(std::make_unique<Default_Translator>());
  } else {
    yamlParser.reset(parser);
  }
  if (stringify == nullptr) {
    yamlStringify = std::make_unique<Default_Stringify>(std::make_unique<Default_Translator>());
  } else {
    yamlStringify.reset(stringify);
  }
}

YAML_Impl::YAML_Impl(const Options &options)
    : memoryResource{options.memoryResource} {
  Default_Parser::setStrictBooleans(options.strictBooleans);

  if (options.parser == nullptr) {
    yamlParser = std::make_unique<Default_Parser>(std::make_unique<Default_Translator>());
  } else {
    yamlParser.reset(options.parser);
  }
  if (options.stringify == nullptr) {
    yamlStringify = std::make_unique<Default_Stringify>(std::make_unique<Default_Translator>());
  } else {
    yamlStringify.reset(options.stringify);
  }
}

std::string YAML_Impl::version() {
  std::stringstream versionString;
  versionString << "YAML_Lib Version  " << YAML_VERSION_MAJOR << "."
                << YAML_VERSION_MINOR << "." << YAML_VERSION_PATCH;
  return versionString.str();
}

void YAML_Impl::parse(ISource &source) {
  // RAII guard: if the caller supplied a PMR resource, install it as the PMR
  // default for the duration of parse so that all std::pmr::* containers
  // created during parse (Array/Document entries, Dictionary entries/index)
  // draw from that resource. The previous default is restored on scope exit.
  // NOTE: this modifies the process-wide PMR default; single-threaded use only.
  struct ResourceScope {
    std::pmr::memory_resource *prev_;
    const bool active_;
    explicit ResourceScope(std::pmr::memory_resource *mr)
        : prev_{mr ? std::pmr::get_default_resource() : nullptr},
          active_{mr != nullptr} {
      if (active_) std::pmr::set_default_resource(mr);
    }
    ~ResourceScope() {
      if (active_) std::pmr::set_default_resource(prev_);
    }
  } scope{memoryResource};
  yamlTree = yamlParser->parse(source);
}

void YAML_Impl::stringify(IDestination &destination) const {
  // Pre-reserve a heuristic capacity to reduce reallocations during stringify.
  // 512 bytes per document is a conservative estimate for typical YAML.
  destination.reserve(std::max(std::size_t{4096}, yamlTree.size() * 512));
  for (auto &document : yamlTree) {
    yamlStringify->stringify(document ,destination, 0);
  }
}

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

const Node & YAML_Impl::operator[](const std::size_t index) const {
  return document(0)[index];
}
} // namespace YAML_Lib
