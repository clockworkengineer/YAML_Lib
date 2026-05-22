# YAML_Lib Portability

YAML_Lib is designed to build and run on major desktop platforms using C++ standard library features and configuration options that minimize platform-specific dependencies.

## Supported platforms

- Linux: GCC 10+ / Clang 15+
- macOS: Apple Clang 15+ / Xcode 15+
- Windows: MSVC 2019+ / Visual Studio 2022

## Supported build configurations

YAML_Lib is built with CMake 3.18+ and targets C++20.

### Recommended options

- `YAML_LIB_NO_EXCEPTIONS` — disable exceptions and use the panic-handler runtime path.
- `YAML_LIB_FILE_IO` — enable file I/O support for file-based parsing/stringification.
- `YAML_LIB_SAX_API` — enable streaming event parsing.
- `YAML_LIB_TIMESTAMP_PARSE` — enable `Timestamp` conversion helpers.
- `BUILD_YAML_PARSER_FUZZ_TESTS` — build the parser fuzz harness.

These flags are build-time only. The library's core runtime remains dependent only on the C++ standard library when optional features are disabled.

### Safe defaults for untrusted YAML

When parsing untrusted input, configure parser limits and strict boolean handling:

```cpp
YAML_Lib::Options options = YAML_Lib::Options::secureOptions();
```

These values help protect against document flooding, deeply nested structures, and alias-expansion attacks.

## Platform-specific implementation notes

- Linux and macOS builds share the POSIX UTF converter implementation located under `classes/source/implementation/converter/linux/`.
- Windows builds use the Windows-specific converter implementation under `classes/source/implementation/converter/windows/`.
- `YAML.hpp` and `YAML_Core.hpp` are the stable public headers for portable integration; platform-specific internals remain isolated behind implementation directories.
- `YAML_LIB_FILE_IO=OFF` is useful for embedded or bare-metal environments where filesystem APIs are unavailable.

## Public header validation

A regression target compiles the stable public header set under strict flags to ensure portability and cleaner public API surface.

```sh
ctest --test-dir build -R "YAML_Lib_Header_Compile_Tests" --output-on-failure
```

## Cross-platform build examples

Linux/macOS:

```sh
cmake -S . -B build
cmake --build build
```

Windows:

```sh
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

Minimal footprint build:

```sh
cmake -S . -B build_minimal \
  -DBUILD_YAML_EXAMPLES=OFF \
  -DBUILD_YAML_TESTS=OFF \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_SAX_API=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF
cmake --build build_minimal
```

## Testing on different configurations

Use CTest to exercise the unit tests and fuzz harness.

```sh
ctest --test-dir build --output-on-failure
```

For a no-exceptions build that omits file I/O:

```sh
cmake -S . -B build_no_exceptions -DBUILD_YAML_TESTS=OFF -DYAML_LIB_NO_EXCEPTIONS=ON -DYAML_LIB_FILE_IO=OFF
cmake --build build_no_exceptions
```

> Note: The Catch2 test suite depends on C++ exceptions. Disable `BUILD_YAML_TESTS` for no-exceptions library builds and use a separate build configuration when running tests.
