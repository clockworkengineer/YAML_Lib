---

## See Also

- [User Guide](guide.md)
- [README](../README.md)
# YAML_Lib API Reference

All public symbols live in the `YAML_Lib` namespace. Include `YAML.hpp` and `YAML_Core.hpp`.

## Table of Contents
- [YAML class](#yaml-class)
- [Node types](#node-types)
- [Node access helpers](#node-access-helpers)
- [I/O — Sources](#io--sources)
- [I/O — Destinations](#io--destinations)
- [Error types](#error-types)
- [Free functions](#free-functions)

---

## YAML class

```cpp
class YAML {
public:
  // Constructors
  explicit YAML(IStringify* stringify = nullptr, IParser* parser = nullptr);
  explicit YAML(const std::string_view& yamlString);  // parse immediately
  YAML(const ArrayInitializer& array);
  YAML(const DictionaryInitializer& dictionary);

  // Library version string
  static std::string version();

  // Number of documents parsed from the last parse() call
  unsigned long getNumberOfDocuments() const;

  // Parse YAML from a source
  void parse(ISource& source) const;
  void parse(ISource&& source) const;

  // Stringify the node tree to a destination
  void stringify(IDestination& destination) const;
  void stringify(IDestination&& destination) const;

  // Access document by zero-based index
  Node& document(unsigned long index);
  const Node& document(unsigned long index) const;

  // Traverse the entire tree
  void traverse(IAction& action);
  void traverse(IAction& action) const;

  // Index into the first document by key or position
  Node& operator[](const std::string_view& key);
  Node& operator[](std::size_t index);

  // Static file helpers
  static std::string fromFile(const std::string_view& fileName);
  static void toFile(const std::string_view& fileName,
                     const std::string_view& yamlString,
                     Format format = Format::utf8);
  static Format getFileFormat(const std::string_view& fileName);

  enum class Format : uint8_t { utf8, utf8BOM, utf16BE, utf16LE, utf32BE, utf32LE };
};
```

### Initializer list types

| Type alias | Description |
|------------|-------------|
| `ArrayInitializer` | `std::initializer_list<InitializerListTypes>` |
| `DictionaryInitializer` | `std::initializer_list<std::pair<std::string, InitializerListTypes>>` |
| `InitializerListTypes` | `std::variant<int, long, long long, float, double, long double, bool, std::string, std::nullptr_t, Node>` |

---

## Node types

Every value in the tree is a `Node` holding one of the following variant types.

### `String`
Represents a YAML scalar string.
```cpp
NRef<String>(node).value()          // → std::string
NRef<String>(node).getQuote()       // → char: kNull (plain), kDoubleQuote, kApostrophe
NRef<String>(node).getQuote()       // set via setValue / construction
```

### `Number`
Represents any numeric scalar (integer or floating-point).
```cpp
NRef<Number>(node).value<int>()         // convert to int
NRef<Number>(node).value<double>()      // convert to double
NRef<Number>(node).is<int>()            // true if stored as int
NRef<Number>(node).is<double>()         // true if stored as double
```
Stored type is the narrowest type that fits the literal: `int`, `long`, `long long`, `float`, `double`, or `long double`.

### `Boolean`
```cpp
NRef<Boolean>(node).value()    // → bool
```
Accepted literals: `true`/`false`, `yes`/`no`, `on`/`off` (case-insensitive).

### `Null`
Represents `null`, `~`, or an empty scalar.
```cpp
isA<Null>(node)   // type check only; no data
```

### `Timestamp`
ISO 8601 date or datetime; also created by `!!timestamp` tag.
```cpp
NRef<Timestamp>(node).value()   // → std::string (raw value as parsed)
```

### `Array`
```cpp
NRef<Array>(node).size()        // → std::size_t
node[0]                         // → Node& (first element)
```

### `Dictionary`
```cpp
NRef<Dictionary>(node).contains("key")   // → bool
NRef<Dictionary>(node).size()            // → std::size_t
node["key"]                              // → Node&
```

### `Document`
Wraps a top-level YAML document returned by `yaml.document(n)`.

### `Comment`
Represents a YAML comment node (read-only; not round-tripped).

---

## Node access helpers

```cpp
// Type check — returns true if node holds variant T
template<typename T>
bool isA(const Node& node);

// Typed reference — throws Node::Error if the type does not match
template<typename T>
T& NRef(Node& node);

template<typename T>
const T& NRef(const Node& node);
```

**Example:**
```cpp
if (isA<String>(node)) {
    const std::string& s = NRef<String>(node).value();
}
```

---

## I/O — Sources

All sources implement `ISource`. Pass by value (rvalue) or lvalue reference to `yaml.parse()`.

### `BufferSource`
```cpp
explicit BufferSource(const std::string_view& buffer);
```
Parse from an in-memory string.

### `FileSource`
```cpp
explicit FileSource(const std::string_view& filename);
```
Parse from a file opened in binary mode.

### `StreamSource`
```cpp
explicit StreamSource(std::istream& stream);
```
Parse from any seekable input stream (`std::istringstream`, `std::ifstream`, etc.).  
The stream must support `seekg`/`tellg` (non-seekable streams like `std::cin` are not supported).

**Common `ISource` methods** (also usable directly for custom tokenisation):

| Method | Description |
|--------|-------------|
| `char current()` | Peek at the current character |
| `void next()` | Advance by one character |
| `bool more()` | Returns `false` at EOF |
| `void reset()` | Seek back to the beginning |
| `std::size_t position()` | Current byte offset |
| `void save()` | Push position onto a stack |
| `void restore()` | Pop and seek back |
| `bool match(std::string_view)` | Consume the string if present; returns `true` if matched |

---

## I/O — Destinations

All destinations implement `IDestination`. Pass by value or lvalue reference to `yaml.stringify()`.

### `BufferDestination`
```cpp
BufferDestination();
std::string toString() const;   // retrieve accumulated output
std::size_t size() const;
void clear();
```

### `FileDestination`
```cpp
explicit FileDestination(const std::string_view& filename);
void close();
```
Writes in binary mode with `\r\n` line endings on Windows.

### `StreamDestination`
```cpp
explicit StreamDestination(std::ostream& stream);
```
Write to any output stream (`std::ostringstream`, `std::ofstream`, `std::cout`, etc.).

**Common `IDestination` methods:**

| Method | Description |
|--------|-------------|
| `void add(char)` | Append a single character |
| `void add(const std::string&)` | Append a string |
| `void add(const char*)` | Append a C-string |
| `void add(std::string_view)` | Append a string view |
| `char last()` | Last character written (`kNull` if nothing written or after `clear()`) |
| `void clear()` | Reset the destination |

---

## Error types

| Type | Base | When thrown |
|------|------|-------------|
| `SyntaxError` | `std::runtime_error` | Malformed YAML input during `parse()` |
| `Node::Error` | `std::runtime_error` | Wrong type in `NRef<T>()`, missing document, etc. |
| `ISource::Error` | `std::runtime_error` | Read past EOF, bad stream state, backup underflow |
| `IDestination::Error` | `std::runtime_error` | Write failure |

---

## Free functions

```cpp
// YAML_Lib::YAML static helpers
std::string YAML::fromFile(const std::string_view& fileName);
void        YAML::toFile(const std::string_view& fileName,
                          const std::string_view& yamlString,
                          YAML::Format format = YAML::Format::utf8);
YAML::Format YAML::getFileFormat(const std::string_view& fileName);
```

`fromFile` reads the entire file into a `std::string`.  
`getFileFormat` inspects the byte-order mark (BOM) to detect UTF-8/UTF-16/UTF-32 encoding.

---

For practical examples see the [User Guide](guide.md) or the `examples/` directory.

