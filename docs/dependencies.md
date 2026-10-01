# YAML_Lib Dependencies

YAML_Lib is designed to minimize external dependencies.

## Runtime Dependencies

- **C++23 standard library**

The library itself does not require any external runtime libraries beyond the C++ standard library (`<expected>`, `<format>`, `<variant>`, `<memory_resource>`, etc.). All core YAML parsing, node manipulation, and stringification are implemented directly within the repository.

## Build Dependencies

- **CMake 3.18+** — required for configuring and generating build files.
- **A standard-compliant C++23 compiler**:
  - GCC 13.0 or newer
  - Clang 17.0 or newer
  - Apple Clang 16.0 or newer (Xcode 16+)
  - Microsoft Visual C++ 2022 v17.8 or newer (toolset v143)

## Optional Dependencies

- **Catch2 (BSL-1.0)** — used only for unit and integration tests.
  - Enabled with `-DBUILD_YAML_TESTS=ON`.
  - Automatically fetched via CMake `FetchContent` during test builds.
  - The test suite is not required to consume the library.
- **PLOG (MIT)** — used only by example programs for console logging.
  - Included under `examples/include/external/plog`.
  - `BUILD_YAML_EXAMPLES` controls whether examples are built.

## Optional Feature Flags

YAML_Lib exposes several feature toggles to keep the dependency footprint small for specific targets:

- `YAML_LIB_FILE_IO` — enable file-stream support for `FileSource`, `FileDestination`, `YAML::fromFile()`, `YAML::toFile()`, and `YAML::getFileFormat()`.
- `YAML_LIB_NO_EXCEPTIONS` — disable exception handling (`-fno-exceptions`) and use the panic-handler path.
- `YAML_LIB_SAX_API` — enable streaming event parsing via `IYAMLEvents` and `YAML::traverseEvents()`.
- `YAML_LIB_TIMESTAMP_PARSE` — enable ISO 8601 timestamp parsing helpers.

These options are build-time feature flags only. Disabling them removes the corresponding functionality from the compiled library and keeps the runtime dependency footprint minimal.

To build the smallest possible library footprint, disable optional features and optional targets:

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

See [docs/embedded_and_minimal.md](embedded_and_minimal.md) for detailed configuration advice.

## License Summary

- `YAML_Lib` source: MIT License (`LICENSE.txt`).
- Catch2: BSL-1.0 (optional test dependency).
- PLOG: MIT License (optional examples dependency).
