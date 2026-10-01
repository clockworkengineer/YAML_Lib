#!/bin/bash
set -euo pipefail

CC_FLAG=""
CXX_FLAG=""
if [ -n "${CC:-}" ]; then
  CC_FLAG="-DCMAKE_C_COMPILER=${CC}"
fi
if [ -n "${CXX:-}" ]; then
  CXX_FLAG="-DCMAKE_CXX_COMPILER=${CXX}"
fi

cmake -S . -B build_minimal \
  -DCMAKE_BUILD_TYPE=Release \
  ${CC_FLAG} \
  ${CXX_FLAG} \
  -DBUILD_YAML_EXAMPLES=OFF \
  -DBUILD_YAML_TESTS=OFF \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_SAX_API=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF
cmake --build build_minimal --target YAML_Lib
