# Object & Container Serialization Guide

`YAML_Lib` 1.4.0 provides header [`YAML_Serialization.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Serialization.hpp) for non-intrusive serialization and deserialization of C++ objects, standard containers, and custom types to and from YAML nodes.

---

## Table of Contents

- [Overview](#overview)
- [The `YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE` Macro](#the-yaml_lib_define_type_non_intrusive-macro)
- [STL Container Serialization](#stl-container-serialization)
- [Nested & Hierarchical Structures](#nested--hierarchical-structures)
- [Custom `to_yaml` and `from_yaml` Overloads](#custom-to_yaml-and-from_yaml-overloads)
- [Complete End-to-End Example](#complete-end-to-end-example)

---

## Overview

The serialization framework uses Argument-Dependent Lookup (ADL) functions:
- `void to_yaml(YAML_Lib::Node& node, const T& value)` — converts a C++ value to a YAML node.
- `void from_yaml(const YAML_Lib::Node& node, T& value)` — extracts a C++ value from a YAML node.

Because these functions are defined non-intrusively outside the class, you can serialize third-party structures or domain types without modifying their class definitions or introducing inheritance dependencies.

Header:
```cpp
#include "YAML.hpp"
#include "YAML_Serialization.hpp"
```

---

## The `YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE` Macro

For structs and classes with public fields, `YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE` automatically generates both `to_yaml` and `from_yaml` implementations. It supports up to 16 member fields.

### Syntax

```cpp
YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(TypeName, member1, member2, ...)
```

> **Note:** The macro must be placed in the global namespace or in the same namespace where the type is defined.

### Example

```cpp
#include "YAML.hpp"
#include "YAML_Serialization.hpp"
#include <iostream>

struct DatabaseConfig {
    std::string host{"localhost"};
    int port{5432};
    std::string username{"admin"};
    bool ssl{true};
};

YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(DatabaseConfig, host, port, username, ssl)

int main() {
    DatabaseConfig config{"db.internal.net", 5433, "app_user", true};

    // Serialize to Node
    YAML_Lib::Node node;
    YAML_Lib::to_yaml(node, config);

    // Convert to YAML text
    std::cout << YAML_Lib::YAML(node).dump() << "\n";

    // Deserialize from Node
    DatabaseConfig restored;
    YAML_Lib::from_yaml(node, restored);

    std::cout << "Restored port: " << restored.port << "\n";
}
```

Generated YAML:
```yaml
host: db.internal.net
port: 5433
ssl: true
username: app_user
```

---

## STL Container Serialization

`YAML_Serialization.hpp` provides built-in serializers for standard C++ containers:

### 1. `std::vector<T>` (Sequences)

Vectors map directly to YAML sequences. Elements are recursively serialized/deserialized:

```cpp
std::vector<int> numbers = {10, 20, 30, 40};

YAML_Lib::Node node;
YAML_Lib::to_yaml(node, numbers); // Creates an Array node

std::vector<int> output;
YAML_Lib::from_yaml(node, output);
```

### 2. `std::map<std::string, T>` and `std::unordered_map<std::string, T>` (Mappings)

Maps convert to and from YAML dictionaries where keys are strings:

```cpp
std::map<std::string, double> metrics = {
    {"cpu_load", 0.75},
    {"memory_usage", 0.42}
};

YAML_Lib::Node node;
YAML_Lib::to_yaml(node, metrics);

std::map<std::string, double> restored;
YAML_Lib::from_yaml(node, restored);
```

### 3. `std::optional<T>` (Nullable / Optional Fields)

Optional values are serialized if engaged; when `std::nullopt`, they produce a `Null` YAML node:

```cpp
std::optional<std::string> token = "xyz-123";

YAML_Lib::Node node;
YAML_Lib::to_yaml(node, token);

std::optional<std::string> loaded;
YAML_Lib::from_yaml(node, loaded);
```

---

## Nested & Hierarchical Structures

Because serialization functions compose via ADL, nested structs serialize seamlessly:

```cpp
struct HttpEndpoint {
    std::string path;
    std::string method;
};
YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(HttpEndpoint, path, method)

struct WebServerConfig {
    std::string name;
    int port;
    std::vector<HttpEndpoint> endpoints;
    std::map<std::string, std::string> environment;
};
YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(WebServerConfig, name, port, endpoints, environment)
```

Usage:
```cpp
WebServerConfig server{
    "PaymentGateway",
    8443,
    {{"/pay", "POST"}, {"/status", "GET"}},
    {{"STAGE", "production"}, {"REGION", "eu-central-1"}}
};

YAML_Lib::Node node;
YAML_Lib::to_yaml(node, server);

std::cout << YAML_Lib::YAML(node).dump() << "\n";
```

Output:
```yaml
endpoints:
  - method: POST
    path: /pay
  - method: GET
    path: /status
environment:
  REGION: eu-central-1
  STAGE: production
name: PaymentGateway
port: 8443
```

---

## Custom `to_yaml` and `from_yaml` Overloads

For types requiring special formatting, non-public fields, or validation, provide custom `to_yaml` and `from_yaml` overloads:

```cpp
enum class LogLevel { Debug, Info, Warn, Error };

inline void to_yaml(YAML_Lib::Node& node, const LogLevel& level) {
    switch (level) {
        case LogLevel::Debug: node = "DEBUG"; break;
        case LogLevel::Info:  node = "INFO"; break;
        case LogLevel::Warn:  node = "WARN"; break;
        case LogLevel::Error: node = "ERROR"; break;
    }
}

inline void from_yaml(const YAML_Lib::Node& node, LogLevel& level) {
    const auto str = node.as<std::string>();
    if (str == "DEBUG") level = LogLevel::Debug;
    else if (str == "INFO") level = LogLevel::Info;
    else if (str == "WARN") level = LogLevel::Warn;
    else if (str == "ERROR") level = LogLevel::Error;
    else throw YAML_Lib::Exception("Invalid log level: " + str);
}
```

---

## Complete End-to-End Example

Combining parsing, serialization, modification, and stringification:

```cpp
#include "YAML.hpp"
#include "YAML_Serialization.hpp"
#include <iostream>

struct ServiceConfig {
    std::string name;
    int port;
    bool debug{false};
};
YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(ServiceConfig, name, port, debug)

int main() {
    std::string_view yamlInput = 
        "name: AuthMicroservice\n"
        "port: 9001\n"
        "debug: true\n";

    // 1. Parse YAML to DOM
    auto doc = YAML_Lib::YAML::fromString(yamlInput);

    // 2. Deserialize to strongly typed struct
    ServiceConfig config;
    YAML_Lib::from_yaml(doc->document(0), config);

    // 3. Mutate configuration
    config.port = 9002;
    config.debug = false;

    // 4. Re-serialize back to YAML
    YAML_Lib::Node updatedNode;
    YAML_Lib::to_yaml(updatedNode, config);

    std::cout << YAML_Lib::YAML(updatedNode).dump() << "\n";
}
```
