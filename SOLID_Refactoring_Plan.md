# SOLID Architectural Refactoring Plan for `YAML_Lib`

## Executive Summary

This document presents a comprehensive, concrete refactoring plan to align **`YAML_Lib`** strictly with the **SOLID design principles**:
- **S**ingle Responsibility Principle (SRP)
- **O**pen/Closed Principle (OCP)
- **L**iskov Substitution Principle (LSP)
- **I**nterface Segregation Principle (ISP)
- **D**ependency Inversion Principle (DIP)

While `YAML_Lib` has clean headers and robust unit test coverage, its current architecture contains monolithic classes (`YAML_Impl`, `Default_Parser`), tight coupling between high-level facade objects and concrete implementations, hardcoded format dispatching, and fat interface definitions.

---

## 1. Single Responsibility Principle (SRP) Analysis & Refactoring

### Current SRP Violations
1. **`YAML` & `YAML_Impl` (God Objects)**:
   - **Current State**: `YAML_Impl` manages document collection lifecycle, parser/stringifier instantiation, file I/O operations (`fromFile`, `toFile`), format detection (`getFileFormat`), node tree traversal (`traverseNodes`), and SAX event dispatch (`traverseEvents`).
   - **Impact**: Changes to file I/O or tree traversal require modifying `YAML_Impl`.

2. **`Default_Parser` (Monolithic Parser)**:
   - **Current State**: Handles lexical tokenization, indentation stack tracking, block/flow scalar decoding, directive parsing (`%YAML`, `%TAG`), anchor/alias table resolution, tag handling, and DOM construction across 11 files (`YAML_Parser_*.cpp`).
   - **Impact**: Parser state and lexer rules are tightly coupled. Adding new tag handling or modifying alias resolution requires altering `Default_Parser` internals.

3. **`YAML_Node` / `YAML_Node_Reference`**:
   - **Current State**: Combines node storage semantics, tree traversal, variant type casting (`isA`, `NRef`), indexing validation, and formatting helper functions.

### Target SRP Architecture
```
                         +------------------------+
                         |      YAML Facade       |
                         +-----------+------------+
                                     | delegates to
            +------------------------+------------------------+
            |                        |                        |
+-----------v------------+ +---------v----------+ +-----------v------------+
|     DocumentStore      | |  YAML_FileReader   | |   YAML_TreeTraverser   |
| (Manages Node Trees)   | | & FileWriter (I/O) | |   & SAXDispatcher      |
+------------------------+ +--------------------+ +------------------------+
```

### Concrete Refactoring Actions
- **Extract `DocumentStore`**: Move document storage, index bounds checking, and collection memory resource management into a dedicated `DocumentStore` class.
- **Extract `YAML_Lexer` / `ITokenizer`**: Separate lexing/tokenization and indentation state management from syntactical parsing logic.
- **Extract `AliasResolver`**: Isolate `&anchor` recording and `*alias` dereferencing into a dedicated stateful anchor table component.
- **Extract `TagRegistry` / `ISchema`**: Separate tag resolution (`!tag`, `!!str`, `!!int`) into schema resolver components.

---

## 2. Open/Closed Principle (OCP) Analysis & Refactoring

### Current OCP Violations
1. **Hardcoded Stringifier & Format Dispatch**:
   - **Current State**: Format conversion (`YAML`, `JSON`, `XML`, `Bencode`) relies on hardcoded `switch`/`if-else` branches in `YAML_Impl` and `YAML_File.cpp`.
   - **Impact**: Adding a new format (e.g., TOML, CBOR) requires editing core library files.

2. **Hardcoded Tag Resolution & Scalar Casting**:
   - **Current State**: `YAML_Parser_Scalar.cpp` and `YAML_Parser_Tag.cpp` hardcode YAML 1.2 core schema scalar type inference.
   - **Impact**: Custom tag schemas or alternative scalar coercion rules cannot be added without modifying parser code.

### Target OCP Architecture (Strategy & Factory Pattern)
```
                  +-------------------------+
                  |    IStringifyFactory    |
                  +------------+------------+
                               |
       +-----------------------+-----------------------+
       |                       |                       |
+------v-------+        +------v-------+        +------v-------+
| YAML_Stringify|       | JSON_Stringify|       | Custom_Format |
|   Strategy   |        |   Strategy   |        |   Strategy   |
+--------------+        +--------------+        +--------------+
```

### Concrete Refactoring Actions
- **Introduce `StringifierRegistry` / `FormatFactory`**:
  ```cpp
  class StringifierFactory {
  public:
      static void registerStringifier(YAML::Format format, std::function<std::unique_ptr<IStringify>()> creator);
      static std::unique_ptr<IStringify> create(YAML::Format format);
  };
  ```
- **Introduce `ISchema` Interface**:
  ```cpp
  class ISchema {
  public:
      virtual ~ISchema() = default;
      virtual Node resolveScalar(std::string_view scalarText, bool isQuoted) const = 0;
      virtual Node resolveTag(std::string_view tagHandle, std::string_view tagSuffix, const Node& node) const = 0;
  };
  ```
  Implement concrete schemas: `CoreSchema`, `JSONSchema`, `FailsafeSchema`.

---

## 3. Liskov Substitution Principle (LSP) Analysis & Refactoring

### Current LSP Violations
1. **`ISource` and `IDestination` Implementation Contracts**:
   - **Current State**: `BufferedSourceBase`, `BufferSource`, `FileSource`, `StreamSource`, and `SpanSource` handle EOF and lookahead slightly differently. For example, some sources throw on out-of-bounds `next()` while others return `\0`.
   - **Impact**: Replacing `BufferSource` with `StreamSource` in certain parser edge-cases risks unexpected exceptions.

2. **Node Variant Type Inspection**:
   - **Current State**: Variant types (`String`, `Number`, `Boolean`, `Array`, `Dictionary`, `Null`, `Comment`, `Hole`) throw generic runtime `Error` when `NRef<T>` mismatch occurs instead of using uniform double-dispatch or non-throwing type inspection.

### Concrete Refactoring Actions
- **Establish Strict Interface Contracts & Contract Tests**:
  - Define exact post-conditions for `ISource`: `current()` at EOF must return `kNull` (`\0`), `more()` must return `false`, and `next()` past EOF must be safe and idempotent.
  - Implement contract test fixtures (`ISourceContractTests`, `IDestinationContractTests`) parameterised across all source/destination types.
- **Standardize Node Variant Visitor (`NodeVisitor`)**:
  - Implement a type-safe `std::visit` / `NodeVisitor` interface for all node variants to avoid unsafe downcasting.

---

## 4. Interface Segregation Principle (ISP) Analysis & Refactoring

### Current ISP Violations
1. **`IParser` Fat Interface**:
   - **Current State**: `IParser` only offers `std::vector<Node> parse(ISource &source)`. Clients wanting SAX events or single-document streaming are forced to instantiate DOM vectors.

2. **`IStringify` Fat Interface**:
   - **Current State**: Forces all implementations to handle `getIndent()` and `setIndent()`, even formats like JSON or Bencode where line indentation state is handled differently or irrelevant.

3. **`YAML` Facade**:
   - **Current State**: Single monolithic header forces all users to pull in parsing, stringification, tree manipulation, file I/O, and traversal interfaces.

### Target ISP Architecture
```
              +-------------------+       +--------------------+
              |    IDOMParser     |       |     ISAXParser     |
              +---------+---------+       +---------+----------+
                        |                           |
                        +-------------+-------------+
                                      |
                             +--------v--------+
                             | Default_Parser  |
                             +-----------------+
```

### Concrete Refactoring Actions
- **Segregate Parsing Interfaces**:
  - `IDOMParser`: `std::vector<Node> parse(ISource &source)`
  - `ISAXParser`: `void parseSAX(ISource &source, IYAMLEvents &events)`
- **Segregate Stringifier Interfaces**:
  - `IFormattedStringify`: For indented human-readable formats (YAML, XML).
  - `ICompactStringify`: For binary/compact formats (Bencode).
- **Segregate Client Facade Headers**:
  - `YAML_Reader.hpp`: Only read/parse types.
  - `YAML_Writer.hpp`: Only output/stringifier types.
  - `YAML_DOM.hpp`: Node, Document, and traversal types.

---

## 5. Dependency Inversion Principle (DIP) Analysis & Refactoring

### Current DIP Violations
1. **Concrete Class Instantiation in High-Level Modules**:
   - **Current State**: `YAML_Impl` directly invokes `new Default_Parser(...)`, `new Default_Stringify(...)`, and `new Default_Translator(...)`.
   - **Impact**: High-level core logic depends directly on concrete low-level implementation details.

2. **Direct Concrete Node Instantiation in Parser**:
   - **Current State**: `Default_Parser` constructs concrete `YAML_Array`, `YAML_Dictionary`, `YAML_String` instances directly rather than delegating to a node factory abstraction.

### Target DIP Architecture
```
+-------------------+        depends on        +--------------------+
|     YAML_Impl     | -----------------------> |      IParser       |
+-------------------+                          +---------^----------+
                                                         |
                                                     implements
                                                         |
                                               +---------+----------+
                                               |   Default_Parser   |
                                               +--------------------+
```

### Concrete Refactoring Actions
- **Inject Dependencies into `YAML_Impl`**:
  - Modify `YAML_Impl` constructor to accept abstract `IParserFactory` and `IStringifyFactory` instead of instantiating concrete defaults directly.
- **Introduce `INodeFactory`**:
  - Allow `IParser` implementations to depend on an `INodeFactory` interface, enabling custom node allocation strategies (e.g. PMR arena allocation or custom node subtypes).

---

## Proposed Refactoring Phases & Timeline

| Phase | Objective | Key Components Refactored | Verification Plan |
| :--- | :--- | :--- | :--- |
| **Phase 1** | **DIP & OCP Strategy Infrastructure** | Introduce `StringifierFactory`, `FormatRegistry`, Dependency Injection in `YAML_Impl` | Unit tests for format registration & DI |
| **Phase 2** | **SRP Parser & Lexer Decoupling** | Extract `YAML_Lexer`, `AliasResolver`, `TagRegistry`, `ISchema` from `Default_Parser` | 100% pass on official `yaml-test-suite` |
| **Phase 3** | **SRP & ISP Facade & I/O Segregation** | Extract `DocumentStore`, `YAML_FileReader/Writer`, segregate `IDOMParser`/`ISAXParser` | Public API header compilation tests |
| **Phase 4** | **LSP Contract Standardization** | Standardize `ISource`/`IDestination` exception guarantees, add contract test suite | Contract test suite execution |

---

## Conclusion & Next Steps

This concrete plan eliminates architectural debt, transforms `YAML_Lib` into a fully SOLID-compliant library, and enables seamless extension for new formats, schemas, and custom parsers without modifying existing code.
