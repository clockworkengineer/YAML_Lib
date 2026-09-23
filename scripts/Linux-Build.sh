#!/usr/bin/env bash
set -euo pipefail

JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

cmake -S . -B Release -DCMAKE_BUILD_TYPE=Release -DBUILD_YAML_PARSER_FUZZ_TESTS=ON
cmake --build Release -j"${JOBS}"

cmake -S . -B Debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_YAML_PARSER_FUZZ_TESTS=ON
cmake --build Debug -j"${JOBS}"

