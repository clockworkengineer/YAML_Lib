# YAML_Lib Dependencies

YAML_Lib is designed to minimize external dependencies.

## Runtime dependencies

- C++20 standard library

The library itself does not require any external runtime libraries beyond the C++ standard library. All core YAML parsing, node manipulation, and stringification is implemented in the repository.

## Build dependencies

- CMake 3.18+ — required for configuring and generating build files.
- A C++20-compatible compiler:
  - GCC 10+
  - Clang 15+
  - MSVC 2019+

## Optional dependencies

- Catch2 (BSL-1.0) — used only for unit and integration tests.
  - Enable with `-DBUILD_YAML_TESTS=ON`.
  - The test suite is not required to consume the library.
- PLOG (MIT) — used only by example programs for logging.
  - Included under `examples/include/external/plog`.
  - `BUILD_YAML_EXAMPLES` controls whether examples are built.

## Optional feature flags

YAML_Lib exposes several feature toggles to keep the dependency footprint small for specific targets:

- `YAML_LIB_FILE_IO` — enable file-stream support for `FileSource`, `FileDestination`, `YAML::fromFile()`, `YAML::toFile()`, and `YAML::getFileFormat()`.
- `YAML_LIB_NO_EXCEPTIONS` — disable exception handling and use the panic-handler path.
- `YAML_LIB_SAX_API` — enable streaming event parsing via `IYAMLEvents` and `YAML::traverseEvents()`.
- `YAML_LIB_TIMESTAMP_PARSE` — enable ISO 8601 timestamp parsing helpers.

These options are build-time feature flags only. Disabling them removes the corresponding functionality from the compiled library and keeps the runtime dependency footprint minimal.

To build the smallest possible library footprint, disable optional features and optional targets:

```sh
cmake -S . -B build_minimal \
  -DBUILD_YAML_EXAMPLES=OFF \
  -DBUILD_YAML_TESTS=OFF \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_SAX_API=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF
cmake --build build_minimal
```

## Header-only support

A dedicated header-only distribution is not currently provided. The library compiles as a static library with its implementation split between headers and source files.

## License summary

- `YAML_Lib` source: MIT License (`LICENSE`).
- Catch2: BSL-1.0 (optional test dependency).
- PLOG: MIT License (optional examples dependency).
