# YAML_Lib User Guide

`YAML_Lib` is a modern, lightweight, header-friendly C++23 library for parsing, manipulating, serializing, and generating YAML. It represents every YAML value as a typed `Node` in a tree that you can read, modify, and stringify back to text — or to JSON, XML, and Bencode via pluggable stringifiers. The library supports the full YAML 1.2 specification including complex anchor/alias graphs, core schema types, `%YAML` and `%TAG` directives, ISO 8601 timestamps, and pluggable I/O sources and destinations.

---

## Table of Contents

- [Troubleshooting & Compiler Support](#troubleshooting--compiler-support)
- [Installation & Integration](#installation--integration)
- [Library Design Principles](#library-design-principles)
- [Parsing YAML](#parsing-yaml)
  - [Throwing vs Monadic Parsing (`std::expected`)](#throwing-vs-monadic-parsing-stdexpected)
  - [Parsing from Buffers, Files, and Streams](#parsing-from-buffers-files-and-streams)
- [Accessing Nodes](#accessing-nodes)
  - [Modern Ergonomic Interrogation (`as`, `is`, `value_or`, `get_if`)](#modern-ergonomic-interrogation-as-is-value_or-get_if)
  - [Structured Bindings & Range Iteration](#structured-bindings--range-iteration)
  - [Advanced Variant Access (`isA`, `NRef`)](#advanced-variant-access-isa-nref)
- [Modifying and Building YAML](#modifying-and-building-yaml)
  - [Dynamic Container Auto-Promotion](#dynamic-container-auto-promotion)
  - [Initializer Lists & Hierarchical Construction](#initializer-lists--hierarchical-construction)
- [Stringifying & Formatting YAML](#stringifying--formatting-yaml)
  - [Standard Stringification & File Output](#standard-stringification--file-output)
  - [`std::format` String Formatting](#stdformat-string-formatting)
- [Object & Container Serialization (`YAML_Serialization.hpp`)](#object--container-serialization-yaml_serializationhpp)
- [Working with Multiple Documents](#working-with-multiple-documents)
- [Advanced Types, Tags, and Directives](#advanced-types-tags-and-directives)
- [Anchors, Aliases, and Merge Keys](#anchors-aliases-and-merge-keys)
- [Error Handling & Security Hardening](#error-handling--security-hardening)
- [Extensibility & Custom I/O](#extensibility--custom-io)
- [SAX Event Traversal](#sax-event-traversal)
- [Alternative Output Formats (JSON, XML, Bencode)](#alternative-output-formats-json-xml-bencode)
- [Example Programs](#example-programs)

---

## Troubleshooting & Compiler Support

### Compiler Requirements
- **Linux**: GCC 13.0+ or Clang 17.0+
- **macOS**: Apple Clang 16.0+ (Xcode 16+)
- **Windows**: MSVC 2022 v17.8+ (toolset v143)

### Common Issues
- **Missing C++23 features (`<expected>`, `<format>`)**: Verify your compiler standard flag is set to `-std=c++23` (or `/std:c++latest` / `/std:c++23` on MSVC).
- **Type conversions on Node**: Use `node.as<T>()` for automatic type conversion or `node.value_or("key", fallback)` to gracefully handle missing fields.
- **Clang on Linux with GCC 14 libstdc++ (LLVM #93734)**: If compiling with Clang against GCC 14 standard headers in C++23 mode, CMake automatically detects and uses GCC 13 toolchain directories when available.
- **File I/O Disabled**: If `loadFile()` or `FileSource` are undefined, ensure `YAML_LIB_FILE_IO=ON` (the default) in your CMake configuration.

---

## Installation & Integration

### Building with CMake

```sh
git clone https://github.com/clockworkengineer/YAML_Lib.git
cd YAML_Lib
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
sudo cmake --install build
```

### Adding to your CMake Project

```cmake
find_package(YAML_Lib REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE YAML_Lib::YAML_Lib)
```

### Main Headers
```cpp
#include "YAML.hpp"
#include "YAML_Core.hpp"
// Optional modern modules:
#include "YAML_Format.hpp"        // std::format support
#include "YAML_Serialization.hpp" // Non-intrusive object serialization
```

---

## Library Design Principles

`YAML_Lib` is engineered around:
- **100% SOLID Architecture**: Single-responsibility lexers, document stores, schema strategies (`ISchema`), and factory strategies (`INodeFactory`).
- **Segregated Facade Headers**: Include only what you need: [`YAML_Reader.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Reader.hpp) for parsing, [`YAML_Writer.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Writer.hpp) for stringification, or [`YAML_DOM.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_DOM.hpp) for tree manipulation.
- **Zero Runtime Dependencies**: Depends strictly on the C++ standard library.
- **Security Hardening**: Defensive defaults via `Options::secureOptions()` to safeguard against document flooding and entity explosion attacks.

---

## Parsing YAML

### Throwing vs Monadic Parsing (`std::expected`)

#### 1. Modern Monadic Parsing (Recommended in C++23)
Non-throwing, functional error handling using `std::expected`:

```cpp
auto result = YAML_Lib::YAML::loadExpected("server:\n  host: 127.0.0.1\n  port: 8080\n");
if (result) {
    YAML_Lib::YAML& doc = *result;
    std::cout << "Host: " << doc["server"]["host"].as<std::string>() << "\n";
} else {
    std::cerr << "Parse error: " << result.error() << "\n";
}
```

#### 2. Traditional Throwing Parsing
Throws `YAML_Lib::SyntaxError` on malformed YAML:

```cpp
try {
    auto doc = YAML_Lib::YAML::fromString("name: Alice\nage: 30\n");
} catch (const YAML_Lib::SyntaxError& e) {
    std::cerr << "Syntax error: " << e.what() << "\n";
}
```

### Parsing from Buffers, Files, and Streams

```cpp
// 1. From an in-memory string view
YAML_Lib::YAML yaml1;
yaml1.parse(YAML_Lib::BufferSource{"key: value\n"});

// 2. Direct file parse helper
std::unique_ptr<YAML_Lib::YAML> yaml2 = YAML_Lib::YAML::loadFile("config.yaml");

// 3. From standard input stream (std::ifstream, std::istringstream)
std::ifstream file("config.yaml", std::ios::binary);
YAML_Lib::YAML yaml3;
yaml3.parse(YAML_Lib::StreamSource{file});
```

---

## Accessing Nodes

### Modern Ergonomic Interrogation (`as`, `is`, `value_or`, `get_if`)

`YAML_Lib` 1.4.0 provides intuitive, type-safe member accessors directly on `Node` and `YAML`:

```cpp
YAML_Lib::YAML doc("host: localhost\nport: 8080\nactive: true\nload: 0.85\n");

// Direct type conversion
std::string host = doc["host"].as<std::string>();
int port         = doc["port"].as<int>();
bool active      = doc["active"].as<bool>();
double load      = doc["load"].as<double>();

// Type queries without exceptions
if (doc["port"].is<int>()) {
    std::cout << "Port is an integer.\n";
}

// Fallbacks for optional or missing fields
int timeout = doc.value_or("timeout", 30);
std::string env = doc["env"].value_or("production");

// Optional extraction with std::optional
if (auto optPort = doc["port"].get_if<int>()) {
    std::cout << "Configured port: " << *optPort << "\n";
}

// Top-level document shortcuts
int p = doc.as<int>("port");
```

### Structured Bindings & Range Iteration

Mappings and arrays provide standard C++ iteration interfaces:

#### 1. Dictionary Structured Bindings (`items()`)
```cpp
YAML_Lib::YAML yaml("workers: 4\nthreads: 16\nqueue: 1024\n");

for (auto&& [key, value] : yaml.document(0).items()) {
    std::cout << key << " = " << value.as<int>() << "\n";
}
```

#### 2. Sequence Range-Based Loops
```cpp
YAML_Lib::YAML yaml("ports: [80, 443, 8080]\n");

auto& arr = YAML_Lib::NRef<YAML_Lib::Array>(yaml["ports"]);
for (auto&& item : arr) {
    std::cout << "Port: " << item.as<int>() << "\n";
}
```

### Advanced Variant Access (`isA`, `NRef`)

For low-level variant inspection:
```cpp
const auto& node = doc["host"];
if (YAML_Lib::isA<YAML_Lib::String>(node)) {
    const std::string& val = YAML_Lib::NRef<YAML_Lib::String>(node).value();
}
```

---

## Modifying and Building YAML

### Dynamic Container Auto-Promotion

Assigning to a subscript on a default-constructed (or `Null`) node automatically promotes it to a `Dictionary` or `Array`:

```cpp
YAML_Lib::Node root; // Null initially

// Automatically promotes root to Dictionary:
root["database"]["host"] = "db.internal";
root["database"]["port"] = 5432;

// Automatically promotes "replicas" to Array:
root["database"]["replicas"][0] = "db-replica-1";
root["database"]["replicas"][1] = "db-replica-2";
```

### Initializer Lists & Hierarchical Construction

```cpp
YAML_Lib::YAML doc = {
    {"service", "Payments"},
    {"version", 1.4},
    {"enabled", true},
    {"endpoints", YAML_Lib::Node{"/charge", "/refund", "/status"}},
    {"metadata", YAML_Lib::Node{{"env", "prod"}, {"dc", "us-east"}}}
};
```

---

## Stringifying & Formatting YAML

### Standard Stringification & File Output

```cpp
// Direct string dump
std::string text = doc.dump();

// Stringify to an in-memory buffer
YAML_Lib::BufferDestination dest;
doc.stringify(dest);
std::string bufferOutput = dest.toString();

// Write directly to file
YAML_Lib::FileDestination fileDest("output.yaml");
doc.stringify(fileDest);

// Write to any std::ostream
doc.stringify(YAML_Lib::StreamDestination{std::cout});
```

### `std::format` String Formatting

Include [`YAML_Format.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Format.hpp) to enable native standard library formatting:

```cpp
#include "YAML.hpp"
#include "YAML_Format.hpp"
#include <format>

std::string summary = std::format("App: {}, Config: {}", doc["service"], doc);
```

---

## Object & Container Serialization (`YAML_Serialization.hpp`)

`YAML_Lib` provides non-intrusive struct and container serialization via [`YAML_Serialization.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Serialization.hpp):

```cpp
#include "YAML.hpp"
#include "YAML_Serialization.hpp"

struct ClientConfig {
    std::string endpoint;
    int timeout_ms{5000};
    std::vector<std::string> headers;
};
YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(ClientConfig, endpoint, timeout_ms, headers)

int main() {
    ClientConfig client{"https://api.service.io", 3000, {"Auth: Bearer 123"}};

    // Serialize to Node
    YAML_Lib::Node node;
    YAML_Lib::to_yaml(node, client);

    // Deserialize from Node
    ClientConfig restored;
    YAML_Lib::from_yaml(node, restored);
}
```

See the dedicated [Object Serialization Guide](serialization.md) for nested structs, maps, optionals, and custom overloads.

---

## Working with Multiple Documents

A single stream may contain multiple YAML documents separated by `---` or `...`:

```cpp
YAML_Lib::YAML stream;
stream.parse(YAML_Lib::BufferSource{
    "---\nservice: auth\n"
    "---\nservice: billing\n"
});

std::cout << "Documents parsed: " << stream.getNumberOfDocuments() << "\n"; // 2
std::cout << "Doc 0 service: " << stream.document(0)["service"].as<std::string>() << "\n";
std::cout << "Doc 1 service: " << stream.document(1)["service"].as<std::string>() << "\n";
```

---

## Advanced Types, Tags, and Directives

### Timestamps
ISO 8601 timestamps are parsed into `Timestamp` nodes automatically:
```cpp
YAML_Lib::YAML doc("started: 2026-10-01T15:30:00Z\n");
if (doc["started"].is<YAML_Lib::Timestamp>()) {
    std::cout << "Date: " << doc["started"].as<std::string>() << "\n";
}
```

### Explicit Tags
```yaml
id: !!str 007
count: !!int "42"
```

---

## Anchors, Aliases, and Merge Keys

Full YAML 1.2 anchor and merge key (`<<`) support:

```yaml
defaults: &defaults
  timeout: 30
  retries: 3

production:
  <<: *defaults
  host: prod.domain.internal
```

```cpp
YAML_Lib::YAML doc(rawYaml);
int timeout = doc["production"]["timeout"].as<int>(); // 30
```

---

## Error Handling & Security Hardening

### Secure Parser Options for Untrusted Input

```cpp
YAML_Lib::Options opts = YAML_Lib::Options::secureOptions();
// Enforces strict booleans, max depth (64), max alias expansions (64), and scalar size caps.
YAML_Lib::YAML secureParser(opts);
```

### Exception-Free Embedded Hardening

When compiled with `-DYAML_LIB_NO_EXCEPTIONS=ON`, register a custom panic handler:

```cpp
YAML_Lib::Error::setPanicHandler([](const std::string& err) {
    std::cerr << "CRITICAL PARSE ERROR: " << err << "\n";
});
```

See the [Embedded & Minimal Systems Guide](embedded_and_minimal.md) for full instructions.

---

## Extensibility & Custom I/O

- **Custom Sources**: Implement `ISource` to stream from sockets, ring buffers, or shared memory.
- **Custom Destinations**: Implement `IDestination` for zero-copy output.
- **Custom Formats**: Register custom format stringifiers via `StringifierFactory::instance().registerCreator("MY_FORMAT", creator)`.

See the [Extending YAML_Lib Guide](extending_yaml_lib.md) and [SOLID Architecture Guide](solid_architecture.md).

---

## Alternative Output Formats (JSON, XML, Bencode)

```cpp
YAML_Lib::YAML doc("name: YAML_Lib\nversion: 1.4\n");

std::string json    = doc.dump("JSON");
std::string xml     = doc.dump("XML");
std::string bencode = doc.dump("Bencode");
```

---

## Example Programs

Executable examples located in [`examples/source/`](file:///home/robt/projects/YAML_Lib/examples/source):

| File | Feature Covered |
| :--- | :--- |
| `YAML_Simple_Read_Write.cpp` | Parsing and serializing to files |
| `YAML_Create_At_Runtime.cpp` | Dynamic building and initializer lists |
| `YAML_Files_To_JSON.cpp` | Converting YAML documents to JSON |
| `YAML_Files_To_XML.cpp` | Converting YAML documents to XML |
| `YAML_Files_To_Bencode.cpp` | Converting YAML documents to Bencode |
| `YAML_Custom_IO.cpp` | Implementing custom `ISource` and `IDestination` |
| `YAML_Error_Handling_Demo.cpp` | Syntax error recovery and diagnostics |
| `YAML_Advanced_Types_Demo.cpp` | Timestamps, tags, anchors, and merge keys |
| `YAML_Performance_Profile.cpp` | Large dataset parse and stringify throughput |
