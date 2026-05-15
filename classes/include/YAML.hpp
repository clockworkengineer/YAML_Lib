
#pragma once

/**
 * @file YAML.hpp
 * @brief Main public API header for YAML_Lib.
 *
 * Include this header for all high-level YAML parsing, manipulation, and stringification features.
 * For advanced or low-level types, see YAML_Core.hpp.
 */

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <memory_resource>
#include <string>
#include <string_view>
#include <variant>

namespace YAML_Lib {

// =========================
// YAML forward declarations
// =========================
class IStringify;   ///< Interface for custom stringifiers
class IParser;      ///< Interface for custom parsers
class ISource;      ///< Interface for input sources
class IDestination; ///< Interface for output destinations
class IAction;      ///< Interface for tree traversal actions
class YAML_Impl;    ///< Internal implementation class
struct Node;        ///< YAML node type
#ifdef YAML_LIB_SAX_API
class IYAMLEvents;  ///< Interface for SAX event handlers
#endif


/**
 * @brief Options for configuring YAML_Lib parsing and stringification.
 *
 * Allows users to customize parser, stringifier, memory resource, and limits.
 *
 * @var IStringify* Options::stringifier
 *   Custom stringifier (default: built-in)
 * @var IParser* Options::parser
 *   Custom parser (default: built-in)
 * @var std::pmr::memory_resource* Options::memory_resource
 *   Polymorphic memory resource for allocations
 * @var bool Options::strict_booleans
 *   Enable strict YAML 1.2 boolean parsing (only 'true'/'false' valid)
 * @var unsigned long Options::max_documents
 *   Max documents per stream (0 = unlimited)
 * @var unsigned long Options::max_parse_depth
 *   Max nested parse depth (0 = unlimited)
 * @var unsigned long Options::max_alias_expansions
 *   Max alias expansions (0 = unlimited)
 */
struct Options {
  IStringify *stringifier{nullptr};
  IParser *parser{nullptr};
  std::pmr::memory_resource *memory_resource{nullptr};
  bool strict_booleans{false};
  unsigned long max_documents{32};
  unsigned long max_parse_depth{128};
  unsigned long max_alias_expansions{64};
};

// ========================
// YAML character constants
// ========================
constexpr char kNull{0x00};
constexpr char kCarriageReturn{0x0D};
constexpr char kLineFeed{0x0A};
constexpr char kSpace{0x20};
constexpr char kDoubleQuote{0x22};
constexpr char kApostrophe{0x27};
constexpr char kComma{0x2C};
constexpr char kColon{0x3A};
constexpr char kLeftSquareBracket{0x5B};
constexpr char kRightSquareBracket{0x5D};
constexpr char kLeftCurlyBrace{0x7B};
constexpr char kRightCurlyBrace{0x7D};

// =====================
// YAML string constants
// =====================
constexpr auto kOverride{"<<"};
constexpr auto kStartDocument{"---"};
constexpr auto kEndDocument{"..."};

/**
 * @brief Main entry point for parsing, manipulating, and generating YAML documents.
 *
 * The YAML class provides methods to parse YAML text, access and modify the node tree,
 * and stringify back to YAML or other formats. Supports multiple documents per stream.
 */
class YAML {
  /**
   * @brief Parse YAML from a string and return a YAML object.
   * @param yaml_string YAML text to parse
   * @return YAML object
   */
  static std::unique_ptr<YAML> fromString(const std::string_view &yaml_string);

#ifdef YAML_LIB_FILE_IO
  /**
   * @brief Parse YAML from a file and return a YAML object.
   * @param file_name Path to YAML file
   * @return YAML object
   */
  static std::unique_ptr<YAML> fromFileToYAML(const std::string_view &file_name);
#endif

  /**
   * @brief Stringify the node tree to a string (YAML format).
   * @return YAML string
   */
  std::string toString() const;
public:
  /**
   * @brief Variant types allowed in YAML initializer lists.
   */
  using InitializerListTypes =
      std::variant<int, long, long long, float, double, long double, bool,
                   std::string, std::nullptr_t, Node>;
  /**
   * @brief Array initializer list for YAML arrays.
   */
  using ArrayInitializer = std::initializer_list<InitializerListTypes>;
  /**
   * @brief Dictionary initializer list for YAML mappings.
   */
  using DictionaryInitializer =
      std::initializer_list<std::pair<std::string, InitializerListTypes>>;
  /**
   * @brief Supported file formats for reading/writing YAML files.
   */
  enum class Format : uint8_t {
    utf8 = 0,    ///< UTF-8 (default)
    utf8BOM,     ///< UTF-8 with BOM
    utf16BE,     ///< UTF-16 big-endian
    utf16LE,     ///< UTF-16 little-endian
    utf32BE,     ///< UTF-32 big-endian
    utf32LE      ///< UTF-32 little-endian
  };

  /**
   * @brief Construct a YAML object with optional custom stringifier and parser.
   * @param stringify Custom stringifier (default: built-in)
   * @param parser Custom parser (default: built-in)
   */
  explicit YAML(IStringify *stringifier = nullptr, IParser *parser = nullptr);

  /**
   * @brief Construct a YAML object with custom options.
   * @param options Options struct for configuration
   */
  explicit YAML(const Options &options);

  /**
   * @brief Construct a YAML object using a custom memory resource.
   * @param mr Polymorphic memory resource for allocations
   * @note The resource must outlive this YAML object.
   */
  explicit YAML(std::pmr::memory_resource *memory_resource);

  /**
   * @brief Construct a YAML object by parsing a YAML string.
   * @param yamlString YAML text to parse
   */
  explicit YAML(const std::string_view &yaml_string);

  /**
   * @brief Construct a YAML array from an initializer list.
   * @param array Array initializer
   */
  YAML(const ArrayInitializer &array_initializer);

  /**
   * @brief Construct a YAML mapping from an initializer list.
   * @param dictionary Dictionary initializer
   */
  YAML(const DictionaryInitializer &dictionary_initializer);

  YAML(const YAML &other) = delete;
  YAML &operator=(const YAML &other) = delete;
  YAML(YAML &&other) = delete;
  YAML &operator=(YAML &&other) = delete;

  /**
   * @brief Destructor for YAML object.
   */
  ~YAML() noexcept;

  /**
   * @brief Get the YAML_Lib version string.
   * @return Version string
   */
  [[nodiscard]] static std::string version();

  /**
   * @brief Get the number of YAML documents in the stream.
   * @return Number of documents
   */
  [[nodiscard]] unsigned long getNumberOfDocuments() const;

  /**
   * @brief Parse YAML from a source into the node tree.
   * @param source Input source
   */
  void parse(ISource &source) const;
  void parse(ISource &&source) const;

  /**
   * @brief Stringify the node tree to a destination (no whitespace formatting).
   * @param destination Output destination
   */
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;

  /**
   * @brief Get a mutable reference to the document at the given index.
   * @param index Document index
   * @return Mutable Node reference
   */
  [[nodiscard]] Node &document(unsigned long index);

  /**
   * @brief Get a const reference to the document at the given index.
   * @param index Document index
   * @return Const Node reference
   */
  [[nodiscard]] const Node &document(unsigned long index) const;

  /**
   * @brief Traverse the YAML node tree with an action visitor.
   * @param action Visitor implementing IAction
   */
  void traverse(IAction &action);
  void traverse(IAction &action) const;

#ifdef YAML_LIB_SAX_API
  /**
   * @brief Emit SAX events for every document in the tree.
   * @param handler SAX event handler
   */
  void traverseEvents(IYAMLEvents &handler) const;
#endif // YAML_LIB_SAX_API

  /**
   * @brief Access a mapping entry by key.
   * @param key Mapping key
   * @return Mutable Node reference
   */
  [[nodiscard]] Node &operator[](const std::string_view &key);
  [[nodiscard]] const Node &operator[](const std::string_view &key) const;

  /**
   * @brief Access a sequence entry by index.
   * @param index Array index
   * @return Mutable Node reference
   */
  [[nodiscard]] Node &operator[](std::size_t index);
  [[nodiscard]] const Node &operator[](std::size_t index) const;

#ifdef YAML_LIB_FILE_IO
  /**
   * @brief Parse a YAML file and return its contents as a string.
   * @param yamlFileName Path to YAML file
   * @return File contents as string
   */
  static std::string fromFile(const std::string_view &yamlFileName);

  /**
   * @brief Write a YAML string to a file.
   * @param fileName Path to output file
   * @param yamlString YAML text to write
   * @param format File encoding format (default: utf8)
   */
  static void toFile(const std::string_view &fileName,
                     const std::string_view &yamlString,
                     Format format = Format::utf8);

  /**
   * @brief Get the file format of a YAML file.
   * @param fileName Path to file
   * @return File format
   */
  static Format getFileFormat(const std::string_view &fileName);
#endif // YAML_LIB_FILE_IO

  /**
   * @brief Enable or disable strict YAML 1.2 boolean parsing.
   * @param strict If true, only 'true'/'false' are valid booleans
   */
  static void setStrictBooleans(bool strict) noexcept;

private:
  // Internal implementation pointer
  const std::unique_ptr<YAML_Impl> implementation;
};

} // namespace YAML_Lib
