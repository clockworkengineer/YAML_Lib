#!/usr/bin/env bash
set -euo pipefail

cmake -S . -B Release -DCMAKE_BUILD_TYPE=Release -DBUILD_YAML_PARSER_FUZZ_TESTS=ON
cmake --build Release

cmake -S . -B Debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_YAML_PARSER_FUZZ_TESTS=ON
cmake --build Debug
