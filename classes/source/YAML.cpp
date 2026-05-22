//
// Class: YAML
//
// Description: Perform YAML parse/stringify to/from a buffer or
// file. For an in-depth description of the YAML specification refer
// to its RFC at https://yaml.org/spec/1.2.2/.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"
#include <stdexcept>

namespace YAML_Lib {

/// <summary>
/// Create YAML object.
/// </summary>
/// <param name="stringify">Pointer to stringifier interface.</param>
/// <param name="parser">Pointer to parser interface.</param>
YAML::YAML([[maybe_unused]] IStringify *stringify,
           [[maybe_unused]] IParser *parser)
    : implementation(std::make_unique<YAML_Impl>(stringify, parser)) {}

/// <summary>
/// Create YAML object using runtime parser/stringifier and strict-mode options.
/// </summary>
/// <param name="options">Runtime configuration options.</param>
YAML::YAML(const Options &options)
    : implementation(std::make_unique<YAML_Impl>(options)) {}
/// <summary>
/// YAML constructor with a PMR memory resource.  All node allocations during
/// parse() draw from the supplied resource.  The resource MUST outlive this
/// YAML object.
/// </summary>
/// <param name="mr">PMR memory resource (e.g. MonotonicArena::resource()).</param>
YAML::YAML(std::pmr::memory_resource *mr)
    : implementation(std::make_unique<YAML_Impl>(nullptr, nullptr, mr)) {}
/// <summary>
/// Destroy YAML object.
/// </summary>
YAML::~YAML() noexcept = default;
/// <summary>
/// YAML constructor. Pass a YAML string to be initially parsed.
/// </summary>
/// <param name="yamlString">YAML string.</param>
YAML::YAML(const std::string_view &yamlString) : YAML() {
  parse(BufferSource{yamlString});
}

void Options::validate() const {
  static constexpr unsigned long kMaxSafeDocuments = 1000000UL;
  static constexpr unsigned long kMaxSafeParseDepth = 65536UL;
  static constexpr unsigned long kMaxSafeAliasExpansions = 1000000UL;
  static constexpr unsigned long kMaxSafeAliases = 1000000UL;
  static constexpr unsigned long kMaxSafeScalarLength = 256UL * 1024UL * 1024UL;
  static constexpr unsigned long kMaxSafeCollectionSize = 10000000UL;

  if (max_documents > kMaxSafeDocuments) {
    YAML_THROW(std::invalid_argument,
               "YAML::Options::max_documents exceeds safe limit.");
  }
  if (max_parse_depth > kMaxSafeParseDepth) {
    YAML_THROW(std::invalid_argument,
               "YAML::Options::max_parse_depth exceeds safe limit.");
  }
  if (max_alias_expansions > kMaxSafeAliasExpansions) {
    YAML_THROW(std::invalid_argument,
               "YAML::Options::max_alias_expansions exceeds safe limit.");
  }
  if (max_aliases > kMaxSafeAliases) {
    YAML_THROW(std::invalid_argument,
               "YAML::Options::max_aliases exceeds safe limit.");
  }
  if (max_scalar_length > kMaxSafeScalarLength) {
    YAML_THROW(std::invalid_argument,
               "YAML::Options::max_scalar_length exceeds safe limit.");
  }
  if (max_collection_size > kMaxSafeCollectionSize) {
    YAML_THROW(std::invalid_argument,
               "YAML::Options::max_collection_size exceeds safe limit.");
  }
}
/// <summary>
/// YAML constructor (array).
/// </summary>
/// <param name="array">Initializer list of single values or Node.</param>
YAML::YAML(const ArrayInitializer &array) : YAML() {
  if (getNumberOfDocuments() == 0) {
    BufferSource source("---\n[]\n...\n");
    parse(source);
  }
  this->document(0) = Node(array);
}

/// <summary>
/// YAML constructor (object).
/// </summary>
/// <param name="dictionary">Initializer list of key/value(Node) pairs.</param>
YAML::YAML(const DictionaryInitializer &dictionary) : YAML() {
  if (getNumberOfDocuments() == 0) {
    BufferSource source("---\n null : null\n...\n");
    parse(source);
  }
  this->document(0) = Node(dictionary);
}
/// <summary>
/// Fetch version string for current YAML_Lib.
/// </summary>
/// <returns>Version string.</returns>
std::string YAML::version() { return YAML_Impl::version(); }
/// <summary>
/// Return number of documents parsed.
/// </summary>
/// <returns>Number of documents.</returns>
unsigned long YAML::getNumberOfDocuments() const {
  return implementation->getNumberOfDocuments();
}
/// <summary>
/// Parse YAML from source stream into the Node tree.
/// </summary>
/// <param name="source"></param>
void YAML::parse(ISource &source) const { implementation->parse(source); }
void YAML::parse(ISource &&source) const { implementation->parse(source); }
#ifndef YAML_LIB_NO_EXCEPTIONS
bool YAML::tryParse(ISource &source, std::string &errorMessage) {
  try {
    parse(source);
    return true;
  } catch (const std::exception &ex) {
    errorMessage = ex.what();
    return false;
  }
}
bool YAML::tryParse(ISource &&source, std::string &errorMessage) {
  return tryParse(source, errorMessage);
}
#endif
/// <summary>
/// Stringify Node tree to destination stream (file/buffer/network).
/// </summary>
/// <param name="destination"></param>
void YAML::stringify(IDestination &destination) const {
  implementation->stringify(destination);
}
void YAML::stringify(IDestination &&destination) const {
  implementation->stringify(destination);
}
#ifndef YAML_LIB_NO_EXCEPTIONS
bool YAML::tryStringify(IDestination &destination, std::string &errorMessage) const {
  try {
    stringify(destination);
    return true;
  } catch (const std::exception &ex) {
    errorMessage = ex.what();
    return false;
  }
}
bool YAML::tryStringify(IDestination &&destination, std::string &errorMessage) const {
  return tryStringify(destination, errorMessage);
}
#endif
/// <summary>
/// Return Node of the index document within YAML tree.
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
Node &YAML::document(const unsigned long index) {
  return implementation->document(index);
}
const Node &YAML::document(const unsigned long index) const {
  return implementation->document(index);
}
/// <summary>
/// Recursively traverse Node structure calling IAction methods (read-only)
///  or to change the YAML tree node directly.
/// </summary>
/// <param name="action">Action methods to call during traversal.</param>
/// Traverse using non-const YAML so can change YAML tree
void YAML::traverse(IAction &action) { implementation->traverse(action); }
// Traverse using const YAML so cannot change YAML tree
void YAML::traverse(IAction &action) const {
  std::as_const(*implementation).traverse(action);
}
#ifdef YAML_LIB_SAX_API
/// <summary>
/// Walk the parsed tree and fire IYAMLEvents callbacks for every node,
/// in document order.  onDocumentStart/End bracket each document.
/// </summary>
/// <param name="handler">Caller-supplied SAX event handler.</param>
void YAML::traverseEvents(IYAMLEvents &handler) const {
  std::as_const(*implementation).traverseEvents(handler);
}
#endif // YAML_LIB_SAX_API
/// <summary>
/// Return object entry for the passed in keys.
/// </summary>
/// <param name="key">Object entry (Node) key.</param>
Node &YAML::operator[](const std::string_view &key) {
  return (*implementation)[key];
}
const Node &YAML::operator[](const std::string_view &key) const {
  return (*implementation)[key];
}
/// <summary>
/// Return array entry for the passed in index.
/// </summary>
/// <param name="index">Array entry (Node) index.</param>
Node &YAML::operator[](const std::size_t index) {
  return (*implementation)[index];
}
const Node &YAML::operator[](const std::size_t index) const {
  return (*implementation)[index];
}
/// <summary>
/// Open a YAML file, read its contents into a string buffer and return
/// the buffer.
/// </summary>
/// <param name="yamlFileName">YAML file name</param>
/// <returns>YAML string.</returns>
#ifdef YAML_LIB_FILE_IO
std::string YAML::fromFile(const std::string_view &yamlFileName) {
  return YAML_Impl::fromFile(yamlFileName);
}

/// <summary>
/// Create an YAML file and write YAML string to it.
/// </summary>
/// <param name="fileName">YAML file name</param>
/// <param name="yamlString">YAML string</param>
/// <param name="format">YAML file format</param>
void YAML::toFile(const std::string_view &fileName,
                  const std::string_view &yamlString, const Format format) {
  YAML_Impl::toFile(fileName, yamlString, format);
}
/// <summary>
/// Return format of YAML file.
/// </summary>
/// <param name="fileName">YAML file name</param>
/// <returns>YAML file format.</returns>
YAML::Format YAML::getFileFormat(const std::string_view &fileName) {
  return YAML_Impl::getFileFormat(fileName);
}
#endif // YAML_LIB_FILE_IO
/// <summary>
/// Enable or disable strict YAML 1.2 boolean parsing.
/// When strict, only 'true' and 'false' are recognised as booleans;
/// YAML 1.1 forms (yes/no/on/off and their variants) are treated as strings.
/// </summary>
/// <param name="strict">True to enable strict booleans, false to restore
/// permissive YAML 1.1 behaviour.</param>
void YAML::setStrictBooleans(const bool strict) noexcept {
  Default_Parser::setStrictBooleans(strict);
}
} // namespace YAML_Lib
