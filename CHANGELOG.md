# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-09-23

### Added
- **Full Move Semantics & Deep Cloning**: Implemented `YAML(YAML&&) noexcept`, `YAML::operator=(YAML&&) noexcept`, `YAML::clone() const`, and `Node::clone() const` for robust value semantics and safe document duplication.
- **Unified Library Exception Hierarchy**: Introduced `YAML_Lib::Exception : public std::runtime_error` as common root for all library exceptions (`Error`, `SyntaxError`, `YAML_MAKE_ERROR`), retaining full backward-compatibility with `std::runtime_error` catchers.
- **Thread-Safe Stringifier Strategy Registry**: Protected `StringifierFactory` registrations and lookups with `std::shared_mutex` (concurrent readers, exclusive writers) and added case-insensitive format lookups.
- **Thread-Safe PMR Memory Resource Mutations**: Synchronized process-wide `std::pmr::set_default_resource` mutations in `YAML_Impl` with `std::mutex`.
- **Complete Header Hygiene & Self-Containment (IWYU)**: Ensured `ISource.hpp`, `IDestination.hpp`, `IAction.hpp`, `ITranslator.hpp`, and `IStringify.hpp` compile cleanly in total isolation without implicit prerequisites.
- **Export Header Generation**: Added CMake `GenerateExportHeader` producing `YAML_Lib_Export.hpp` for shared/static library symbol visibility control.
- **Package Manager Manifests**: Added `conanfile.py` (Conan 2.0 / 1.x) and `vcpkg.json` manifest support for frictionless ecosystem adoption.
- **Expanded CI Matrix & Automated Releases**: Added GitHub Actions workflows for macOS (Apple Clang), Windows (MSVC), code style verification (`clang-format`), embedded configurations (`YAML_LIB_NO_EXCEPTIONS`, `YAML_LIB_FILE_IO=OFF`), and automated GitHub Releases on tag push (`.github/workflows/release.yml`).
- **Production CMake Packaging**: Full GNUInstallDirs compliance, dual-path include installations (`include/` and `include/YAML_Lib/`), `find_package(YAML_Lib REQUIRED)` CMake target support (`YAML_Lib::YAML_Lib`), and `yaml_lib.pc` for `pkg-config`.
- **Thread-Safe & Re-entrant Parsing**: Replaced mutable static boolean state with per-parser `Options::strict_booleans`, with thread-safe atomic fallback.
- **Pure C++20 Unicode Transcoding**: Modern, portable, thread-safe UTF-8 / UTF-16 codec in `YAML_Converter`, removing deprecated `<codecvt>` / `std::wstring_convert` and Windows-specific `WideCharToMultiByte`.
- **Standalone Header Verification**: Automated test ensuring all public facade and segregated headers compile independently without implicit dependencies.

### Fixed
- **Unsafe `std::tmpnam` Deprecation**: Replaced deprecated `std::tmpnam` in test helpers with high-resolution atomic unique filename generation.
- **Memory Ownership Defect**: Fixed heap-corruption and double-free in `YAML_Impl` when caller passed custom non-owning `IParser` or `IStringify` pointers via `Options`.
- **Missing `<fstream>` Includes**: Added required standard includes in `YAML_FileSource.hpp` and `YAML_FileDestination.hpp` ensuring standalone compilation without precompiled headers.
- **Clang Strict Warnings**: Resolved `-Wmismatched-tags` on forward-declared `Number`, `-Wredundant-move` in `YAML_CoreSchema`, and unused variables across parser sources under `-Wall -Wextra -Werror -pedantic`.
- **PCH Contamination**: Decoupled header compilation test suite from precompiled header reuse to guarantee clean downstream compilation.
- **Example Installation Path**: Fixed hardcoded `bin` destination in examples to respect `${CMAKE_INSTALL_BINDIR}`.

### Changed
- Flattened and organized installed headers into `include/`, `include/interface/`, and `include/implementation/`.
- Updated developer scripts (`Linux-Build.sh`, `Linux-Build-Sanitizers.sh`, `Linux-Run-Tests.sh`, `Linux-Build-Minimal.sh`, `Linux-Style-Check.sh`) with parallel jobs and robust error handling.

---

## [1.1.0] - 2026-08-15

### Added
- **SOLID Architectural Segregation**: Introduced `IDOMParser`, `ISAXParser`, `INodeFactory`, `ISchema`, and `DefaultNodeFactory`.
- **Segregated Facade Headers**: Added modular headers `YAML_Reader.hpp`, `YAML_Writer.hpp`, and `YAML_DOM.hpp` for granular compile-time inclusion.
- **Schema Extensibility**: Pluggable tag and scalar resolution via `ISchema` (`CoreSchema`).
- **Comprehensive Source/Destination Contract Tests**: Added Catch2 test suites verifying `ISource` and `IDestination` contract guarantees.

### Changed
- Refactored `YAML_Impl` into dedicated single-responsibility components: `YAML_DocumentStore`, `YAML_FileIO`, `YAML_Lexer`, `YAML_AliasResolver`.

---

## [1.0.0] - 2026-06-01

### Added
- Full YAML 1.2 core parser and serializer passing 758 YAML Test Suite cases.
- Multi-format output generation: YAML, JSON, XML, and Bencode.
- Flow and block sequence/mapping support, comments, anchors, and aliases.
- Exception and non-exception (`-fno-exceptions`) error handling support.
