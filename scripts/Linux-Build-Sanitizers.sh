#!/usr/bin/env bash
set -euo pipefail

CC="${CC:-clang}"
CXX="${CXX:-clang++}"

if ! command -v "${CC}" >/dev/null 2>&1 || ! command -v "${CXX}" >/dev/null 2>&1; then
  echo "ERROR: ${CC} and ${CXX} are required for sanitizer builds."
  exit 1
fi

JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

echo "Configuring and building AddressSanitizer build..."
cmake -S . -B build_sanitizers_asan \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER="${CC}" \
  -DCMAKE_CXX_COMPILER="${CXX}" \
  -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF
cmake --build build_sanitizers_asan -j"${JOBS}"

echo "Running AddressSanitizer test suite..."
ctest --test-dir build_sanitizers_asan/tests --output-on-failure

echo "Configuring and building UndefinedBehaviorSanitizer build..."
cmake -S . -B build_sanitizers_ubsan \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER="${CC}" \
  -DCMAKE_CXX_COMPILER="${CXX}" \
  -DCMAKE_CXX_FLAGS="-fsanitize=undefined -fno-sanitize-recover=undefined" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=undefined" \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF
cmake --build build_sanitizers_ubsan -j"${JOBS}"

echo "Running UndefinedBehaviorSanitizer test suite..."
ctest --test-dir build_sanitizers_ubsan/tests --output-on-failure

