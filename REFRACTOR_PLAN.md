# YAML_Lib Refactor Plan

## Purpose
This plan refactors `YAML_Lib` to fully implement the 10 library attributes from `notes/attributes.md`.
It maps the library's existing design to the desired qualities, identifies gaps, and defines concrete implementation tasks.

## Summary of current state
- The library already provides a strong C++20 API, parser/stringifier extensibility, and optional build-time features.
- Public API exposure is currently broad: many internal headers are installed and exposed via CMake.
- Documentation is good in README, but there is no dedicated developer-facing attribute implementation plan or a concise public API reference mapping.
- Reliability is mostly enforced by exception-based errors, but there is no explicit plan for no-exception / panic modes beyond build flags.
- Performance and memory resource support exists, but internal container allocation and parser allocation paths should be audited and hardened.
- Test coverage exists, but it is not explicitly tied to the ten library attributes.

## Refactor goals by attribute

### 1. Intuitive API Design
Goal: Make the public API easy to discover, use, and extend.

Tasks:
- Consolidate the public install interface to a minimal header set. Only install `YAML.hpp`, `YAML_Core.hpp`, `YAML_Interfaces.hpp`, and explicit public `implementation` headers that are intended for extension.
- Hide low-level internal headers behind `classes/include/implementation` and keep them out of the installed include path unless genuinely public.
- Add explicit convenience entry points:
  - `YAML::fromString()` / `YAML::fromFile()` static factory functions
  - `YAML::toString()` / `YAML::toFile()` convenience methods
  - alias-friendly names like `YAML::load()` / `YAML::dump()` if they improve usability without clutter.
- Normalize naming conventions across `Options` and node access methods. For example, choose consistent snake_case vs camelCase for configuration options and method names.
- Document lifetime rules clearly in header comments for objects like `Options`, `ISource`, `IDestination`, and `std::pmr::memory_resource`.

### 2. Comprehensive Documentation
Goal: Make it easy for users and contributors to understand usage, internals, and safety.

Tasks:
- Create a dedicated public API reference document in `docs/public_api.md` or improve the existing one, linked from `README.md`.
- Add a new `docs/attribute-driven-refactor.md` or update `docs/guide.md` with a section that explains how the library implements the ten attributes.
- Add examples for:
  - custom `IParser` / `IStringify`
  - `Options` usage with `strictBooleans`, `maxParseDepth`, and `memory_resource`
  - `YAML_LIB_NO_EXCEPTIONS` panic-handler usage
- Document supported build options and safe defaults in a new section of `docs/portability.md` or `README.md`.
- Add precise API comments for public interfaces and options in `classes/include/YAML.hpp`.

### 3. High Reliability
Goal: Make the code predictable and robust with explicit defensive behavior.

Tasks:
- Audit parser limits and enforce them at all parse entry points:
  - document count
  - parse depth
  - alias expansion
- Add defensive validation of custom runtime objects:
  - `YAML::Options` should validate `memory_resource` and avoid dereferencing null pointers in custom interfaces.
  - `YAML::parse()` / `stringify()` should emit clear errors when provided invalid sources/destinations.
- Add dedicated unit tests for error conditions in `tests/source/`:
  - invalid YAML documents and recovery paths
  - alias loops and merge key abuse
  - invalid BOM handling in file I/O
- Add test coverage for `YAML_LIB_NO_EXCEPTIONS=ON` and `YAML_LIB_FILE_IO=ON/OFF` combinations.
- Introduce `YAML::tryParse` or a safe parse wrapper if it improves reliability for users who prefer status-code handling over exceptions.

### 4. Performance and Efficiency
Goal: Keep parsing/stringifying fast while minimizing memory use.

Tasks:
- Audit and extend `Options.memory_resource` usage so all internal containers and node allocations can use `std::pmr` when provided.
- Review internal parser allocation patterns for string copies and temporary buffers; replace unnecessary copies with `std::string_view` or `std::pmr::string` where safe.
- Add reserve/capacity hints in sequence and dictionary containers during parse if size can be predicted.
- Profile the parser and stringify implementations to identify hot paths; document any large allocations and optimize them incrementally.
- Ensure `classes/source/implementation/parser` and `stringify` modules avoid redundant buffer copies.

### 5. Maintainability
Goal: Make the library easy to update, refactor, and extend without breaking ABI.

Tasks:
- Separate public headers from internal headers in CMake and on install. Create a clean boundary between `include/` and `implementation/` headers.
- Make internal implementation types opaque where possible using `YAML_Impl` PIMPL or hidden details in source files.
- Refactor large parser sources into smaller, well-named units and ensure each file has one obvious responsibility.
- Add a code-style reference for maintainers in `docs/testing.md` or `docs/contributing.md`.
- Add comments or doc blocks for all public interface classes explaining their intended stable semantics.

### 6. Flexibility and Customization
Goal: Make it easy to customize parsing, formatting, and I/O.

Tasks:
- Verify that `YAML::Options` can install custom `IParser`, `IStringify`, and memory resource consistently.
- Add or improve examples for custom `ISource`, `IDestination`, and `IStringify` implementations.
- Consider adding runtime stringifier options or a `StringifyOptions` struct for format-specific behavior.
- Ensure `IYAMLEvents` SAX API is well documented and maintainer-friendly when enabled.
- Provide stable extension points for new formats, custom tag handling, and custom scalar translators.

### 7. Strong Security
Goal: Protect users from malformed, malicious, or resource-exhausting YAML inputs.

Tasks:
- Promote safe defaults in docs and code. For untrusted input, the library should encourage:
  - `maxDocuments = 1`
  - `maxParseDepth = 64`
  - `maxAliasExpansions = 64`
  - `strictBooleans = true`
- Harden alias handling in `YAML_Parser_Directive.cpp` and ensure alias cycles cannot bypass configured limits.
- Add unit tests for alias explosion and deeply nested YAML to confirm limits work.
- Document `YAML_LIB_NO_EXCEPTIONS` security tradeoffs and panic handler contract clearly.
- Validate file I/O and stream state aggressively, rejecting invalid or partial input cleanly.

### 8. High Testability
Goal: Ensure the library can be tested thoroughly and predictably.

Tasks:
- Add attribute-driven test cases grouped by API quality dimensions, e.g. `tests/source/test_intuitive_api.cpp`, `tests/source/test_security_limits.cpp`.
- Ensure examples can be compiled and run as lightweight integration tests.
- Add tests covering `YAML::Options` edge cases, custom I/O adapters, and alternative formats.
- Expand test docs with a short guide on how to add new public and internal tests.
- Add CTest labels or groups if not already present, so maintainers can run targeted attribute-related suites.

### 9. Compatibility and Portability
Goal: Keep the library buildable and usable on all supported platforms.

Tasks:
- Keep platform-specific conversion code isolated in `classes/source/implementation/converter/{linux,windows}`.
- Document expected platform behavior and compiler requirements in `docs/portability.md` and `README.md`.
- Add a regression test that validates the public header set builds cleanly with `-Wall -Werror -pedantic` on Linux and equivalent MSVC flags.
- Reduce header exposure of platform-specific internals and ensure `YAML.hpp` remains portable.

### 10. Low Dependency Footprint
Goal: Minimize runtime and install-time dependencies.

Tasks:
- Ensure public headers do not require optional runtime dependencies such as Catch2 or PLOG.
- Confirm `YAML_LIB_FILE_IO`, `YAML_LIB_SAX_API`, and `YAML_LIB_TIMESTAMP_PARSE` are build-time options only, not required defaults.
- Document the minimal dependency footprint clearly in `README.md` and `docs/dependencies.md`.
- Keep install target dependencies limited to C++ standard library and optional components only when enabled.

## Concrete implementation roadmap

1. Public header / install refactor
   - Reduce installed header set to only stable public headers.
   - Remove internal implementation directories from `target_include_directories` public interface where possible.
   - Update `docs/public_api.md` and `README.md` to list exported headers.

2. API convenience and clarity
   - Add static factory methods and convenience wrappers for parsing/stringifying.
   - Add `YAML::Options` validation and explicit lifetime doc comments.
   - Standardize naming conventions in `YAML.hpp` and `Options`.

3. Documentation and developer guidance
   - Add a refactor-attribute guide in `docs/`.
   - Add usage examples for custom extensions and safety options.
   - Add API coverage notes to `docs/public_api.md`.

4. Reliability / parser limit hardening
   - Audit and enforce limit checks in parser sources.
   - Add error-handling tests for invalid input and custom interface failure modes.
   - Add `YAML::tryParse` or status-returning alternative if useful.

5. Memory and performance improvements
   - Audit PMR usage across parsing and node representation.
   - Replace redundant parsing string copies with `std::string_view` / arena-backed memory when safe.
   - Add tests demonstrating custom memory resource use.

6. Test expansion and attribute coverage
   - Create new tests aligned with the 10 library attributes.
   - Add targeted unit tests for security, compatibility, and customization.
   - Improve existing test documentation.

7. Portability and build hygiene
   - Strengthen CMake install rules and build targets.
   - Add platform compatibility documentation and targeted build validation.

## Suggested file locations for refactor work
- `classes/include/YAML.hpp`
- `classes/include/YAML_Core.hpp`
- `classes/include/interface/YAML_Interfaces.hpp`
- `classes/source/implementation/parser/*`
- `classes/source/implementation/common/YAML_Error.cpp`
- `classes/source/implementation/translator/*`
- `classes/source/implementation/stringify/*`
- `CMakeLists.txt`
- `README.md`
- `docs/public_api.md`
- `docs/portability.md`
- `docs/testing.md`
- `tests/source/`

## Immediate next steps
1. Create `docs/public_api.md` and a dedicated `docs/attribute-driven-refactor.md` entry.
2. Refactor CMake public include exports and public header install list.
3. Add the first incremental tests for parser limit enforcement and `Options` validation.
4. Add a small API convenience wrapper set in `classes/include/YAML.hpp`.

---

This refactor plan can be used as a baseline for implementation sprints that align the codebase with the 10 attributes from `notes/attributes.md`.
