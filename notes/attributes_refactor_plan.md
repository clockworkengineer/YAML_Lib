# YAML_Lib Attributes Refactor Plan

This plan applies the 10 library attributes from `notes/attributes.md` to the current `YAML_Lib` codebase. It identifies current strengths, compliance gaps, and concrete refactoring tasks with file-level targets.

## Current status summary

- `CMakeLists.txt` is modern and C++23-based with platform-specific warning flags.
- Public API in `classes/include/YAML.hpp` already uses some attributes: `[[nodiscard]]` and `[[noreturn]]` in error handling.
- The library has a useful feature set and pluggable design, but there are maintainability, reliability, and API hygiene gaps.
- `YAML_LIB_NO_EXCEPTIONS` support exists, but the codebase still relies on exceptions in many hot paths and may not be fully exception-free.
- Documentation is present in `README.md` and `docs/`, but public API documentation and build-option coverage can be improved.

## Attribute-by-attribute assessment

### 1. Intuitive API Design

**Observations**
- `YAML` constructors and overloads are powerful but ambiguous for users.
- `operator[]` may throw on missing keys or invalid indexes, creating surprising behavior.
- Public headers expose too much implementation detail via installed includes.

**Refactor tasks**
- Audit public API surface in `classes/include/YAML.hpp`, `YAML_Core.hpp`, and interface headers.
- Add explicit, self-documenting factory/option API, e.g. `YAML::Options` or `YAML::Config`.
- Avoid silent overload ambiguity by favoring clearly named factory functions or config objects.
- Hide internal implementation headers from install targets.

### 2. Comprehensive Documentation

**Observations**
- README is strong on examples, but the library lacks explicit documentation for build-time features and compile-time options.
- `YAML_LIB_NO_EXCEPTIONS`, `YAML_LIB_FILE_IO`, and `YAML_LIB_SAX_API` are not documented in public user docs.

**Refactor tasks**
- Document build options and supported compile-time profiles in `README.md` and `docs/guide.md`.
- Add a public API section for error modes, file I/O, SAX API, and memory resource support.
- Create a short developer note for public headers and install layout.

### 3. High Reliability

**Observations**
- The codebase has manual exception-safety patterns and parser depth tracking that are fragile.
- `YAML_LIB_NO_EXCEPTIONS` path uses panic abort semantics, which are not explicitly validated by tests.
- Many accessors could be marked `noexcept` but are not.

**Refactor tasks**
- Add RAII guards for parser state and source lifetime, especially in `classes/source/implementation/parser/*.cpp` and `classes/include/implementation/parser/Default_Parser.hpp`.
- Add `noexcept` to accessor functions and error-mode setters where exceptions cannot escape.
- Stabilize `YAML_LIB_NO_EXCEPTIONS` by auditing all `throw`/`YAML_THROW` paths.

### 4. Performance and Efficiency

**Observations**
- The library uses exceptions for common control flow in some APIs.
- `std::pmr` is supported, but the public API could better expose memory resource use.

**Refactor tasks**
- Replace exception-based "not found" behavior with explicit query results in dictionary/array access.
- Add `[[nodiscard]]` to all lookup and parse result functions that return status or values.
- Improve parser number parsing and string handling by using `std::from_chars` where appropriate.
- Ensure `std::pmr` path is exercised and documented.

### 5. Maintainability

**Observations**
- The install list includes implementation headers under `classes/include/implementation`, making the public API surface too broad.
- The source tree is already modular, but interface vs. implementation separation is not clean.

**Refactor tasks**
- Restructure installed include list to only export stable public headers.
- Move implementation-only headers into a separate non-installable include subdirectory.
- Consolidate API header imports and reduce redundant public includes.

### 6. Flexibility and Customization

**Observations**
- The design offers pluggable parsers and stringifiers, but configuration is mostly implicit.
- Global settings like `YAML::setStrictBooleans` are not clearly scoped.

**Refactor tasks**
- Introduce a lightweight runtime options structure: `YAML::Options` or `YAMLConfig`.
- Expose parser/stringifier selection and strict-mode behavior through that options object.
- Preserve compile-time feature flags while also allowing runtime control of behavior where safe.

### 7. Strong Security

**Observations**
- No explicit input-size or recursion-limit protection is documented.
- `FileSource` and stream parsing are powerful, but untrusted data handling is not called out.

**Refactor tasks**
- Add safe parse options such as maximum document count, depth limits, and input size quotas.
- Document secure usage for untrusted YAML inputs.
- Add bounds checks and early reject code paths in parser helpers.

### 8. High Testability

**Observations**
- Existing tests cover many runtime behaviors, but build-option variations are not clearly visible in the test matrix.
- No explicit public test plan for attribute-annotated APIs and no-exceptions builds.

**Refactor tasks**
- Add tests for `[[nodiscard]]` semantics through compile-time checks if feasible.
- Add dedicated test cases for `YAML_LIB_NO_EXCEPTIONS`, `YAML_LIB_FILE_IO`, and `YAML_LIB_SAX_API` build variants.
- Add API contract tests for `document(index)` bounds and non-throwing accessors.

### 9. Compatibility and Portability

**Observations**
- CMake and compiler flags support Linux and MSVC; the codebase targets modern compilers.
- The public API currently exports a mix of interface and implementation headers.

**Refactor tasks**
- Ensure the installed include layout works for both MSVC and GNU toolchains.
- Add a CMake option or documentation for C++20 compatibility if intended.
- Add a platform compatibility note for `YAML_LIB_NO_EXCEPTIONS` and `std::pmr` support.

### 10. Low Dependency Footprint

**Observations**
- The library already depends only on the C++ standard library.
- `Catch2` is used only for tests.

**Refactor tasks**
- Keep the public API free of additional external dependencies.
- Document the library’s dependency footprint explicitly in `README.md`.

## Concrete implementation plan

### Phase 1: Public API / attribute hygiene

1. Audit and tighten public headers:
   - `classes/include/YAML.hpp`
   - `classes/include/YAML_Core.hpp`
   - `classes/include/interface/*.hpp`
   - `classes/include/implementation/common/YAML_Error.hpp`
2. Expand attribute use:
   - Add `[[nodiscard]]` to all value-producing public methods and status-returning functions.
   - Add `noexcept` to destructors, accessors, setters, and functions that cannot fail.
   - Add `[[deprecated]]` only if there are stable legacy APIs that need phase-out.
   - Add `[[maybe_unused]]` to intentionally unused parameters in platform-specific code and diagnostics.
3. Harden error handling:
   - Make `errorPanic()` and panic-related helper functions `[[noreturn]]` and `noexcept` where not already.
   - Confirm `YAML_THROW` macros work correctly under both exception-enabled and exception-free builds.

### Phase 2: Reliability and maintainability

1. Implement parser and state RAII guards for exception safety in parser source files.
2. Review `operator[]`, `document()`, and `contains()` behavior; change any exception-based normal path to explicit status results.
3. Reduce public install surface by removing implementation headers from `install(FILES ...)` and exporting only stable headers.
4. Add a small `YAML_Lib::Options` or `YAMLConfig` API for runtime configuration of strict mode and parse/stringify behavior.

### Phase 3: Documentation and tests

1. Update `README.md` and `docs/guide.md` to include:
   - build-time feature flags
   - exception-free build support
   - file I/O, SAX, and memory resource usage
   - safe parsing guidance
2. Add dedicated test coverage for:
   - exception-free build semantics
   - missing key/index lookup results
   - API contract invariants
   - feature-flag variants (`YAML_LIB_FILE_IO`, `YAML_LIB_SAX_API`)
3. Add a small developer note or doc page enumerating the chosen public API headers and installed include tree.

### Phase 4: Portability and security hardening

1. Add explicit documentation for supported platforms and CMake options.
2. Introduce safe parse limits for untrusted input if security posture requires it.
3. Add a short compatibility checklist around modern C++ features, `std::pmr`, and `-fno-exceptions`.

## Recommended first change set

- `classes/include/YAML.hpp`: add `[[nodiscard]]` and `noexcept` to value-returning accessors.
- `classes/include/implementation/common/YAML_Error.hpp`: enforce `[[noreturn]]` / `noexcept` and document `YAML_THROW` behavior.
- `CMakeLists.txt`: document build option defaults and explicitly export `YAML_LIB_NO_EXCEPTIONS`, `YAML_LIB_FILE_IO`, `YAML_LIB_SAX_API` in installation docs.
- `README.md` or `docs/guide.md`: add a dedicated section for compile-time and runtime usage patterns.
- `tests/CMakeLists.txt` and test sources: add a matrix of feature-flag variants and non-throwing API tests.

## File targets for implementation

- `notes/attributes_refactor_plan.md` (this document)
- `classes/include/YAML.hpp`
- `classes/include/YAML_Core.hpp`
- `classes/include/implementation/common/YAML_Error.hpp`
- `CMakeLists.txt`
- `README.md`
- `docs/guide.md`
- `tests/CMakeLists.txt`
- `tests/source/*`

## Outcome expected

After this refactor plan is executed, `YAML_Lib` will be more aligned with the 10 attributes:
- cleaner, safer public API
- stronger documentation of build and usage options
- better reliability across exception-enabled and exception-free builds
- improved test coverage for real-world and edge-case library behaviors
- a smaller, more maintainable public header footprint
