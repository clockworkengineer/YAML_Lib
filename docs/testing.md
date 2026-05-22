# Testing YAML_Lib

YAML_Lib includes unit tests, integration tests, and parser resilience coverage. This document explains how to run tests, add new ones, and extend test coverage for public APIs and custom I/O.

## Running tests

1. Configure and build the project:

```sh
cmake -S . -B build
cmake --build build
```

2. Run the full test suite with CTest:

```sh
ctest --test-dir build --output-on-failure
```

3. Validate hardened build configurations:

```sh
cmake -S . -B build_no_file_io -DYAML_LIB_FILE_IO=OFF
cmake --build build_no_file_io --target YAML_Lib

cmake -S . -B build_minimal \
  -DBUILD_YAML_EXAMPLES=OFF \
  -DBUILD_YAML_TESTS=OFF \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_SAX_API=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF
cmake --build build_minimal --target YAML_Lib

cmake -S . -B build_no_exceptions -DYAML_LIB_NO_EXCEPTIONS=ON
cmake --build build_no_exceptions --target YAML_Lib

> Note: Catch2-based unit tests cannot run under `-fno-exceptions`. The library build itself is the best no-exceptions validation, and this repository includes a small compile-only target `YAML_Lib_NoExceptions_Compile_Tests` to verify the panic handler API.```

4. Refer to `docs/attribute-checklist.md` for a Phase 7 audit summary of library attributes.

3. To build and run fuzz tests:

```sh
cmake -S . -B build -DBUILD_YAML_PARSER_FUZZ_TESTS=ON
cmake --build build
./build/tests/YAML_Lib_Fuzz_Tests
```

## Test structure

- `tests/source/parse/` — parser feature and compliance tests
- `tests/source/stringify/` — stringifier output tests for YAML, JSON, XML, and Bencode
- `tests/source/io/` — source/destination I/O implementation tests
- `tests/source/misc/` — API behavior, option coverage, custom parser/stringifier behaviors, and security tests
- `tests/include/YAML_Lib_Tests.hpp` — shared test utilities and Catch2 configuration

## Adding tests

Use Catch2 `TEST_CASE` and `SECTION` for new coverage.

### Example

Add a new file under `tests/source/misc/` or a domain-specific subfolder.

```cpp
TEST_CASE("YAML::Options preserves strict boolean parsing", "[YAML][Options][Parse]") {
  YAML_Lib::Options options;
  options.strict_booleans = true;

  YAML_Lib::YAML yaml(options);
  yaml.parse(YAML_Lib::BufferSource{"---\nvalue: yes\n"});

  REQUIRE(isA<YAML_Lib::String>(yaml.document(0)["value"]));
}
```

## Extending tests with custom I/O and stringifiers

YAML_Lib was designed for testability. You can implement `ISource`, `IDestination`, `IStringify`, and `IParser` directly in tests to verify custom behaviors without relying on production I/O.

- `ISource` custom test sources can simulate streams, files, or protocol buffers.
- `IDestination` test destinations can capture or validate output without filesystem dependencies.
- `IStringify` fake stringifiers can verify `YAML::stringify()` uses the configured serializer.
- `IParser` fake parsers can verify `YAML::parse()` delegates to custom parsing logic.

A concrete test example is available in `tests/source/misc/YAML_Lib_Tests_Testability.cpp`.

## Attribute 8 — High Testability

This library tracks testability through focused, attribute-driven test coverage.

- Use consistent Catch2 tags to group related cases: `[YAML][Options][Validation]`, `[YAML][Testability]`, `[YAML][API][Integration]`, `[YAML][Security]`.
- Add new tests under `tests/source/misc/` or domain-specific folders when covering API quality, runtime options, or custom adapters.
- The `YAML_Lib_Unit_Tests` executable is now labeled for CTest as `unit`, `testability`, and `api`.
- The fuzz harness executable is labeled `fuzz` and `security`.

Run the unit test suite with CTest labels:

```sh
ctest --test-dir build -L unit --output-on-failure
```

Run security-focused fuzz/resilience tests if enabled:

```sh
ctest --test-dir build -L fuzz --output-on-failure
```

For example-based integration coverage, enable example builds and run a small example program after build:

```sh
cmake -S . -B build -DBUILD_YAML_EXAMPLES=ON
cmake --build build
./build/examples/YAML_Simple_Read_Write
```

This helps keep example programs as lightweight integration checks for the public API.

## Best practices

- Keep tests small and focused on one behavior.
- Use `REQUIRE` for hard assertions and `CHECK` for non-fatal checks.
- Avoid depending on external files unless testing file I/O explicitly.
- Use fake sources/destinations for parser or stringifier contract testing.

## Code style and maintainability

- Keep public API headers lean and stable; internal implementation headers should remain hidden unless explicitly exposed by `YAML_Core.hpp`, `YAML.hpp`, or the public interface.
- Use the existing `YAML_Lib_Tests_HeaderCompile` target as a maintainability gate for public header stability.
- Add focused public API tests for top-level helpers such as `YAML::load()`, `YAML::dump()`, `YAML::toString()`, and `YAML::loadFile()`.
- Prefer one responsibility per source file, especially in parser and stringify implementation modules.
- Document all public interfaces with clear lifetime and ownership semantics for callers.
- Use consistent doxygen-style comments for stable public contracts, and keep implementation details in `classes/source/implementation`.

## Running a single test case

Use CTest with a regex filter:

```sh
ctest --test-dir build -R "YAML_Lib_Unit_Tests" --output-on-failure
```

Or run a specific Catch2 executable directly with section matching:

```sh
./build/tests/YAML_Lib_Unit_Tests -c "YAML\[Testability\]"
```
