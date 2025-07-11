#pragma once

namespace YAML_Lib {

// =========================
// YAML forward declarations
// =========================
  class IStringify;
  class IParser;
  class ISource;
  class IDestination;
  class IAction;
  class YAML_Impl;
  struct Node;

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

class YAML {
public:
  // Possible YAML Node initializer list types
  using InitializerListTypes =
      std::variant<int, long, long long, float, double, long double, bool,
                   std::string, std::nullptr_t, Node>;
  // Array initializer list
  using ArrayInitializer = std::initializer_list<InitializerListTypes>;
  // Dictionary initializer list
  using DictionaryInitializer =
      std::initializer_list<std::pair<std::string, InitializerListTypes>>;
  // YAML file formats
  enum class Format : uint8_t {
    utf8 = 0,
    utf8BOM,
    utf16BE,
    utf16LE,
    utf32BE,
    utf32LE
  };
  // Pass any user defined parser/stringifier here
  explicit YAML(IStringify *stringify = nullptr, IParser *parser = nullptr);
  // Pass in default YAML to parse
  explicit YAML(const std::string_view &yamlString);
  // Construct an array
  YAML(const ArrayInitializer &array);
  // Construct object
  YAML(const DictionaryInitializer &dictionary);
  YAML(const YAML &other) = delete;
  YAML &operator=(const YAML &other) = delete;
  YAML(YAML &&other) = delete;
  YAML &operator=(YAML &&other) = delete;
  // Provide own destructor
  ~YAML();
  // Get YAML library version
  [[nodiscard]] static std::string version();
  // Get YAML library version
  [[nodiscard]] unsigned long getNumberOfDocuments() const;
  // Parse YAML into a tree
  void parse(ISource &source) const;
  void parse(ISource &&source) const;
  // Create YAML text string from Node tree (no whitespace)
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;
  // Get index of YAML document
  [[nodiscard]] Node &document(unsigned long index);
  [[nodiscard]] const Node &document(unsigned long index) const;
  // Traverse YAML tree
  void traverse(IAction &action);
  void traverse(IAction &action) const;
  // Search for YAML object entry with a given key
  Node &operator[](const std::string_view &key);
  const Node &operator[](const std::string_view &key) const;
  // Get YAML array entry at index
  Node &operator[](std::size_t index);
  const Node &operator[](std::size_t index) const;
  // Read/write YAML file
  static std::string fromFile(const std::string_view &yamlFileName);
  static void toFile(const std::string_view &fileName, const std::string_view &yamlString,
                     Format format = Format::utf8);
  // Get YAML file format
  static Format getFileFormat(const std::string_view &fileName);

private:
  // YAML implementation
  const std::unique_ptr<YAML_Impl> implementation;
};

} // namespace YAML_Lib
