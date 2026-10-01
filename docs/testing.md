# Testing YAML_Lib

`YAML_Lib` includes a comprehensive automated test framework encompassing unit tests, integration tests, contract verification, header compile isolation tests, fuzz harnesses, and sanitizer builds.

This document details how to run tests, add new test cases, and validate specialized build configurations.

---

## Running Tests

### 1. Configure and Build the Test Suite

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_YAML_TESTS=ON
cmake --build build -j$(nproc)
```

### 2. Execute Full Test Suite via CTest

```sh
ctest --test-dir build --output-on-failure
```

### 3. Running Specific Subsets

To execute only the unit tests:
```sh
ctest --test-dir build -R "YAML_Lib_Unit_Tests" --output-on-failure
```

To execute header compilation independence tests:
```sh
ctest --test-dir build -R "YAML_Lib_Header_Compile_Tests" --output-on-failure
```

To run a specific Catch2 tag or test section directly:
```sh
./build/tests/YAML_Lib_Unit_Tests "[Cpp23]"
./build/tests/YAML_Lib_Unit_Tests "[YAML][API]"
```

---

## Validating Specialized Build Configurations

### 1. Minimal Embedded Build (Zero Filesystem & Zero SAX)
```sh
cmake -S . -B build_minimal \
  -DBUILD_YAML_EXAMPLES=OFF \
  -DBUILD_YAML_TESTS=OFF \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_SAX_API=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF
cmake --build build_minimal --target YAML_Lib
```

### 2. Exception-Free Build (`-fno-exceptions`)
```sh
cmake -S . -B build_no_exceptions -DYAML_LIB_NO_EXCEPTIONS=ON
cmake --build build_no_exceptions --target YAML_Lib_NoExceptions_Compile_Tests
```
> **Note**: Catch2 requires exceptions to report assertion failures. Therefore, unit test executables are excluded under `-fno-exceptions`. Instead, the repository provides the compile-only target `YAML_Lib_NoExceptions_Compile_Tests` to ensure error handling, panic callbacks, and non-throwing code paths compile cleanly without `-fexceptions`.

### 3. Address & Undefined Behavior Sanitizers
Run the dedicated sanitizer build script:
```sh
./scripts/Linux-Build-Sanitizers.sh
```

### 4. Fuzz Testing
```sh
cmake -S . -B build_fuzz -DBUILD_YAML_PARSER_FUZZ_TESTS=ON
cmake --build build_fuzz
./build_fuzz/tests/YAML_Lib_Fuzz_Tests
```

---

## Test Directory Structure

- `tests/source/misc/YAML_Lib_Tests_Cpp23.cpp` — Comprehensive C++23 test suite:
  - `std::expected` non-throwing parsing (`loadExpected`, `parseExpected`)
  - Modern ergonomic accessors (`as<T>()`, `is<T>()`, `value_or()`, `get_if<T>()`)
  - Container auto-promotion on `operator[]`
  - Structured bindings and iteration with `items()`
  - Sequence range-based `for` loops
  - Native `std::format` integration (`YAML_Format.hpp`)
  - Non-intrusive object serialization and STL container mapping (`YAML_Serialization.hpp`)
- `tests/source/misc/YAML_Lib_Tests_HeaderCompile.cpp` & `YAML_Lib_Tests_Header_Interfaces.cpp` — Verifies every public header compiles in total isolation without implicit prerequisites (IWYU).
- `tests/source/contract/YAML_Lib_Tests_SourceContract.cpp` — Formal behavioral contract testing for `ISource` and `IDestination` implementations.
- `tests/source/parse/` — Spec compliance and parser edge-case suites (including 758 YAML Test Suite cases).
- `tests/source/stringify/` — Output generators for YAML, JSON, XML, and Bencode.
- `tests/source/io/` — Buffer, file, and stream I/O adapter tests.
- `tests/include/YAML_Lib_Tests.hpp` — Shared Catch2 test environment and utilities.

---

## Adding New Tests

Add new test cases using Catch2 `TEST_CASE` and `SECTION`:

```cpp
#include "YAML_Lib_Tests.hpp"
#include "YAML_Serialization.hpp"

struct Point { int x; int y; };
YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(Point, x, y)

TEST_CASE("Custom Point serialization", "[YAML][Cpp23][Serialization]") {
  Point p{10, 20};
  YAML_Lib::Node n;
  to_yaml(n, p);

  REQUIRE(n["x"].as<int>() == 10);
  REQUIRE(n["y"].as<int>() == 20);

  Point restored;
  from_yaml(n, restored);
  REQUIRE(restored.x == 10);
  REQUIRE(restored.y == 20);
}
```

Ensure formatting conforms to `.clang-format` before committing:
```sh
./scripts/Linux-Style-Check.sh
```
