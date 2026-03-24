# YAML_Lib API Reference

This document provides an overview of the public API for the YAML_Lib library, including its main classes, functions, and usage examples.

## Table of Contents
- [Overview](#overview)
- [Main Classes and Functions](#main-classes-and-functions)
- [Usage Examples](#usage-examples)
- [Error Handling](#error-handling)
- [Extending the Library](#extending-the-library)

---

## Overview
YAML_Lib is a C++ library for parsing, manipulating, and generating YAML files. It is designed to be lightweight, efficient, and easy to integrate into C++ projects.

## Main Classes and Functions

> **Note:** This is a template. Please update with actual class/function names and signatures as needed.

### YAMLDocument
- **Description:** Represents a YAML document in memory.
- **Key Methods:**
  - `LoadFromFile(const std::string& filename)`
  - `SaveToFile(const std::string& filename)`
  - `GetNode(const std::string& path)`

### YAMLNode
- **Description:** Represents a node (mapping, sequence, or scalar) in a YAML document.
- **Key Methods:**
  - `AsString()`
  - `AsInt()`
  - `AsBool()`
  - `operator[] (const std::string& key)`

### Parsing Functions
- `ParseYAML(const std::string& yamlText)`
- `ValidateYAML(const std::string& yamlText)`

## Usage Examples
See the [Guide](guide.md) and the `examples/` directory for practical usage.

## Error Handling
YAML_Lib throws exceptions for parsing errors and invalid operations. Catch exceptions of type `YAMLException` for error handling.

## Extending the Library
You can extend YAML_Lib by adding new node types or serialization formats. See the developer documentation for more details.

---

For more information, see the [Guide](guide.md) or the `examples/` directory.
