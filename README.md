# YAML_Lib

YAML_Lib is a lightweight, header-friendly C++20 library for parsing, manipulating, and generating YAML. It converts YAML text into a typed node tree that can be interrogated, modified, and stringified back to YAML — or to JSON, XML, and Bencode via pluggable stringifiers.

## Features

### YAML Support
- **YAML 1.2 scalar types** — strings (plain, single-quoted, double-quoted), integers (decimal, hex `0x`, octal `0o`), floats (including `.inf`, `-.inf`, `.nan`), booleans (`true`/`false`/`yes`/`no`/`on`/`off`), null (`~`/`null`)
- **Block scalars** — literal `|` and folded `>` with all chomping indicators (`|`, `|-`, `|+`, `>`, `>-`, `>+`)
- **Collections** — block and flow arrays, block and flow mappings, deeply nested structures
- **Anchors & aliases** — `&anchor` / `*alias`, merge key `<<`
- **Multiple documents** — `---` / `...` document boundaries in a single stream
- **Tags** — `!!str`, `!!int`, `!!float`, `!!bool`, `!!null`, `!!seq`, `!!map`, `!!timestamp`, `!!binary`, custom `!tag`, verbatim `!<uri>`, named handles via `%TAG`
- **Directives** — `%YAML X.Y` version checking, `%TAG handle prefix` named handle expansion; unknown directives silently ignored
- **Timestamps** — automatic detection and parsing of ISO 8601 dates and datetimes; dedicated `Timestamp` node type
- **Explicit mapping keys** — `? key\n: value` block form
- **Unicode** — full YAML 1.2 escape set including `\U` 8-digit SMP codepoints

### Library Design
- **Extensible I/O** — parse from `BufferSource`, `FileSource`, or `StreamSource` (`std::istream&`); stringify to `BufferDestination`, `FileDestination`, or `StreamDestination` (`std::ostream&`)
- **Pluggable stringifiers** — built-in YAML, JSON, XML, and Bencode output; custom stringifiers via `IStringify`
- **Traversal** — visitor pattern via `IAction` for tree-wide operations
- **Exception-based error reporting** — `SyntaxError` on malformed input; `Node::Error` on type violations

## Getting Started

### Requirements
- C++20 or newer (GCC, Clang, MSVC all supported)
- CMake 3.18+
- No runtime dependencies beyond the C++ standard library

### Build

```sh
git clone <repo-url> YAML_Lib
cd YAML_Lib
cmake -S . -B build
cmake --build build
```

On Windows with Visual Studio:
```sh
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### Integrate with CMake

```cmake
add_subdirectory(YAML_Lib)
target_link_libraries(your_target PRIVATE YAML_Lib)
```

### Build options

CMake options supported by YAML_Lib:

- `YAML_LIB_NO_EXCEPTIONS=ON` — disable C++ exceptions and use the panic handler path.
- `YAML_LIB_FILE_IO=ON` — enable file I/O support for `FileSource`, `FileDestination`, `YAML::fromFile()`, `YAML::toFile()`, and `YAML::getFileFormat()`.
- `YAML_LIB_SAX_API=ON` — enable SAX-style event parsing via `IYAMLEvents` and `YAML::traverseEvents()`.
- `YAML_LIB_TIMESTAMP_PARSE=ON` — enable timestamp parsing helpers for ISO 8601 date/time values.

Example:

```sh
cmake -S . -B build -DYAML_LIB_NO_EXCEPTIONS=OFF -DYAML_LIB_FILE_IO=ON -DYAML_LIB_SAX_API=ON
cmake --build build
```

See `docs/public_api.md` for the installed public header set.

### Runtime configuration with `YAML::Options`

`YAML::Options` lets you configure parser/stringifier choice, memory resource usage, and strict boolean parsing at runtime:

```cpp
#include "YAML.hpp"
#include "YAML_Core.hpp"
using namespace YAML_Lib;

YAML::Options options;
options.strictBooleans = true;
options.memoryResource = std::pmr::get_default_resource();
options.maxDocuments = 4;        // limit document count for untrusted input
options.maxParseDepth = 64;      // prevent deeply nested input from exhausting the parser
options.maxAliasExpansions = 128; // avoid alias explosion attacks

YAML yaml(options);
yaml.parse(BufferSource{"---\nvalue: yes\n"});

const auto &doc = yaml.document(0);
// strict booleans preserves 'yes' as a string rather than a boolean
std::string value = NRef<String>(doc["value"]).value();
```

## Quick Start

### Parse YAML from a string

```cpp
#include "YAML.hpp"
#include "YAML_Core.hpp"
using namespace YAML_Lib;

YAML yaml;
yaml.parse(BufferSource{"---\nname: Alice\nage: 30\n"});

const auto &doc = yaml.document(0);
std::string name = NRef<String>(doc["name"]).value();   // "Alice"
int age = NRef<Number>(doc["age"]).value<int>();         // 30
```

### Parse YAML from a file

```cpp
yaml.parse(FileSource{"config.yaml"});
```

### Parse YAML from any std::istream

```cpp
std::istringstream ss{"key: value\n"};
yaml.parse(StreamSource{ss});
```

### Stringify to a string buffer

```cpp
BufferDestination dest;
yaml.stringify(dest);
std::string output = dest.toString();
```

### Stringify to a file

```cpp
yaml.stringify(FileDestination{"output.yaml"});
```

### Stringify to any std::ostream

```cpp
std::ostringstream ss;
yaml.stringify(StreamDestination{ss});
```

### Build YAML programmatically

```cpp
YAML yaml;
yaml["name"]    = "Alice";
yaml["age"]     = 30;
yaml["scores"]  = {95, 87, 92};
yaml["address"] = {{"city", "London"}, {"zip", "EC1A"}};

BufferDestination dest;
yaml.stringify(dest);
```

### Type-safe node access

```cpp
// Check type before accessing
if (isA<Number>(doc["age"])) {
    int age = NRef<Number>(doc["age"]).value<int>();
}
if (isA<Dictionary>(doc)) {
    bool hasKey = NRef<Dictionary>(doc).contains("name");
}
```

### Error handling

```cpp
try {
    yaml.parse(BufferSource{"bad: [\n"});
} catch (const SyntaxError &ex) {
    std::cerr << ex.what() << "\n";
}
```

## Node Types

| Type | `isA<T>` check | Access |
|------|---------------|--------|
| `String` | `isA<String>(node)` | `NRef<String>(node).value()` |
| `Number` | `isA<Number>(node)` | `NRef<Number>(node).value<int>()` |
| `Boolean` | `isA<Boolean>(node)` | `NRef<Boolean>(node).value()` |
| `Null` | `isA<Null>(node)` | — |
| `Timestamp` | `isA<Timestamp>(node)` | `NRef<Timestamp>(node).value()` |
| `Array` | `isA<Array>(node)` | `node[0]`, `NRef<Array>(node).size()` |
| `Dictionary` | `isA<Dictionary>(node)` | `node["key"]`, `NRef<Dictionary>(node).contains("key")` |

## I/O Sources and Destinations

| Class | Direction | Backed by |
|-------|-----------|-----------|
| `BufferSource` | input | `std::string` / `std::string_view` |
| `FileSource` | input | file path (binary mode) |
| `StreamSource` | input | any seekable `std::istream&` |
| `BufferDestination` | output | internal `std::string` buffer |
| `FileDestination` | output | file path |
| `StreamDestination` | output | any `std::ostream&` |

## Examples

All examples are in `examples/source/`:

| Program | Description |
|---------|-------------|
| `YAML_Simple_Read_Write.cpp` | Parse a YAML file, stringify back to a new file |
| `YAML_Parse_File.cpp` | Parse files from a directory with timing measurements |
| `YAML_Create_At_Runtime.cpp` | Build a YAML document in code using initializer lists |
| `YAML_Nested_Structure_Demo.cpp` | Work with deeply nested YAML structures |
| `YAML_Files_To_JSON.cpp` | Convert YAML files to JSON |
| `YAML_Files_To_XML.cpp` | Convert YAML files to XML |
| `YAML_Files_To_Bencode.cpp` | Convert YAML files to Bencode |
| `YAML_Analyze_File.cpp` | Parse and inspect YAML structure/statistics |
| `YAML_Fibonacci.cpp` | Build a YAML sequence of Fibonacci numbers |
| `YAML_Error_Handling_Demo.cpp` | Best-practice error handling patterns |
| `YAML_Advanced_Types_Demo.cpp` | Timestamps, `%TAG` handles, verbatim tags, binary, anchors, merge keys, multi-doc |

## Documentation

- [API Reference](docs/api.md)
- [User Guide](docs/guide.md)
- [Feature Plan](docs/PARSER_FEATURES_AND_PLAN.md)
- [YAML 1.2.2 Specification](https://yaml.org/spec/1.2.2/)

## Contributing

1. Fork the repository.
2. Create a branch: `git checkout -b feature-name`
3. Commit your changes: `git commit -m "Description of change"`
4. Push and open a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
