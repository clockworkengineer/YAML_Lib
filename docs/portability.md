# YAML_Lib Portability

`YAML_Lib` is engineered to build and run across major platforms using C++ standard library features and configuration options that minimize platform-specific dependencies.

---

## Supported Platforms & Compilers

`YAML_Lib` 1.4.0 requires **C++23** (`CMAKE_CXX_STANDARD 23`):

- **Linux**: GCC 13.0+ / Clang 17.0+
- **macOS**: Apple Clang 16.0+ / Xcode 16+
- **Windows**: MSVC 2022 v17.8+ (v143 toolset)

---

## Toolchain & Platform Portability Notes

### 1. Clang on Linux with libstdc++ 14 (LLVM #93734 Workaround)
In C++23 mode, pairing Clang 17 or 18 with GCC 14's `libstdc++` headers triggers constraint failures in `std::pair` and `std::tuple` constructors (`no matching function for call to 'get'`). 
To ensure seamless, warning-free builds:
- `CMakeLists.txt` automatically detects if GCC 13's libstdc++ directory (`/usr/lib/gcc/*-linux-gnu/13`) is present when compiling with Clang on Linux, passing `--gcc-install-dir` to Clang.
- In `classes/include/implementation/common/YAML_Error.hpp`, constructors avoid default arguments on `std::pair` positions to prevent triggering pair conversion template checks.

### 2. Apple Clang Template Syntax
On macOS with Apple Clang (Xcode 16), the compiler strictly diagnoses `-Wmissing-template-arg-list-after-template-kw`. Member function templates invoked without explicit `<...>` template arguments on non-dependent types must not use the `template` keyword prefix:
```cpp
// Correct & portable:
return (*this)[key].value_or(std::forward<T>(fallback));

// Incorrect (rejected by Apple Clang):
return (*this)[key].template value_or(std::forward<T>(fallback));
```

### 3. Windows / MSVC Multi-Configuration Generators
When building with Visual Studio (`-G "Visual Studio 17 2022"`), CMake uses multi-configuration generators where binaries are located in `build/tests/Release/` rather than `build/tests/`. The test harness dynamically probes both working directory layouts to locate test data files reliably.

### 4. Unicode Transcoding
UTF-8, UTF-16, and UTF-32 conversion is implemented in standard C++23 in `classes/source/implementation/converter/YAML_Converter.cpp` (and `YAML_Converter.hpp`), completely avoiding deprecated `<codecvt>` or Windows-specific `WideCharToMultiByte` calls.

---

## Supported Build Configurations

YAML_Lib is built with CMake 3.18+ and targets C++23.

### Recommended Options

- `YAML_LIB_NO_EXCEPTIONS` — compile with `-fno-exceptions` and use registered panic handlers.
- `YAML_LIB_FILE_IO` — enable file I/O support for file-based parsing/stringification.
- `YAML_LIB_SAX_API` — enable streaming event parsing.
- `YAML_LIB_TIMESTAMP_PARSE` — enable `Timestamp` conversion helpers.
- `BUILD_YAML_PARSER_FUZZ_TESTS` — build the parser fuzz harness.

These flags are build-time only. The library's core runtime remains dependent only on the C++ standard library when optional features are disabled.

> When compiling with `YAML_LIB_NO_EXCEPTIONS=ON`, register a custom panic handler with `YAML_Lib::Error::setPanicHandler(...)` so errors can be logged or handled before the library aborts.

### Safe Defaults for Untrusted YAML

When parsing untrusted input, configure parser limits and strict boolean handling:

```cpp
YAML_Lib::Options options = YAML_Lib::Options::secureOptions();
```

These values help protect against document flooding, deeply nested structures, and alias-expansion attacks.

---

## Public Header Validation

A regression target compiles the stable public header set under strict flags to ensure portability and a clean public API surface:

```sh
ctest --test-dir build -R "YAML_Lib_Header_Compile_Tests" --output-on-failure
```

For public API contract hardening, validate across supported build configurations:
- Default feature set
- Minimal builds with `YAML_LIB_FILE_IO=OFF`, `YAML_LIB_SAX_API=OFF`, and `YAML_LIB_TIMESTAMP_PARSE=OFF`
- No-exceptions builds with `YAML_LIB_NO_EXCEPTIONS=ON`

---

## Cross-Platform Build Examples

### Linux & macOS:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### Windows (Visual Studio 2022):
```sh
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### Minimal Footprint Build:
```sh
cmake -S . -B build_minimal \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DBUILD_YAML_EXAMPLES=OFF \
  -DBUILD_YAML_TESTS=OFF \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_SAX_API=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF \
  -DYAML_LIB_NO_EXCEPTIONS=ON
cmake --build build_minimal
```
