# YAML_Lib Refactor Plan

## Goal
Implement the 10 library attributes described in `notes/attributes.md` across YAML_Lib. The plan targets the library's public API, documentation, build system, parser reliability, and test coverage so the library becomes more intuitive, maintainable, portable, secure, and lightweight.

## Scope
- Public API and interface cleanup
- Documentation and developer guidance
- Reliability, security, and defensive parsing
- Performance and portability
- Test coverage and quality gates
- Dependency footprint and build configuration

## Attributes to Implement
1. Intuitive API Design
2. Comprehensive Documentation
3. High Reliability
4. Performance and Efficiency
5. Maintainability
6. Flexibility and Customization
7. Strong Security
8. High Testability
9. Compatibility and Portability
10. Low Dependency Footprint

---

## Phase 1: Audit and API Stabilization

### 1.1 Public API audit
- Inspect `classes/include/YAML.hpp`, `classes/include/YAML_Core.hpp`, and `classes/include/interface/YAML_Interfaces.hpp`.
- Identify public types and methods that should remain stable.
- Document which headers are public and which are internal implementation only.
- Ensure `docs/public_api.md` matches actual installed header set.

### 1.2 API clarity improvements
- Consolidate top-level public API into `classes/include/YAML.hpp` with a small stable surface.
- Keep implementation details inside `classes/include/interface/*` and `classes/include/implementation/*` only when needed by extensions.
- Rename or alias confusing entry points if necessary to make usage easier.
- Add small helper overloads for common operations, e.g. parse-from-string and stringify-to-string.

### 1.3 Public interface review
- Review `IParser`, `ISource`, `IDestination`, `IStringify`, `ITranslator`, `IAction`, and `Options`.
- Ensure method names and contracts are explicit and documented.
- Add `noexcept` where safe and appropriate for public methods.
- Add `[[nodiscard]]` to result-returning APIs when ignoring results would be a bug.

---

## Phase 2: Documentation and Library Guidance

### 2.1 Update README and docs
- Expand README feature and build sections with explicit attribute-driven messaging.
- Add an `Attribute-driven design` section in `docs/guide.md` or new `docs/attributes.md`.
- Ensure `docs/api.md` documents:
  - public parse/stringify APIs
  - runtime configuration via `Options`
  - custom I/O, custom stringifiers, and custom parsers
  - secure parsing options

### 2.2 Example coverage
- Add or update examples in `examples/source/` to demonstrate:
  - secure/untrusted input parsing
  - custom `IDestination` and `IStringify`
  - `Options::secureOptions()` usage
  - error handling for `SyntaxError` and no-exceptions path
- Make one example focus on portability and configuration with `CMake` options.

### 2.3 Developer hygiene docs
- Update `docs/testing.md` with an attribute checklist:
  - how to add tests for reliability and security
  - how to add header compile tests for portability
  - how to verify optional feature flags

---

## Phase 3: Reliability and Security

### 3.1 Parser robustness
- Review `classes/source/implementation/parser/YAML_Parser.cpp` and supporting parser units.
- Add or tighten checks around:
  - alias loops and alias expansion limits
  - maximum document count
  - parse depth limits
  - buffer overrun and source exhaustion conditions
- Ensure `Options::secureOptions()` is a first-class documented configuration.

### 3.2 Error handling
- Confirm `SyntaxError` and runtime error types are consistent in `classes/include/implementation/common/YAML_Error.hpp`.
- Add tests for:
  - malformed YAML
  - unsupported features or invalid tags
  - parser panic handler in `YAML_LIB_NO_EXCEPTIONS` builds
- Review and document the `YAML_LIB_NO_EXCEPTIONS` code path and custom panic handler registration.

### 3.3 Secure defaults
- Make secure defaults the recommended path in docs and examples.
- Ensure `Options::secureOptions()` sets limits on:
  - `max_documents`
  - `max_parse_depth`
  - `max_alias_expansions`
  - `max_node_size` or equivalent if present

---

## Phase 4: Performance and Efficiency

### 4.1 Parser and memory paths
- Review `classes/include/implementation/common/YAML_Arena.hpp` and parser memory allocation.
- Add documentation and tests for efficient reuse of parser state where possible.
- Avoid unnecessary heap allocations in the node tree and string handling.

### 4.2 Code locality and compile efficiency
- Reduce public header inclusion surface to minimize compile-time dependencies.
- Keep `classes/include/YAML_Core.hpp` from pulling internal-only files into public compilation units unless necessary.
- Validate the precompiled header `stdafx.h` usage and remove unused includes if it increases build time.

### 4.3 Benchmark candidate
- Add a lightweight performance test or example that parses and stringifies a medium YAML document.
- Use the benchmark to validate that refactors do not regress parser throughput or memory use.

---

## Phase 5: Maintainability and Modularity

### 5.1 Modular source layout
- Confirm implementation details stay under `classes/include/implementation/*` and `classes/source/implementation/*`.
- Ensure public headers do not leak internal implementation headers.
- Introduce clear folder boundaries for parser, stringify, I/O, translator, node, and variant modules.

### 5.2 Code readability
- Refactor large public headers into smaller focused headers where it improves comprehension.
- Add more inline comments and documentation in parser state management and node construction paths.
- Standardize naming conventions across public interfaces and internal types.

### 5.3 Build configuration
- Validate `CMakeLists.txt` options and feature flags:
  - `YAML_LIB_NO_EXCEPTIONS`
  - `YAML_LIB_FILE_IO`
  - `YAML_LIB_TIMESTAMP_PARSE`
  - `YAML_LIB_SAX_API`
- Confirm optional features are isolated and do not increase the default dependency footprint.

---

## Phase 6: Testing and Quality Gate

### 6.1 Expand unit tests
- Add new tests under `tests/source/misc/` for attribute-driven behavior:
  - `YAML_Lib_Tests_API_Intuitive.cpp`
  - `YAML_Lib_Tests_Security.cpp`
  - `YAML_Lib_Tests_Flexibility.cpp`
- Add coverage for:
  - public API stable surface
  - `Options::secureOptions()` and parser limits
  - custom `IDestination` and `IStringify` extensions
  - `isA<T>` / `NRef<T>` type-safety patterns

### 6.2 Portability and header compile
- Keep the existing header-only compile test in `tests/CMakeLists.txt`.
- Add or update tests that exercise `YAML_LIB_FILE_IO=OFF` and `YAML_LIB_NO_EXCEPTIONS=ON` paths if feasible.

### 6.3 Test documentation
- Document how to run the unit test suite and fuzz harness in `README.md` and `docs/testing.md`.
- Align test labels with attributes: `unit`, `security`, `portability`, `testability`.

---

## Phase 7: Implementation Work Plan

### Step 1: Baseline audit
- Confirm actual public header install set in `CMakeLists.txt`.
- Enumerate public symbols from `YAML.hpp`, `YAML_Core.hpp`, and `YAML_Interfaces.hpp`.
- Create an `attribute checklist` document in `docs/` or `notes/` if needed.

### Step 2: API and header cleanup
- Refactor `classes/include/YAML.hpp` to expose only the stable top-level API.
- Move internal-only headers behind a clear path and update `CMakeLists.txt` include directories accordingly.

### Step 3: Documentation work
- Update `README.md` and `docs/api.md`.
- Add new examples or refresh existing examples that highlight secure and flexible usage.

### Step 4: Security/reliability hardening
- Harden parser limits in `classes/source/implementation/parser/*`.
- Add explicit alias and depth tests in `tests/source/parse/`.
- Add defensive tests for `YAML_LIB_NO_EXCEPTIONS` parse failure paths.

### Step 5: Performance and portability validation
- Run build with default options, `YAML_LIB_FILE_IO=OFF`, and `YAML_LIB_NO_EXCEPTIONS=ON` to validate all code paths.
- Add a benchmark-style test or example to confirm no regressions.

### Step 6: Release and review
- Update docs and changelog with the new attribute-driven refactor.
- Run `cmake -S . -B build && cmake --build build` and `ctest --test-dir build`.
- Review failures and iterate.

---

## File and module targets

### Public headers
- `classes/include/YAML.hpp`
- `classes/include/YAML_Core.hpp`
- `classes/include/interface/YAML_Interfaces.hpp`
- `classes/include/interface/*.hpp`

### Core implementation
- `classes/source/YAML.cpp`
- `classes/source/YAML_Utility.cpp`
- `classes/source/implementation/YAML_Impl.cpp`
- `classes/source/implementation/YAML_Impl_Stringify.cpp`
- `classes/source/implementation/YAML_Impl_Access.cpp`
- `classes/source/implementation/parser/*.cpp`
- `classes/include/implementation/common/YAML_Arena.hpp`
- `classes/include/implementation/common/YAML_Error.hpp`

### Build and docs
- `CMakeLists.txt`
- `docs/api.md`
- `docs/guide.md`
- `docs/testing.md`
- `README.md`

### Tests
- `tests/CMakeLists.txt`
- `tests/source/misc/YAML_Lib_Tests_HighTestability.cpp`
- new tests under `tests/source/misc/`
- `tests/source/parse/*`
- `tests/source/io/*`

---

## Success criteria
- The public API is clearly defined and easier to use.
- Documentation now explicitly maps to the 10 library attributes.
- Parser limits and security options are documented and tested.
- Test coverage includes the public API, option permutations, and portability paths.
- Build remains clean with no extra runtime dependencies beyond the C++ standard library.
- The library is easier to maintain and extend through clearer modular boundaries.
