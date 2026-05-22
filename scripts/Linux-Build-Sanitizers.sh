#!/usr/bin/env bash
set -euo pipefail

if ! command -v clang >/dev/null 2>&1 || ! command -v clang++ >/dev/null 2>&1; then
  echo "ERROR: clang and clang++ are required for sanitizer builds."
  exit 1
fi

echo "Configuring and building AddressSanitizer build..."
cmake -S . -B build_sanitizers_asan \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF
cmake --build build_sanitizers_asan

echo "Running AddressSanitizer test suite..."
ctest --test-dir build_sanitizers_asan/tests --output-on-failure

echo "Configuring and building UndefinedBehaviorSanitizer build..."
cmake -S . -B build_sanitizers_ubsan \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_CXX_FLAGS="-fsanitize=undefined -fno-sanitize-recover=undefined" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=undefined" \
  -DBUILD_YAML_PARSER_FUZZ_TESTS=OFF
cmake --build build_sanitizers_ubsan

echo "Running UndefinedBehaviorSanitizer test suite..."
ctest --test-dir build_sanitizers_ubsan/tests --output-on-failure
