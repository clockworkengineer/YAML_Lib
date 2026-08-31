# SOLID Architecture Guide

`YAML_Lib` is designed around a 100% **SOLID** object-oriented C++20 architecture. This guide details how each of the five SOLID design principles is implemented within the codebase.

---

## Architecture Overview

```
                                  +-----------------------+
                                  |      YAML Facade      |
                                  +-----------+-----------+
                                              |
                   +--------------------------+--------------------------+
                   |                          |                          |
        +----------v-----------+   +----------v-----------+   +----------v-----------+
        |   DocumentStore      |   |   YAML_FileReader    |   |  StringifierFactory  |
        | (SRP Container Mgr)  |   | & YAML_FileWriter(I/O)|   |  (OCP/DIP Strategy)  |
        +----------------------+   +----------------------+   +----------------------+

                   +--------------------------+--------------------------+
                   |                          |                          |
        +----------v-----------+   +----------v-----------+   +----------v-----------+
        |     YAML_Lexer       |   |    AliasResolver     |   |    CoreSchema        |
        | (SRP Lexer Scanning) |   | (SRP Anchor Table)   |   | (OCP ISchema Impl)   |
        +----------------------+   +----------------------+   +----------------------+

                                              |
                                   +----------v-----------+
                                   |    INodeFactory      |
                                   | (DIP Abstract Factory|
                                   +----------------------+
```

---

## Principles & Implementation

### 1. Single Responsibility Principle (SRP)

Each class in `YAML_Lib` has one focused responsibility:

- **[`DocumentStore`](../classes/include/implementation/common/YAML_DocumentStore.hpp)**: Manages document collection lifecycle and memory storage for parsed trees.
- **[`YAML_FileReader`](../classes/include/implementation/io/YAML_FileIO.hpp)** & **[`YAML_FileWriter`](../classes/source/implementation/io/YAML_FileIO.cpp)**: Manages file I/O streams and Byte Order Mark (BOM) detection.
- **[`AliasResolver`](../classes/include/implementation/parser/YAML_AliasResolver.hpp)**: Encapsulates `&anchor` recording, `*alias` resolution, and recursion depth validation.
- **[`YAML_Lexer`](../classes/include/implementation/parser/YAML_Lexer.hpp)**: Handles character scanning, line/column tracking, whitespace/comment skipping, and indentation level stack management.
- **`YAML_Impl`**: Acts as a facade coordinator, delegating specific work to these single-responsibility components.

---

### 2. Open/Closed Principle (OCP)

The library is open for extension without modifying core parsing or stringifying code:

- **[`StringifierFactory`](../classes/include/implementation/common/YAML_StringifierFactory.hpp)**: Strategy registry that allows registering custom output format generators (e.g. TOML, CSV) dynamically.
- **[`ISchema`](../classes/include/interface/ISchema.hpp)** & **[`CoreSchema`](../classes/include/implementation/common/YAML_CoreSchema.hpp)**: Strategy interface for customizable scalar resolution and tag coercion rules (e.g. Core Schema, JSON Schema, Failsafe Schema).

---

### 3. Liskov Substitution Principle (LSP)

All interface implementations strictly adhere to behavioral contracts:

- **[`ISource`](../classes/include/interface/ISource.hpp)** and **[`IDestination`](../classes/include/interface/IDestination.hpp)** implementations (`BufferSource`, `FileSource`, `StreamSource`, etc.) undergo automated contract testing in [`YAML_Lib_Tests_SourceContract.cpp`](../tests/source/contract/YAML_Lib_Tests_SourceContract.cpp).
- Any custom `ISource` can be substituted transparently into `Default_Parser::parse()`.

---

### 4. Interface Segregation Principle (ISP)

Clients depend only on the minimal interface methods they require:

- **[`IDOMParser`](../classes/include/interface/IDOMParser.hpp)**: Segregated interface for DOM vector parsing (`parse(ISource &)`).
- **[`ISAXParser`](../classes/include/interface/ISAXParser.hpp)**: Segregated interface for push-based SAX event parsing (`parseSAX(...)`).
- **Segregated Facade Headers**:
  - `YAML_Reader.hpp`: Only read/parse types.
  - `YAML_Writer.hpp`: Only write/stringify types.
  - `YAML_DOM.hpp`: Only DOM tree manipulation types.

---

### 5. Dependency Inversion Principle (DIP)

High-level modules depend on abstractions, not concrete details:

- **[`INodeFactory`](../classes/include/interface/INodeFactory.hpp)**: Abstract Factory interface for Node creation.
- **[`Default_Parser`](../classes/include/implementation/parser/Default_Parser.hpp)** receives injected `ISchema`, `INodeFactory`, and `ITranslator` instances, decoupling parsing algorithms from concrete node storage and scalar resolution rules.
