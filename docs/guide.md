# YAML_Lib User Guide

This guide will help you get started with YAML_Lib, covering installation, basic usage, and advanced features.

## Table of Contents
- [Introduction](#introduction)
- [Installation](#installation)
- [Basic Usage](#basic-usage)
- [Working with YAML Files](#working-with-yaml-files)
- [Examples](#examples)
- [Advanced Features](#advanced-features)
- [Troubleshooting](#troubleshooting)

---

## Introduction
YAML_Lib is a modern C++ library designed for reading, writing, and manipulating YAML files with ease and efficiency. It provides a simple and intuitive API for configuration management, data serialization, and other use cases where YAML is preferred for its readability and structure. YAML_Lib aims to be lightweight, fast, and extensible, making it suitable for both small projects and large-scale applications. The library supports the full YAML 1.2 specification, including complex data types, anchors, and references.

## Installation
To install YAML_Lib, follow these steps:

1. **Clone the repository:**
   ```sh
   git clone <repo-url>
   ```
2. **Build using CMake:**
   ```sh
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```
   - You can specify build type (Debug/Release) with `-DCMAKE_BUILD_TYPE=Release`.
   - On Windows, use the appropriate generator for your Visual Studio version.
3. **Link the library:**
   - Add the `include/` directory to your project's include path.
   - Link against the built YAML_Lib library (static or shared, as built).
   - Example for CMake:
     ```cmake
     target_link_libraries(your_target PRIVATE YAML_Lib)
     target_include_directories(your_target PRIVATE <path-to-YAML_Lib>/include)
     ```
4. **Dependencies:**
   - YAML_Lib has no external dependencies beyond the C++ standard library.
   - For testing, Catch2 is included as a submodule.

## Basic Usage
To get started, include the main header and use the API to load and manipulate YAML files:
```cpp
#include <YAML_Lib/YAML_Lib.hpp>

int main() {
   YAMLDocument doc;
   // Load a YAML file
   doc.LoadFromFile("config.yaml");

   // Access a value by key
   auto value = doc.GetNode("key").AsString();
   std::cout << "Value: " << value << std::endl;

   // Modify a value
   doc.GetNode("key").SetString("new_value");

   // Save changes
   doc.SaveToFile("config.yaml");
   return 0;
}
```

**Key Points:**
- The main class is `YAMLDocument`, representing the root of a YAML file.
- Use `LoadFromFile` and `SaveToFile` for file operations.
- Nodes can be accessed and modified using `GetNode` and type conversion methods like `AsString()`, `AsInt()`, etc.

## Working with YAML Files
YAML_Lib provides a flexible API for interacting with YAML documents:

- **Loading a YAML file:**
   ```cpp
   doc.LoadFromFile("file.yaml");
   ```
- **Saving a YAML file:**
   ```cpp
   doc.SaveToFile("file.yaml");
   ```
- **Accessing nodes:**
   - Use dot notation for nested keys: `doc.GetNode("parent.child.key")`.
   - You can also access sequences (arrays):
      ```cpp
      auto arr = doc.GetNode("list");
      for (size_t i = 0; i < arr.Size(); ++i) {
            std::cout << arr[i].AsInt() << std::endl;
      }
      ```
- **Creating new nodes:**
   ```cpp
   doc.GetNode("new.key").SetString("value");
   doc.GetNode("numbers").PushBack(42);
   ```
- **Removing nodes:**
   ```cpp
   doc.RemoveNode("obsolete.key");
   ```

**Tip:** Use `HasNode("key")` to check if a node exists before accessing it.

## Examples
Explore the [examples/](../examples/) directory for practical sample programs:

- **Reading and writing YAML files:**
   - `YAML_Simple_Read_Write.cpp`: Demonstrates loading a YAML file, modifying values, and saving changes.
- **Creating YAML at runtime:**
   - `YAML_Create_At_Runtime.cpp`: Shows how to build a YAML document from scratch in code, add nodes, and write to disk.
- **Converting YAML to other formats:**
   - `YAML_Files_To_JSON.cpp`, `YAML_Files_To_XML.cpp`, `YAML_Files_To_Bencode.cpp`: Convert YAML files to JSON, XML, or Bencode formats.
- **Analyzing YAML structure:**
   - `YAML_Analyze_File.cpp`: Parse a YAML file and print its structure or statistics.
- **Working with nested structures:**
   - `YAML_Nested_Structure_Demo.cpp`: Example of handling deeply nested YAML data.

## Advanced Features
YAML_Lib offers advanced capabilities for power users:

- **Custom node types:**
   - Extend the library to support application-specific data types by subclassing node classes.
   - Register custom serializers/deserializers for complex objects.
- **Validation and error handling:**
   - Use `IsValid()` to check document validity.
   - Catch exceptions such as `YAMLParseException` for detailed error messages.
   - Validate node types and required fields before processing.
- **Extending the parser:**
   - Plug in custom tag handlers to support non-standard YAML tags.
   - Override parsing behavior for special use cases.
- **Anchors and references:**
   - Full support for YAML anchors (&) and aliases (*), allowing reuse of nodes.
- **Comments and formatting:**
   - Preserve or add comments programmatically when saving files.

## Troubleshooting
If you encounter issues, consider the following tips:

- **YAML file validity:**
   - Use online YAML validators or linters to check your files.
   - Ensure correct indentation and syntax.
- **Error messages:**
   - Catch and inspect exceptions for detailed parsing errors.
   - Use `doc.GetLastError()` to retrieve the last error message.
- **Debugging node access:**
   - Use `HasNode("key")` to check for existence before accessing.
   - Print node types and values for debugging:
      ```cpp
      auto node = doc.GetNode("key");
      std::cout << node.Type() << ": " << node.AsString() << std::endl;
      ```
- **API Reference:**
   - See the [API Reference](api.md) for a full list of methods, classes, and usage notes.

---

For more details, see the [API Reference](api.md) or explore the `examples/` directory. If you have questions or need help, open an issue on the repository or consult the community forums.
