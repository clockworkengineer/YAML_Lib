#!/usr/bin/env bash
set -euo pipefail

cmake -S . -B build_header_default \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_YAML_TESTS=ON \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF
cmake --build build_header_default --target YAML_Lib_Header_Compile_Tests

cmake -S . -B build_header_minimal \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_YAML_TESTS=ON \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_SAX_API=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF
cmake --build build_header_minimal --target YAML_Lib_Header_Compile_Tests

cmake -S . -B build_header_no_exceptions \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_YAML_TESTS=ON \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF \
  -DYAML_LIB_NO_EXCEPTIONS=ON
cmake --build build_header_no_exceptions --target YAML_Lib_Header_Compile_Tests
