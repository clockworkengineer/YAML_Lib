# Concrete Documentation Plan for `YAML_Lib` (Version 1.4.0 / C++23 Baseline)

## Executive Summary

Following the upgrade to **C++23** (`CMAKE_CXX_STANDARD 23`), the release of **Version 1.4.0**, and the introduction of advanced language features (`std::expected` monadic parsing, `std::format` integration, non-intrusive object serialization, ergonomic accessors, and dynamic container auto-promotion), the project's documentation must be brought into full alignment with the current codebase.

This document presents a concrete, actionable plan to:
1. **Add 3 new dedicated architectural and technical guides** in [`docs/`](file:///home/robt/projects/YAML_Lib/docs) addressing C++23 capabilities, object/container serialization, and embedded/minimal build profiles.
2. **Modify and modernize 9 existing documentation files and configurations** to update API references, replace legacy C++20 terminology with C++23 requirements, document compiler matrices, and ensure complete code-to-documentation parity.

---

## 1. Source Code & API Gap Analysis

A comprehensive audit of the `YAML_Lib` headers and source files reveals several recent capabilities that are currently under-documented or absent from the documentation suite:

| Component / Header | Current Implementation in Source | Documentation Status & Gaps |
| :--- | :--- | :--- |
| **Monadic Parsing**<br>[`classes/include/YAML.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML.hpp) | `YAML::loadExpected(yaml_string)`<br>`YAML::loadExpected(source)`<br>`yaml.parseExpected(source)` returning `std::expected<YAML, std::string>` | **Missing**: Not documented in [`docs/api.md`](file:///home/robt/projects/YAML_Lib/docs/api.md) or [`docs/guide.md`](file:///home/robt/projects/YAML_Lib/docs/guide.md). Only older throwing `parse()` and `tryParse()` are covered. |
| **Modern Ergonomics**<br>[`classes/include/YAML_Core.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Core.hpp)<br>[`YAML_Node_Reference.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/implementation/node/YAML_Node_Reference.hpp) | `node.as<T>()`, `node.is<T>()`<br>`node.value_or(fallback)`<br>`node.value_or(key, fallback)`<br>`node.get_if<T>()`<br>`yaml.as<T>(key)`, `yaml.value_or(key, fallback)` | **Missing**: Guide and API reference still emphasize verbose legacy casts (`isA<T>()` / `NRef<T>()`). Modern direct accessors are not documented. |
| **Auto-Promotion & Iteration**<br>[`classes/include/YAML_Core.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Core.hpp) | Dynamic node promotion on assignment (`node["k"] = v`, `node[0] = v`). Structured bindings: `dict.items()` returning `std::tuple`-compatible `DictionaryEntry`. Range-based loops for `Array`. | **Missing**: Not explained in [`docs/guide.md`](file:///home/robt/projects/YAML_Lib/docs/guide.md) or [`docs/api.md`](file:///home/robt/projects/YAML_Lib/docs/api.md). |
| **`std::format` Integration**<br>[`classes/include/YAML_Format.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Format.hpp) | `std::formatter<YAML_Lib::Node>` and `std::formatter<YAML_Lib::YAML>` specializations for standard formatting via `std::format("{}", node)`. | **Missing**: Completely undocumented header. No mention in [`docs/public_api.md`](file:///home/robt/projects/YAML_Lib/docs/public_api.md) or [`docs/guide.md`](file:///home/robt/projects/YAML_Lib/docs/guide.md). |
| **Non-Intrusive Serialization**<br>[`classes/include/YAML_Serialization.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Serialization.hpp) | `YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(Type, ...)` macro; `to_yaml` / `from_yaml` for primitives, `std::vector`, `std::map`, `std::unordered_map`, and `std::optional`. | **Missing**: Completely undocumented header. No dedicated guide or API specification exists for serialization. |
| **Embedded & No-Exceptions**<br>`YAML_LIB_NO_EXCEPTIONS`<br>`YAML_LIB_FILE_IO=OFF` | Dedicated panic handler API, `tryParse` exclusions, non-exception fallback in `value_or` and `get_if`, `YAML_Lib_NoExceptions_Compile_Tests`. | **Partial**: [`docs/testing.md`](file:///home/robt/projects/YAML_Lib/docs/testing.md) notes compilation flags, but there is no guide explaining how to design application code for zero-exception / embedded targets. |
| **Language Baseline**<br>Project-wide | Upgraded to C++23 (`set(CMAKE_CXX_STANDARD 23)`). Compilers: GCC 13+, Clang 17+, AppleClang 16+, MSVC 17.8+. | **Outdated**: [`docs/guide.md`](file:///home/robt/projects/YAML_Lib/docs/guide.md), [`docs/dependencies.md`](file:///home/robt/projects/YAML_Lib/docs/dependencies.md), [`docs/portability.md`](file:///home/robt/projects/YAML_Lib/docs/portability.md), and [`docs/Doxyfile`](file:///home/robt/projects/YAML_Lib/docs/Doxyfile) still claim C++20 and list obsolete compiler versions (e.g. GCC 10, Clang 15). |

---

## 2. New Documents to Add (`docs/`)

### 2.1. [`docs/cpp23_features.md`](file:///home/robt/projects/YAML_Lib/docs/cpp23_features.md) — *C++23 Modern Capabilities & Migration Guide*
- **Purpose**: Serve as the comprehensive guide for developers adopting modern C++23 features within `YAML_Lib`.
- **Target Audience**: Application developers upgrading from C++20 or integrating `YAML_Lib` into modern C++23 codebases.
- **Contents**:
  1. **Standard Baseline Requirements**: Explanation of C++23 requirements, supported compiler toolchains (GCC 13/14, Clang 17/18, AppleClang 16, MSVC 2022 v17.8+), and toolchain detection notes (e.g. Clang with GCC 13 libstdc++ compatibility).
  2. **Monadic Error Handling with `std::expected`**:
     - `YAML::loadExpected(yaml_string)` and `YAML::loadExpected(source)`.
     - `yaml.parseExpected(source)`.
     - Monadic chaining patterns with `.and_then()`, `.or_else()`, and `.transform()`.
     - Comparison with traditional exception handling (`SyntaxError`) and boolean `tryParse()`.
  3. **Ergonomic Node Interrogation**:
     - Direct typed conversion: `node.as<int>()`, `node.as<std::string>()`, `node.as<std::string_view>()`.
     - Safe type queries: `node.is<std::string>()`, `node.is<Dictionary>()`.
     - Fallback accessors: `node.value_or("default")`, `node.value_or("key", 42)`.
     - Optional extraction: `node.get_if<double>()`.
     - Top-level shortcuts: `yaml.as<T>("key")`, `yaml.value_or("key", fallback)`.
  4. **Dynamic Container Auto-Promotion**:
     - Behavior of default-constructed / `Null` nodes when indexed.
     - Promotion to `Dictionary` on `node["key"] = val`.
     - Promotion to `Array` on `node[0] = val`.
  5. **Structured Bindings & Range Iteration**:
     - Iterating mappings: `for (auto&& [key, value] : dict.items())` and `for (auto&& [key, value] : node.items())`.
     - Iterating sequences: `for (auto&& item : array)`.
     - Tuple protocol details (`std::tuple_size`, `std::tuple_element`, `std::get`).
  6. **`std::format` String Formatting**:
     - Formatting nodes: `std::format("{}", node)`.
     - Formatting documents: `std::format("{}", yaml)`.
  7. **Migration Guide**:
     - Step-by-step instructions to migrate legacy C++20 code (`isA<T>`, `NRef<T>`, manual loops) to concise C++23 idioms.

---

### 2.2. [`docs/serialization.md`](file:///home/robt/projects/YAML_Lib/docs/serialization.md) — *Object & STL Container Serialization Guide*
- **Purpose**: Provide full documentation, recipes, and best practices for converting custom C++ data structures and standard containers to/from YAML using [`YAML_Serialization.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Serialization.hpp).
- **Target Audience**: Developers using `YAML_Lib` for application configuration, persistence, data exchange, and serialization pipelines.
- **Contents**:
  1. **Architecture of `YAML_Serialization.hpp`**:
     - Non-intrusive design: Zero modifications required to user class definitions.
     - Customization points: ADL-based `to_yaml(Node&, const T&)` and `from_yaml(const Node&, T&)`.
  2. **The `YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE` Macro**:
     - Syntax, mechanics, and usage for structs and classes with up to 16 members.
     - Complete working code examples (e.g. `ServerConfig`, `DatabaseConfig`).
  3. **STL Container Support**:
     - Sequences: `std::vector<T>` mapped to YAML sequences.
     - Associative mappings: `std::map<std::string, T>` and `std::unordered_map<std::string, T>` mapped to YAML dictionaries.
     - Optional values: `std::optional<T>` mapped to presence/absence or `Null`.
  4. **Nested & Hierarchical Structures**:
     - Composing nested structs (e.g., `AppConfig` containing `ServerConfig` and a list of `Endpoint` structs).
  5. **Custom `to_yaml` / `from_yaml` Overloads**:
     - Authoring custom serialization for third-party types, non-default constructible types, enums, or specialized formats (e.g. hexadecimal numbers, custom timestamps).
  6. **Error Handling & Validation**:
     - Detecting missing keys, type mismatches, and schema violations during deserialization.

---

### 2.3. [`docs/embedded_and_minimal.md`](file:///home/robt/projects/YAML_Lib/docs/embedded_and_minimal.md) — *Embedded, Minimal, and Hardened Systems Guide*
- **Purpose**: Document deployment of `YAML_Lib` in resource-constrained, embedded, real-time, or safety-critical C++ environments.
- **Target Audience**: Embedded engineers, systems programmers, and developers building lightweight, exception-free binaries.
- **Contents**:
  1. **Build Configuration Flags**:
     - `-DYAML_LIB_NO_EXCEPTIONS=ON`: Compiling under `-fno-exceptions` / `/EHs-c-`.
     - `-DYAML_LIB_FILE_IO=OFF`: Eliminating standard filesystem and `<fstream>` dependencies for pure memory-buffer applications.
     - `-DYAML_LIB_TIMESTAMP_PARSE=OFF`: Disabling regex/timestamp parsing overhead.
     - `-DYAML_LIB_SAX_API=OFF`: Omitting SAX visitor symbols when DOM parsing suffices.
     - Comparison table of binary sizes across configuration profiles.
  2. **Exception-Free Programming Model**:
     - Error handling mechanisms when exceptions are disabled:
       - Inspection with `is<T>()` and extraction with `get_if<T>()` or `value_or()`.
       - Boolean status methods (`tryParse`, `tryStringify`).
       - Custom panic handler callback registration via `Error::setPanicHandler()`.
  3. **Deterministic Memory Management with PMR**:
     - Utilizing `std::pmr::memory_resource` with `Options::memory_resource` and `YAML(std::pmr::memory_resource*)`.
     - Using `std::pmr::monotonic_buffer_resource` for zero-heap fragmentation during parse-and-discard operations.
  4. **SAX Streaming for Low Memory Footprint**:
     - Event-driven processing with `IYAMLEvents` and `YAML::traverseEvents()`.
     - Avoiding full DOM tree allocation for large streams.
  5. **Security Hardening**:
     - Applying `Options::secureOptions()` to protect against entity expansion (Billion Laughs), excessive parse depth, oversized scalars, and document flooding.

---

## 3. Existing Documents to Modify

### 3.1. [`README.md`](file:///home/robt/projects/YAML_Lib/README.md)
- **Modifications**:
  - Update version badges and text references to Version `1.4.0`.
  - In **Features**, add highlights for `YAML_Serialization.hpp` and `YAML_Format.hpp`.
  - In **Documentation**, add links and descriptions for the three new guides:
    - `docs/cpp23_features.md`
    - `docs/serialization.md`
    - `docs/embedded_and_minimal.md`
  - Update quick-start examples to demonstrate modern accessors (`node.as<int>()`, `node.value_or()`, structured bindings with `items()`).

### 3.2. [`docs/api.md`](file:///home/robt/projects/YAML_Lib/docs/api.md)
- **Modifications**:
  - **`YAML` Class**:
    - Add `loadExpected()` and `parseExpected()`.
    - Add `as<T>(key)` and `value_or(key, fallback)`.
    - Add move constructor `YAML(YAML&&)` and move assignment `operator=(YAML&&)`.
    - Add deep cloning method `clone()`.
    - Add named format `dump(format)` and `stringify(format)`.
  - **`Node` & `NRef` Methods**:
    - Document `node.as<T>()`, `node.is<T>()`, `node.value_or(fallback)`, `node.value_or(key, fallback)`, and `node.get_if<T>()`.
    - Document `node.items()` and `dict.items()` for structured bindings.
    - Document auto-promotion on `operator[]`.
    - Document `node.clone()`.
  - **New Header Specifications**:
    - Add reference section for [`YAML_Format.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Format.hpp) (`std::formatter` specializations).
    - Add reference section for [`YAML_Serialization.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Serialization.hpp) (`YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE`, `to_yaml`, `from_yaml`).
    - Add reference section for segregated headers ([`YAML_Reader.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Reader.hpp), [`YAML_Writer.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_Writer.hpp), [`YAML_DOM.hpp`](file:///home/robt/projects/YAML_Lib/classes/include/YAML_DOM.hpp)).

### 3.3. [`docs/guide.md`](file:///home/robt/projects/YAML_Lib/docs/guide.md)
- **Modifications**:
  - Update Introduction and Prerequisites from C++20 to C++23.
  - Update compiler troubleshooting tips with supported toolchains (GCC 13+, Clang 17+, MSVC 2022).
  - Rewrite **Accessing nodes** section to showcase `node.as<T>()`, `node.is<T>()`, `node.value_or()`, and `node.get_if<T>()` as the primary idiomatic pattern, retaining `isA<T>()` and `NRef<T>()` as advanced low-level tools.
  - Add subsection on **Iterating mappings and sequences** demonstrating `for (auto&& [k, v] : dict.items())` and `for (auto&& item : array)`.
  - Add subsection on **Monadic parsing with `std::expected`** (`YAML::loadExpected()`).
  - Add subsection on **Formatting nodes and documents with `std::format`**.
  - Add subsection on **Object Serialization** linking to `docs/serialization.md`.

### 3.4. [`docs/public_api.md`](file:///home/robt/projects/YAML_Lib/docs/public_api.md)
- **Modifications**:
  - Add `YAML_Format.hpp` and `YAML_Serialization.hpp` to the list of official public interface headers.
  - Detail `std::formatter` and serialization API stability contracts.
  - Update language baseline statement from C++20 to C++23.

### 3.5. [`docs/dependencies.md`](file:///home/robt/projects/YAML_Lib/docs/dependencies.md)
- **Modifications**:
  - Change "C++20 standard library" to "C++23 standard library".
  - Update compiler matrix:
    - GCC 13.0 or newer (previously GCC 10+)
    - Clang 17.0 or newer (previously Clang 15+)
    - Apple Clang 16.0 or newer (previously Apple Clang 14+)
    - Microsoft Visual C++ 2022 v17.8+ (previously MSVC 2019+)
  - Update `<expected>` and `<format>` library dependencies.

### 3.6. [`docs/portability.md`](file:///home/robt/projects/YAML_Lib/docs/portability.md)
- **Modifications**:
  - Update standard target to C++23.
  - Document the Clang on Linux with GCC 14 libstdc++ workaround (LLVM Issue #93734 / GCC 13 toolchain selection in CMake).
  - Document portable AppleClang template syntax rules (avoiding invalid `template` keyword prefix on non-dependent template member functions).
  - Document MSVC multi-config generator path handling for test data.

### 3.7. [`docs/solid_architecture.md`](file:///home/robt/projects/YAML_Lib/docs/solid_architecture.md)
- **Modifications**:
  - Update baseline from C++20 to C++23.
  - Update Interface Segregation Principle (ISP) section to mention `YAML_Format.hpp` and `YAML_Serialization.hpp` as orthogonal, opt-in capability headers that keep the core parser interface lean.

### 3.8. [`docs/testing.md`](file:///home/robt/projects/YAML_Lib/docs/testing.md)
- **Modifications**:
  - Add documentation for `tests/source/misc/YAML_Lib_Tests_Cpp23.cpp`.
  - Add documentation for header independence tests (`YAML_Lib_Header_Compile_Tests` and `YAML_Lib_Tests_Header_Interfaces.cpp`).
  - Document `YAML_Lib_NoExceptions_Compile_Tests` target for validating `-fno-exceptions` builds.

### 3.9. [`docs/Doxyfile`](file:///home/robt/projects/YAML_Lib/docs/Doxyfile)
- **Modifications**:
  - Update `PROJECT_BRIEF` from `"Lightweight, header-friendly C++20 YAML library"` to `"Lightweight, header-friendly C++23 YAML library"`.
  - Verify `PROJECT_NUMBER` remains synchronized at `1.4.0`.

---

## 4. Implementation Roadmap & Checklist

### Phase 1: Create New Documentation Files
- [x] Create [`docs/cpp23_features.md`](file:///home/robt/projects/YAML_Lib/docs/cpp23_features.md) with comprehensive coverage of `std::expected`, `std::format`, ergonomic accessors, container auto-promotion, and structured bindings.
- [x] Create [`docs/serialization.md`](file:///home/robt/projects/YAML_Lib/docs/serialization.md) covering `YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE`, standard container mapping, and custom serialization overloads.
- [x] Create [`docs/embedded_and_minimal.md`](file:///home/robt/projects/YAML_Lib/docs/embedded_and_minimal.md) covering `-fno-exceptions`, memory resources (PMR), SAX streaming, and build configuration profiles.

### Phase 2: Modernize Core Guides & API References
- [x] Update [`docs/api.md`](file:///home/robt/projects/YAML_Lib/docs/api.md) with modern `YAML` methods, `Node` accessors, and references for new headers.
- [x] Update [`docs/guide.md`](file:///home/robt/projects/YAML_Lib/docs/guide.md) to use C++23 idioms throughout, update troubleshooting tips, and add sections on `std::expected` and serialization.
- [x] Update [`docs/public_api.md`](file:///home/robt/projects/YAML_Lib/docs/public_api.md) with new headers and C++23 baseline specification.

### Phase 3: Update System, Platform & Testing Documents
- [x] Update [`docs/dependencies.md`](file:///home/robt/projects/YAML_Lib/docs/dependencies.md) with C++23 compiler prerequisites.
- [x] Update [`docs/portability.md`](file:///home/robt/projects/YAML_Lib/docs/portability.md) with modern compiler matrix and platform workarounds.
- [x] Update [`docs/solid_architecture.md`](file:///home/robt/projects/YAML_Lib/docs/solid_architecture.md) and [`docs/testing.md`](file:///home/robt/projects/YAML_Lib/docs/testing.md).
- [x] Update [`docs/Doxyfile`](file:///home/robt/projects/YAML_Lib/docs/Doxyfile) `PROJECT_BRIEF`.

### Phase 4: Synchronize Root Overview & Links
- [x] Update [`README.md`](file:///home/robt/projects/YAML_Lib/README.md) with new doc links, Version 1.4.0 notes, and modern C++23 examples.
- [x] Validate cross-file markdown links across all documentation files.
