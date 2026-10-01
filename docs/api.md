# YAML_Lib API Reference

All public symbols live in the `YAML_Lib` namespace. Standard entry headers are [`YAML.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML.hpp) and [`YAML_Core.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Core.hpp), with specialized modules [`YAML_Format.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Format.hpp) and [`YAML_Serialization.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Serialization.hpp).

## See Also
- [User Guide](guide.md)
- [C++23 Modern Features & Migration](cpp23_features.md)
- [Object & Container Serialization](serialization.md)
- [Embedded & Minimal Builds](embedded_and_minimal.md)
- [SOLID Architecture](solid_architecture.md)
- [Extending YAML_Lib](extending_yaml_lib.md)
- [README](../README.md)

---

## Table of Contents
- [YAML class](#yaml-class)
- [Node class & Ergonomic Accessors](#node-class--ergonomic-accessors)
- [Node types](#node-types)
- [Formatting Support (`YAML_Format.hpp`)](#formatting-support-yaml_formathpp)
- [Serialization Framework (`YAML_Serialization.hpp`)](#serialization-framework-yaml_serializationhpp)
- [Segregated Facade Headers](#segregated-facade-headers)
- [I/O — Sources](#io--sources)
- [I/O — Destinations](#io--destinations)
- [Options & Security Hardening](#options--security-hardening)
- [Error types](#error-types)
- [Strategy & Extensibility Interfaces (SOLID)](#strategy--extensibility-interfaces-solid)

---

## YAML class

Header: `#include "YAML.hpp"`

```cpp
class YAML {
public:
  // Constructors & Rule of Five
  explicit YAML(IStringify* stringify = nullptr, IParser* parser = nullptr);
  explicit YAML(const std::string_view& yamlString);
  explicit YAML(const Options& options);
  explicit YAML(std::pmr::memory_resource* memory_resource);
  YAML(const ArrayInitializer& array);
  YAML(const DictionaryInitializer& dictionary);
  YAML(const YAML& other);
  YAML& operator=(const YAML& other);
  YAML(YAML&& other) noexcept;
  YAML& operator=(YAML&& other) noexcept;
  ~YAML();

  // Deep clone
  [[nodiscard]] std::unique_ptr<YAML> clone() const;

  // Library version string
  [[nodiscard]] static std::string version();

  // Convenience parse helpers (Throwing)
  [[nodiscard]] static std::unique_ptr<YAML> fromString(const std::string_view& yamlString);
  [[nodiscard]] static std::unique_ptr<YAML> load(const std::string_view& yamlString);
#ifdef YAML_LIB_FILE_IO
  [[nodiscard]] static std::unique_ptr<YAML> fromFileToYAML(const std::string_view& fileName);
  [[nodiscard]] static std::unique_ptr<YAML> loadFile(const std::string_view& fileName);
#endif

  // C++23 Monadic parse helpers (std::expected)
#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
  [[nodiscard]] static std::expected<YAML, std::string> loadExpected(const std::string_view& yamlString);
  [[nodiscard]] static std::expected<YAML, std::string> loadExpected(ISource& source);
  [[nodiscard]] std::expected<void, std::string> parseExpected(ISource& source);
  [[nodiscard]] std::expected<void, std::string> parseExpected(ISource&& source);
#endif

  // Parse and stringify
  void parse(ISource& source) const;
  void parse(ISource&& source) const;
#ifndef YAML_LIB_NO_EXCEPTIONS
  [[nodiscard]] bool tryParse(ISource& source, std::string& errorMessage);
  [[nodiscard]] bool tryParse(ISource&& source, std::string& errorMessage);
#endif

  [[nodiscard]] std::string toString() const;
  [[nodiscard]] std::string dump() const;
  [[nodiscard]] std::string dump(const std::string_view& format) const;
  [[nodiscard]] std::string stringify(const std::string_view& format) const;
  void stringify(IDestination& destination) const;
  void stringify(IDestination&& destination) const;
#ifndef YAML_LIB_NO_EXCEPTIONS
  [[nodiscard]] bool tryStringify(IDestination& destination, std::string& errorMessage) const;
  [[nodiscard]] bool tryStringify(IDestination&& destination, std::string& errorMessage) const;
#endif

  // Number of documents parsed
  [[nodiscard]] unsigned long getNumberOfDocuments() const;

  // Access document by zero-based index
  [[nodiscard]] Node& document(unsigned long index);
  [[nodiscard]] const Node& document(unsigned long index) const;

  // Modern Ergonomic Accessors on first document
  template <typename T>
  [[nodiscard]] decltype(auto) as(const std::string_view& key) const;

  template <typename T>
  [[nodiscard]] auto value_or(const std::string_view& key, T&& fallback) const;

  // Subscript operator into the first document
  [[nodiscard]] Node& operator[](const std::string_view& key);
  [[nodiscard]] const Node& operator[](const std::string_view& key) const;
  [[nodiscard]] Node& operator[](std::size_t index);
  [[nodiscard]] const Node& operator[](std::size_t index) const;

  // Tree Traversal
  void traverse(IAction& action);
  void traverse(IAction& action) const;
#ifdef YAML_LIB_SAX_API
  void traverseEvents(IYAMLEvents& handler) const;
#endif

#ifdef YAML_LIB_FILE_IO
  [[nodiscard]] static std::string fromFile(const std::string_view& fileName);
  static void toFile(const std::string_view& fileName,
                     const std::string_view& yamlString,
                     Format format = Format::utf8);
  static Format getFileFormat(const std::string_view& fileName);
#endif

  static void setStrictBooleans(bool strict) noexcept;

  enum class Format : uint8_t { utf8, utf8BOM, utf16BE, utf16LE, utf32BE, utf32LE };
};
```

---

## Node class & Ergonomic Accessors

Header: `#include "YAML_Core.hpp"`

```cpp
struct Node {
  // Constructors & Rule of Five
  Node();
  template <typename T> Node(T&& value);
  Node(const Node& other);
  Node(Node&& other) noexcept;
  Node& operator=(const Node& other);
  Node& operator=(Node&& other) noexcept;

  // Deep clone
  [[nodiscard]] Node clone() const;

  // Dynamic Container Promotion & Subscript
  Node& operator[](const std::string_view& key); // Auto-promotes to Dictionary
  const Node& operator[](const std::string_view& key) const;
  Node& operator[](std::size_t index);           // Auto-promotes to Array
  const Node& operator[](std::size_t index) const;

  // Modern C++23 Ergonomic Interrogation
  template <typename T>
  [[nodiscard]] decltype(auto) as() const;

  template <typename T>
  [[nodiscard]] bool is() const noexcept;

  template <typename T>
  [[nodiscard]] auto value_or(T&& fallback) const;

  template <typename T>
  [[nodiscard]] auto value_or(const std::string_view& key, T&& fallback) const;

  template <typename T>
  [[nodiscard]] std::optional<T> get_if() const noexcept;

  // Range and Structured Bindings Iteration
  [[nodiscard]] auto items();
  [[nodiscard]] auto items() const;
};
```

### Legacy Cast Helpers

For backward compatibility with earlier versions:
```cpp
template<typename T> bool isA(const Node& node);
template<typename T> T& NRef(Node& node);
template<typename T> const T& NRef(const Node& node);
```

---

## Node types

Every value in the tree is represented by one of the following variant structures:

- **`String`**: YAML scalar string with quotation indicator (`kNull`, `kSingleQuote`, `kDoubleQuote`).
- **`Number`**: Numeric scalar stored in narrowest exact type (`int`, `long`, `long long`, `float`, `double`, `long double`).
- **`Boolean`**: Boolean scalar (`true`/`false`).
- **`Null`**: Empty scalar, `null`, or `~`.
- **`Timestamp`**: ISO 8601 date and time representation (`value()`, `toString()`).
- **`Array`**: Sequence of `Node` elements (`size()`, `empty()`, standard iterators `begin()`/`end()`).
- **`Dictionary`**: Key-value mapping (`size()`, `contains()`, `items()`, `find()`).
- **`Document`**: Top-level document wrapper.
- **`Comment`**: Comment node.

---

## Formatting Support (`YAML_Format.hpp`)

Header: `#include "YAML_Format.hpp"`

Provides `std::formatter` template specializations when `<format>` is supported:

```cpp
template <> struct std::formatter<YAML_Lib::Node> : std::formatter<std::string_view>;
template <> struct std::formatter<YAML_Lib::YAML> : std::formatter<std::string_view>;
```

**Usage:**
```cpp
std::string text = std::format("Config: {}", yaml);
std::string entry = std::format("Host is {}", yaml["host"]);
```

---

## Serialization Framework (`YAML_Serialization.hpp`)

Header: `#include "YAML_Serialization.hpp"`

Non-intrusive object serialization and deserialization via ADL.

### Macro

```cpp
YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(TypeName, member1, member2, ...)
```

### Functions

```cpp
template <typename T> void to_yaml(YAML_Lib::Node& node, const T& value);
template <typename T> void from_yaml(const YAML_Lib::Node& node, T& value);
```

### Supported Types
- Primitives (`int`, `double`, `bool`, `std::string`, `std::string_view`, etc.)
- `std::vector<T>`
- `std::map<std::string, T>`
- `std::unordered_map<std::string, T>`
- `std::optional<T>`

---

## Segregated Facade Headers

To minimize include dependencies and compile times, `YAML_Lib` provides segregated facade headers:

- **[`YAML_Reader.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Reader.hpp)**: Exposes only parsing components (`IParser`, `ISource`, `YAML_FileReader`).
- **[`YAML_Writer.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Writer.hpp)**: Exposes only stringification components (`IStringify`, `IDestination`, `YAML_FileWriter`).
- **[`YAML_DOM.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_DOM.hpp)**: Exposes tree structures and nodes (`Node`, `INodeFactory`, `YAML_Core.hpp`).

---

## I/O — Sources

All sources implement `ISource`. Pass by value or lvalue reference to `yaml.parse()`:

- **`BufferSource`**: In-memory string view source (`BufferSource(const std::string_view& buffer)`).
- **`FileSource`**: File-based binary stream source (`FileSource(const std::string_view& filename)`).
- **`StreamSource`**: Generic seekable input stream source (`StreamSource(std::istream& stream)`).

---

## I/O — Destinations

All destinations implement `IDestination`. Pass to `yaml.stringify()`:

- **`BufferDestination`**: In-memory buffer accumulator (`BufferDestination()`, `toString()`).
- **`FileDestination`**: Binary file output writer (`FileDestination(const std::string_view& filename)`).
- **`StreamDestination`**: Generic output stream destination (`StreamDestination(std::ostream& stream)`).

---

## Options & Security Hardening

```cpp
struct Options {
  IStringify* stringifier{nullptr};
  IParser* parser{nullptr};
  bool own_stringifier{true};
  bool own_parser{true};
  std::pmr::memory_resource* memory_resource{nullptr};
  bool strict_booleans{false};
  unsigned long max_documents{32};
  unsigned long max_parse_depth{128};
  unsigned long max_alias_expansions{64};
  unsigned long max_aliases{0};
  unsigned long max_scalar_length{0};
  unsigned long max_collection_size{0};

  [[nodiscard]] static Options secureOptions();
  void validate() const;
};
```

`Options::secureOptions()` returns recommended settings for untrusted input:
- `strict_booleans = true`
- `max_documents = 1`
- `max_parse_depth = 64`
- `max_alias_expansions = 64`
- `max_aliases = 256`
- `max_scalar_length = 64 * 1024`
- `max_collection_size = 1024`

---

## Error types

All exceptions derive from `YAML_Lib::Exception` (which inherits from `std::runtime_error`):

| Exception | Thrown When |
| :--- | :--- |
| `SyntaxError` | Malformed YAML syntax encountered during parsing. |
| `Node::Error` | Invalid node variant conversion or missing element in `NRef`. |
| `ISource::Error` | Read past EOF, corrupted stream, or backup stack underflow. |
| `IDestination::Error` | Output write failure. |

In `-DYAML_LIB_NO_EXCEPTIONS=ON` mode, exceptions are replaced by calls to the panic handler:
```cpp
void Error::setPanicHandler(void (*handler)(const std::string& message));
```

---

## Strategy & Extensibility Interfaces (SOLID)

- **`IDOMParser`**: Interface for vector DOM parsing (`parse(ISource &)`).
- **`ISAXParser`**: Interface for push-based SAX event parsing (`parseSAX(ISource &, IYAMLEvents &)`).
- **`ISchema` / `CoreSchema`**: Strategy for scalar resolution and tag coercions.
- **`INodeFactory` / `DefaultNodeFactory`**: Abstract factory for node variant construction.
- **`StringifierFactory`**: Thread-safe registry for output format creators (`"YAML"`, `"JSON"`, `"XML"`, `"Bencode"`).
- **`DocumentStore`**: Container manager isolating document storage from facade coordination.
