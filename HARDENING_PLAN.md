# YAML_Lib Hardening Plan

## Goal
Harden YAML_Lib against untrusted input, parser abuse, and runtime exposure while preserving the existing modular build and test infrastructure.

## Current state
- `README.md` and `docs/portability.md` already recommend `YAML_Lib::Options::secureOptions()` for untrusted YAML.
- `classes/source/YAML.cpp` validates `Options` limits for `max_documents`, `max_parse_depth`, and `max_alias_expansions`.
- parser security tests exist under `tests/source/parse/YAML_Lib_Tests_Parse_SecurityLimits.cpp`.
- optional runtime features are isolated behind CMake flags such as `YAML_LIB_FILE_IO`, `YAML_LIB_SAX_API`, and `YAML_LIB_TIMESTAMP_PARSE`.
- `docs/testing.md` documents hardened build configurations and fuzz harness usage.

## Hardening objectives
1. Strengthen parser limits and input validation.
2. Harden alias/anchor handling and expansion semantics.
3. Reduce attack surface in optional feature code paths.
4. Verify exception-free runtime and custom error handling paths.
5. Expand coverage with security tests, fuzzing, sanitizers, and static analysis.
6. Document safe build modes, runtime expectations, and secure API use.

## Concrete action items

### 1. Parser boundary enforcement
- Audit `YAML::Options::validate()` in `classes/source/YAML.cpp` and preserve strict caps.
- Add explicit parser limits beyond current options if not already present:
  - maximum scalar/string length
  - maximum collection size per sequence/mapping
  - maximum anchor table size or alias count per document
- Ensure all parser entrypoints use validated `Options` before parse begins.
- Add regression tests for invalid `Options` values and out-of-range parse limits.

### 2. Alias and anchor hardening
- Review anchor/alias handling in `classes/source/implementation/parser/YAML_Parser_Directive.cpp`.
- Confirm recursive anchor loops and alias expansion limits are enforced consistently.
- Add tests for:
  - alias graphs that grow exponentially without cycles
  - repeated alias expansion near the limit
  - alias-based denial-of-service patterns in flow and block contexts
- Consider adding a separate `Options::max_anchor_nodes` or similar if needed.

### 3. Input validation and malformed content
- Audit YAML parser utilities in `classes/source/implementation/parser/YAML_Parser_*.cpp` for robust invalid input handling.
- Add tests for malformed sequences, tags, invalid escapes, truncated UTF, and BOM handling.
- Harden file I/O path validation in `classes/source/implementation/file/YAML_File.cpp`:
  - ensure unsupported BOM formats are rejected cleanly
  - avoid filename or path string issues during error construction

### 4. Optional feature isolation and minimal safe build
- Validate the build matrix around `YAML_LIB_FILE_IO`, `YAML_LIB_SAX_API`, and `YAML_LIB_TIMESTAMP_PARSE`.
- Maintain a documented minimal safe build with only core parsing enabled.
- Audit any optional public APIs exposed by these features for safety and documentation.
- Add CI jobs for at least one minimal build configuration with tests.

### 5. Exception-free and panic handling path
- Validate `YAML_LIB_NO_EXCEPTIONS` path and custom panic handler registration via `YAML_Lib::setErrorHandler(...)`.
- Add tests for panic handler behavior and ensure the no-exceptions runtime does not abort unexpectedly.
- Document `YAML_LIB_NO_EXCEPTIONS` expectations in `docs/portability.md` and `docs/guide.md`.

### 6. Security-focused testing and tooling
- Enable parser fuzz harness `BUILD_YAML_PARSER_FUZZ_TESTS=ON` in CI and run it regularly.
- Add more targeted parser fuzz inputs for security/negative cases.
- Add sanitizers to the CI pipeline:
  - AddressSanitizer (ASAN)
  - UndefinedBehaviorSanitizer (UBSAN)
  - MemorySanitizer if feasible for the supported platforms
- Add static analysis or clang-tidy security checks to the build or pre-commit flow.
- Extend `docs/testing.md` with explicit security test labels and checklists.

### 7. Public API contract hardening
- Keep public headers stable and minimal, as documented in `docs/public_api.md`.
- Ensure all public parser/stringifier APIs clearly document ownership, lifetime, and safe usage for untrusted YAML.
- Add header-only compile validation for all supported configurations via `YAML_Lib_Header_Compile_Tests`.
- Add tests for public API misuse scenarios, e.g. invalid `ISource` implementations and incorrect `std::ostream` states.

### 8. Documentation and release guidance
- Add a security hardening section to `README.md`, `docs/guide.md`, and `docs/portability.md` with:
  - recommended safe options
  - minimal safe build flags
  - exception-free runtime tips
  - alias/anchor hardening guidance
- Keep `docs/attribute-checklist.md` updated with parser security and reliability hardening results.
- Maintain this `HARDENING_PLAN.md` as the living source for future security engineering work.

## Recommended priority order
1. Parser limit validation and alias/anchor protections.
2. Security-focused tests and fuzzing.
3. Exception-free runtime and optional feature isolation.
4. Documentation, build hardening, and public API contract coverage.

## Suggested verification steps
- Build and run `ctest --test-dir build --output-on-failure` on default and hardened builds.
- Build with `-DYAML_LIB_FILE_IO=OFF` and `-DYAML_LIB_NO_EXCEPTIONS=ON` and verify library target success.
- Run fuzz harness: `cmake -S . -B build -DBUILD_YAML_PARSER_FUZZ_TESTS=ON && cmake --build build && ./build/tests/YAML_Lib_Fuzz_Tests`.
- Add a CI job for one sanitized build and one minimal build.

## Notes
This plan is intentionally concrete and actionable. It leverages the library's existing security controls and documentation, while adding coverage for the remaining hardening gaps in parser behavior, build isolation, and runtime error paths.
