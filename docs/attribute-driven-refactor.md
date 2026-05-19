# Attribute-driven Refactor Notes

This document explains how `YAML_Lib` implements the library attributes from `notes/attributes.md` and where to find the relevant documentation and design decisions.

## Purpose

`YAML_Lib` is designed to be more than a parser; it is a library with clear API design, comprehensive documentation, reliability safeguards, portability, and low dependency overhead.

## Current implementation map

### 1. Intuitive API Design
- Public headers are intentionally limited to a stable export set in `docs/public_api.md`.
- Convenience helpers such as `YAML::load(...)`, `YAML::loadFile(...)`, and `YAML::dump()` are exposed from `YAML.hpp`.
- `Options` lifetime requirements and custom interface ownership rules are documented in `YAML.hpp`, `ISource.hpp`, and `IDestination.hpp`.

### 2. Comprehensive Documentation
- `docs/public_api.md` documents the installed public header set and explicit public API surface.
- `docs/guide.md` includes usage examples for `Options`, custom stringifiers, custom parsers, and panic-handler usage.
- `docs/portability.md` documents supported platforms, build configurations, and safe defaults for untrusted YAML.
- This document itself is the attribute-driven implementation reference.

### 3. High Reliability
- Parser limits, strict booleans, and secure `Options` usage are documented in `docs/guide.md`.
- `docs/portability.md` highlights safe defaults and no-exceptions build guidance.

### 4. Performance and Efficiency
- The library already exposes `std::pmr::memory_resource` customization via `Options`.
- Performance and memory usage considerations are discussed in `docs/guide.md` under the performance section.

### 5. Maintainability
- Public headers and internal implementation headers are separated at the CMake install boundary.
- `docs/public_api.md` helps maintain the export surface and keep internal headers private unless explicitly required.

### 6. Flexibility and Customization
- `docs/guide.md` shows how to implement custom `IStringify`, `ISource`, `IDestination`, and `IParser` components.
- `Options` allows runtime customization of parser, stringifier, and memory resource.

### 7. Strong Security
- Safe default parser limits are documented in `docs/guide.md` and `docs/portability.md`.
- The library encourages secure settings for untrusted YAML and explains the tradeoffs of `YAML_LIB_NO_EXCEPTIONS` usage.

### 8. High Testability
- The existing test suite is described in `docs/testing.md`.
- The public API and extension points are documented so new tests can target the intended interfaces.

### 9. Compatibility and Portability
- Supported platforms and build examples are documented in `docs/portability.md`.
- The library keeps platform-specific converter code isolated under `classes/source/implementation/converter`.

### 10. Low Dependency Footprint
- Dependencies are documented in `docs/dependencies.md`.
- Runtime usage depends only on the C++ standard library, while examples and tests may use optional dependencies like Catch2 and PLOG.

## How to use this documentation

- If you are using `YAML_Lib`, start with `docs/guide.md` for examples and `docs/public_api.md` for installed headers.
- If you are contributing to the library, use this doc to understand how each attribute is reflected in the repository structure and documentation.
- If you are evaluating the library, use the safe defaults in `docs/portability.md` and the public API coverage in `docs/public_api.md`.
