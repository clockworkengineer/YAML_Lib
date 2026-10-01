# Modern C++23 Features & Migration Guide

`YAML_Lib` version 1.4.0 establishes **C++23** (`CMAKE_CXX_STANDARD 23`) as the standard library baseline. This upgrade unlocks powerful language and standard library features, including monadic error handling via `std::expected`, native string formatting via `std::format`, ergonomic node accessors, dynamic container promotion, structured bindings, and simplified container iteration.

This guide details these modern features with practical code examples and provides migration guidance for updating code from C++20.

---

## Table of Contents

- [Standard Baseline & Toolchain Support](#standard-baseline--toolchain-support)
- [Monadic Error Handling with `std::expected`](#monadic-error-handling-with-stdexpected)
- [Ergonomic Node Interrogation (`as`, `is`, `value_or`, `get_if`)](#ergonomic-node-interrogation-as-is-value_or-get_if)
- [Dynamic Container Auto-Promotion](#dynamic-container-auto-promotion)
- [Structured Bindings & Range Iteration](#structured-bindings--range-iteration)
- [`std::format` String Formatting](#stdformat-string-formatting)
- [Migration Guide: Upgrading from C++20](#migration-guide-upgrading-from-c20)

---

## Standard Baseline & Toolchain Support

`YAML_Lib` requires a standard-compliant C++23 compiler and standard library:

| Platform / Toolchain | Minimum Version | Notes |
| :--- | :--- | :--- |
| **GCC** (Linux) | 13.0+ (13.3, 14.1 recommended) | Full C++23 library support (`<expected>`, `<format>`). |
| **Clang** (Linux) | 17.0+ (18.1 recommended) | On Linux, automatically utilizes GCC 13 toolchain when paired with libstdc++. |
| **Apple Clang** (macOS) | 16.0+ (Xcode 16+) | Portable C++23 libc++ support. |
| **MSVC** (Windows) | 2022 v17.8+ (v143 toolset) | Supported with `/std:c++latest` or `/std:c++23`. |

---

## Monadic Error Handling with `std::expected`

When compiling with C++23 and standard library support (`__cpp_lib_expected >= 202211L`), `YAML_Lib` provides non-throwing, functional parse entry points via `std::expected<YAML, std::string>` and `std::expected<void, std::string>`.

Header: `#include "YAML.hpp"`

### 1. Static Parsing with `YAML::loadExpected`

`YAML::loadExpected()` parses a string or an input source and returns either the parsed `YAML` object or an error string without throwing exceptions:

```cpp
#include "YAML.hpp"
#include <iostream>

void parseConfig(std::string_view yaml_text) {
    auto result = YAML_Lib::YAML::loadExpected(yaml_text);
    if (!result) {
        std::cerr << "Parse error: " << result.error() << "\n";
        return;
    }

    YAML_Lib::YAML& yaml = *result;
    std::cout << "Loaded document with " << yaml.getNumberOfDocuments() << " document(s)\n";
}
```

### 2. Stream In-Place Parsing with `yaml.parseExpected`

To parse into an existing `YAML` instance from an `ISource`:

```cpp
YAML_Lib::YAML yaml;
YAML_Lib::BufferSource source("database:\n  port: 5432\n");

auto status = yaml.parseExpected(source);
if (status) {
    std::cout << "Port: " << yaml["database"]["port"].as<int>() << "\n";
} else {
    std::cerr << "Stream failed: " << status.error() << "\n";
}
```

### 3. Monadic Pipeline Workflows

`std::expected` seamlessly composes with C++23 monadic operations:

```cpp
auto dbHost = YAML_Lib::YAML::loadExpected(rawYaml)
    .and_then([](const YAML_Lib::YAML& doc) -> std::expected<std::string, std::string> {
        if (doc.document(0).is<YAML_Lib::Dictionary>()) {
            return doc.value_or("host", std::string("localhost"));
        }
        return std::unexpected("Root node is not a dictionary");
    });

if (dbHost) {
    std::cout << "Connecting to: " << *dbHost << "\n";
}
```

---

## Ergonomic Node Interrogation (`as`, `is`, `value_or`, `get_if`)

Headers: `#include "YAML_Core.hpp"` or `#include "YAML.hpp"`

In addition to traditional `isA<T>()` and `NRef<T>()` helpers, `YAML_Lib` provides expressive member accessors directly on `Node` and `YAML`.

### 1. Typed Conversion: `.as<T>()`

Converts the scalar node to the requested type. Throws `YAML_Lib::Exception` (or panics in `-fno-exceptions` mode) if the node cannot be represented as `T`:

```cpp
YAML_Lib::YAML doc("port: 8080\nhost: localhost\nssl: true\ntimeout: 2.5\n");

// Member access on Node
int port = doc["port"].as<int>();
std::string host = doc["host"].as<std::string>();
std::string_view host_view = doc["host"].as<std::string_view>();
bool ssl = doc["ssl"].as<bool>();
double timeout = doc["timeout"].as<double>();

// Top-level shortcut on YAML (queries first document)
int p = doc.as<int>("port");
std::string h = doc.as<std::string>("host");
```

### 2. Type Queries: `.is<T>()`

Returns `true` if the node matches the requested type without throwing:

```cpp
if (doc["port"].is<int>()) {
    std::cout << "Valid integer port.\n";
}

if (doc.document(0).is<YAML_Lib::Dictionary>()) {
    std::cout << "Root is a mapping.\n";
}
```

### 3. Fallbacks: `.value_or()`

Provides safe fallback values for missing keys or mismatched types:

```cpp
// Direct node fallback (if node is Null or absent)
std::string cluster = doc["cluster"].value_or("us-east-1");

// Dictionary lookup fallback by key
int max_retries = doc.document(0).value_or("retries", 3);

// Top-level document shortcut
int workers = doc.value_or("workers", 4);
```

### 4. Optional Extraction: `.get_if<T>()`

Returns `std::optional<T>` containing the value if present and convertible, or `std::nullopt` otherwise:

```cpp
if (auto optPort = doc["port"].get_if<int>()) {
    std::cout << "Port is set to: " << *optPort << "\n";
} else {
    std::cout << "Port not specified or not an integer.\n";
}
```

---

## Dynamic Container Auto-Promotion

In `YAML_Lib` 1.4.0, default-constructed (or `Null`) nodes automatically promote to containers upon subscript assignment. This allows declarative, intuitive document construction without manual factory calls:

```cpp
YAML_Lib::Node root; // Default-constructed (Null)

// Automatically promotes root to a Dictionary:
root["server"]["host"] = "127.0.0.1";
root["server"]["port"] = 9000;

// Automatically promotes "endpoints" to an Array:
root["server"]["endpoints"][0] = "/api/v1/health";
root["server"]["endpoints"][1] = "/api/v1/query";

std::cout << YAML_Lib::YAML(root).dump() << "\n";
```

Output:
```yaml
server:
  endpoints:
    - /api/v1/health
    - /api/v1/query
  host: 127.0.0.1
  port: 9000
```

---

## Structured Bindings & Range Iteration

`YAML_Lib` provides native standard iteration protocols for both dictionaries and arrays.

### 1. Dictionary Structured Bindings

The `items()` method on `Dictionary` and `Node` returns iterable entries implementing the `std::tuple` protocol (`std::tuple_size`, `std::tuple_element`, `std::get`), enabling direct C++ structured bindings:

```cpp
YAML_Lib::YAML yaml("cpu: 4\nmemory: 16GB\nenv: production\n");

// Structured bindings on Node or Dictionary
for (auto&& [key, value] : yaml.document(0).items()) {
    std::cout << "Key: " << key << " -> Value: " << value.as<std::string>() << "\n";
}
```

### 2. Sequence Range-Based `for` Loops

`Array` supports standard `begin()` and `end()` iterators:

```cpp
YAML_Lib::YAML yaml("numbers: [10, 20, 30, 40]\n");

auto& arr = YAML_Lib::NRef<YAML_Lib::Array>(yaml["numbers"]);
for (auto&& item : arr) {
    std::cout << item.as<int>() << " ";
}
std::cout << "\n";
```

---

## `std::format` String Formatting

Header: `#include "YAML_Format.hpp"`

`YAML_Lib` provides standard `std::formatter` specializations for both `YAML_Lib::Node` and `YAML_Lib::YAML`.

```cpp
#include "YAML.hpp"
#include "YAML_Format.hpp"
#include <format>
#include <iostream>

int main() {
    YAML_Lib::YAML config("app: Gateway\nversion: 1.4.0\n");

    // Format a single Node
    std::string appName = std::format("Application: {}", config["app"]);
    std::cout << appName << "\n"; // Application: Gateway

    // Format an entire YAML document
    std::string fullDoc = std::format("Full configuration:\n{}", config);
    std::cout << fullDoc << "\n";
}
```

---

## Migration Guide: Upgrading from C++20

| Legacy C++20 Idiom | Modern C++23 Idiom in YAML_Lib 1.4.0 | Benefit |
| :--- | :--- | :--- |
| `if (isA<String>(node)) { std::string s = NRef<String>(node).value(); }` | `if (node.is<std::string>()) { std::string s = node.as<std::string>(); }` | Cleaner, eliminates raw variant casts and boilerplate. |
| `try { yaml.parse(src); } catch (const SyntaxError& e) { ... }` | `auto res = YAML::loadExpected(src); if (!res) { ... }` | Non-throwing, composable, zero-cost error paths. |
| `Node dict = Node::make<Dictionary>(); NRef<Dictionary>(dict)["key"] = ...;` | `Node root; root["key"] = ...;` | Automatic container promotion; no factory boilerplate. |
| `for (const auto& [k, v] : NRef<Dictionary>(doc).get()) { ... }` | `for (auto&& [k, v] : doc.items()) { ... }` | Direct structured bindings on `Node` and `Dictionary`. |
| `std::string s = node.toString();` | `std::format("{}", node);` | Standard C++ format integration with `<format>`. |
