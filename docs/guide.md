# YAML_Lib User Guide

## Table of Contents
- [Introduction](#introduction)
- [Installation](#installation)
- [Parsing YAML](#parsing-yaml)
- [Accessing nodes](#accessing-nodes)
- [Modifying and building YAML](#modifying-and-building-yaml)
- [Stringifying YAML](#stringifying-yaml)
- [Working with multiple documents](#working-with-multiple-documents)
- [Advanced types](#advanced-types)
- [Tags and directives](#tags-and-directives)
- [Anchors, aliases, and merge keys](#anchors-aliases-and-merge-keys)
- [Error handling](#error-handling)
- [Custom I/O — StreamSource and StreamDestination](#custom-io--streamsource-and-streamdestination)
- [Traversal with IAction](#traversal-with-iaction)
- [Alternative output formats](#alternative-output-formats)
- [Examples](#examples)

---

## Introduction

YAML_Lib is a C++20 library for parsing, building, and generating YAML. It represents every YAML value as a typed `Node` in a tree that you can read, modify, and stringify back to text. The library supports the full YAML 1.2 specification including complex anchor/alias patterns, all core schema types, `%YAML` and `%TAG` directives, ISO 8601 timestamps, and pluggable I/O sources and destinations.

---

## Installation

```sh
git clone <repo-url> YAML_Lib
cd YAML_Lib
cmake -S . -B build
cmake --build build
```

Add to your own CMake project:
```cmake
add_subdirectory(YAML_Lib)
target_link_libraries(your_target PRIVATE YAML_Lib)
```

Include the two main headers:
```cpp
#include "YAML.hpp"
#include "YAML_Core.hpp"
using namespace YAML_Lib;
```

---

## Parsing YAML

### From a string buffer
```cpp
YAML yaml;
yaml.parse(BufferSource{"---\nname: Alice\nage: 30\n"});
```

### From a file
```cpp
yaml.parse(FileSource{"config.yaml"});
```

### From any std::istream
```cpp
std::istringstream ss{"key: value\n"};
yaml.parse(StreamSource{ss});
```

`StreamSource` requires the stream to be seekable (`std::istringstream`, `std::ifstream` in binary mode). It is not suitable for non-seekable streams like `std::cin`.

### Convenience: parse a file to string
```cpp
std::string text = YAML::fromFile("config.yaml");
yaml.parse(BufferSource{text});
```

---

## Accessing nodes

After parsing, access the first document with `yaml.document(0)`:

```cpp
const Node& doc = yaml.document(0);
```

### Type-checking with `isA<T>`

Always check the node type before casting:
```cpp
if (isA<String>(doc["name"])) {
    std::string name = NRef<String>(doc["name"]).value();
}
if (isA<Number>(doc["age"])) {
    int age = NRef<Number>(doc["age"]).value<int>();
}
if (isA<Boolean>(doc["active"])) {
    bool active = NRef<Boolean>(doc["active"]).value();
}
if (isA<Null>(doc["nothing"])) {
    // value is null
}
```

### Typed access with `NRef<T>`

`NRef<T>(node)` returns a typed reference. It throws `Node::Error` if the type does not match:
```cpp
std::string city = NRef<String>(doc["address"]["city"]).value();
int count        = NRef<Number>(doc["count"]).value<int>();
double ratio     = NRef<Number>(doc["ratio"]).value<double>();
bool flag        = NRef<Boolean>(doc["flag"]).value();
```

### Dictionary membership test
```cpp
if (NRef<Dictionary>(doc).contains("optional_key")) {
    // safe to access
}
```

### Array access
```cpp
std::size_t n = NRef<Array>(doc["scores"]).size();
for (std::size_t i = 0; i < n; ++i) {
    int score = NRef<Number>(doc["scores"][i]).value<int>();
}
```

### Number precision

`Number` stores values using the narrowest type that fits the literal.  
Use `.is<T>()` to query and `.value<T>()` to convert:
```cpp
NRef<Number>(node).is<int>()        // stored as int?
NRef<Number>(node).value<long>()    // convert to long
NRef<Number>(node).value<double>()  // convert to double
```

---

## Modifying and building YAML

### Assign scalar values
```cpp
YAML yaml;
yaml["name"]  = "Bob";
yaml["score"] = 42;
yaml["ratio"] = 3.14;
yaml["valid"] = true;
yaml["empty"] = nullptr;
```

### Build an array
```cpp
yaml["items"] = {1, 2, 3};
```

### Build a dictionary
```cpp
yaml["address"] = {{"city", "London"}, {"zip", "EC1A"}};
```

### Nest structures with `Node{}`
```cpp
yaml["profile"] = {
    {"name", "Alice"},
    {"scores", Node{95, 87, 92}},
    {"address", Node{{"city", "London"}, {"zip", "EC1A"}}}
};
```

### Construct from initializer list
```cpp
const YAML yaml = {
    {"pi",    3.141},
    {"name",  "Niels"},
    {"list",  Node{1, 0, 2}},
    {"obj",   Node{{"key", "val"}}}
};
```

---

## Stringifying YAML

### To a string buffer
```cpp
BufferDestination dest;
yaml.stringify(dest);
std::string output = dest.toString();
```

### To a file
```cpp
yaml.stringify(FileDestination{"output.yaml"});
```

To write with a specific encoding:
```cpp
YAML::toFile("output.yaml", dest.toString(), YAML::Format::utf8BOM);
```

### To any std::ostream
```cpp
std::ostringstream ss;
yaml.stringify(StreamDestination{ss});
std::cout << ss.str();

// Or directly to cout
yaml.stringify(StreamDestination{std::cout});
```

---

## Working with multiple documents

```cpp
YAML yaml;
yaml.parse(BufferSource{
    "---\nfirst: 1\n"
    "---\nsecond: 2\n"
});

unsigned long n = yaml.getNumberOfDocuments();  // 2
const Node& doc0 = yaml.document(0);
const Node& doc1 = yaml.document(1);
```

---

## Advanced types

### Timestamps

YAML_Lib automatically recognises ISO 8601 date and datetime strings and creates `Timestamp` nodes:

```cpp
yaml.parse(BufferSource{"---\ncreated: 2024-04-05\n"});
if (isA<Timestamp>(yaml.document(0)["created"])) {
    std::string ts = NRef<Timestamp>(yaml.document(0)["created"]).value();
    // ts == "2024-04-05"
}
```

Supported formats:
- `YYYY-MM-DD`
- `YYYY-MM-DDThh:mm:ssZ`
- `YYYY-MM-DDThh:mm:ss±hh:mm`

### Special float values
```cpp
yaml.parse(BufferSource{"---\n- .inf\n- -.inf\n- .nan\n"});
```
Parsed as `std::numeric_limits<double>::infinity()`, `-infinity()`, and `quiet_NaN()`.

### Octal and hex integers
```cpp
yaml.parse(BufferSource{"---\nhex: 0xFF\noct: 0o17\n"});
// hex → 255, oct → 15
```

---

## Tags and directives

### Standard `!!` tags

Force a specific type regardless of the literal value:
```yaml
id:      !!str 007        # stored as String "007"
count:   !!int "99"       # stored as Number 99
enabled: !!bool yes       # stored as Boolean true
nothing: !!null ~         # stored as Null
```

```cpp
NRef<String>(doc["id"]).value()           // "007"
NRef<Number>(doc["count"]).value<int>()   // 99
```

### Custom and verbatim tags
```yaml
- !mytag foo          # custom tag
- !<tag:example.com>  # verbatim URI tag
```
Access the tag string:
```cpp
std::string tag = node.getVariant().getTag();
```

### `%TAG` named handles

```yaml
%TAG !e! tag:example.com,2024:
---
item: !e!widget foo   # expands to tag:example.com,2024:widget
```

### `%YAML` directive

```yaml
%YAML 1.2
---
key: value
```
Only YAML major version 1 is supported; a different major version throws `SyntaxError`.

---

## Anchors, aliases, and merge keys

```yaml
---
defaults: &defaults
  timeout: 30
  retries: 3

production:
  <<: *defaults        # merge key — copies timeout and retries
  host: prod.example
```

```cpp
int timeout = NRef<Number>(doc["production"]["timeout"]).value<int>();  // 30
```

Multi-source merge with priority:
```yaml
<<: [*base, *overrides]   # overrides takes priority over base
```

---

## Error handling

### Catch parse errors
```cpp
try {
    yaml.parse(BufferSource{"bad: [\n"});
} catch (const SyntaxError& ex) {
    std::cerr << "Parse error: " << ex.what() << "\n";
    // e.g. "YAML Syntax Error [Line: 1 Column: 8]: ..."
}
```

### Catch type errors
```cpp
try {
    int n = NRef<Number>(stringNode).value<int>();   // throws
} catch (const Node::Error& ex) {
    std::cerr << "Type error: " << ex.what() << "\n";
}
```

### Defensive patterns
```cpp
// Check type before accessing
if (isA<Number>(doc["count"])) {
    int count = NRef<Number>(doc["count"]).value<int>();
}

// Check key existence before indexing a dictionary
if (NRef<Dictionary>(doc).contains("optional")) {
    auto& v = doc["optional"];
}
```

---

## Custom I/O — StreamSource and StreamDestination

For integration with existing C++ stream pipelines:

```cpp
// Parse from a network or pipe stream
void parseFromStream(std::istream& input) {
    YAML yaml;
    yaml.parse(StreamSource{input});
    // ...
}

// Stringify into an existing output stream
void stringifyToStream(const YAML& yaml, std::ostream& output) {
    yaml.stringify(StreamDestination{output});
}
```

For custom sources beyond the built-in three, implement `ISource`. For custom output, implement `IDestination`. See `classes/include/interface/ISource.hpp` and `IDestination.hpp`.

---

## Traversal with IAction

Implement `IAction` to visit every node in the tree:

```cpp
struct MyVisitor : public IAction {
    void onNode(Node& node) override {
        if (isA<String>(node)) {
            // process string node
        }
    }
};

MyVisitor v;
yaml.traverse(v);
```

---

## Alternative output formats

YAML_Lib ships with built-in stringifiers for JSON, XML, and Bencode. Pass a stringifier to the `YAML` constructor:

```cpp
// JSON output (example — using built-in JSON stringifier)
// See examples/source/YAML_Files_To_JSON.cpp for details.
```

Each stringifier implements `IStringify`. To use one:
```cpp
YAML yaml{new YourStringify()};
yaml.parse(BufferSource{yamlText});
BufferDestination dest;
yaml.stringify(dest);
```

---

## Examples

All programs are in `examples/source/`. Build them with the main CMake build.

| Program | What it shows |
|---------|---------------|
| `YAML_Simple_Read_Write.cpp` | Parse a file, write to another file |
| `YAML_Parse_File.cpp` | Batch parse with timing measurements |
| `YAML_Create_At_Runtime.cpp` | Build YAML in code; initializer lists |
| `YAML_Nested_Structure_Demo.cpp` | Deep nesting, traversal |
| `YAML_Files_To_JSON.cpp` | Reformat YAML as JSON |
| `YAML_Files_To_XML.cpp` | Reformat YAML as XML |
| `YAML_Files_To_Bencode.cpp` | Reformat YAML as Bencode |
| `YAML_Analyze_File.cpp` | Inspect structure and statistics |
| `YAML_Fibonacci.cpp` | Build a sequence programmatically |
| `YAML_Error_Handling_Demo.cpp` | All error handling patterns |
| `YAML_Advanced_Types_Demo.cpp` | Timestamps, `%TAG`, binary, anchors, merge keys, multi-doc |

---

For a complete method listing see the [API Reference](api.md).

