#!/usr/bin/env bash
set -euo pipefail

run_tests() {
  local dir="$1"
  if [ -d "${dir}/tests" ]; then
    echo "Running tests in ${dir}..."
    ctest --test-dir "${dir}/tests" --output-on-failure
  elif [ -d "${dir}" ]; then
    echo "Running tests in ${dir}..."
    ctest --test-dir "${dir}" --output-on-failure
  fi
}

found=0
for build_dir in Release Debug build build_asan build_ubsan; do
  if [ -d "${build_dir}" ]; then
    run_tests "${build_dir}"
    found=1
  fi
done

if [ "$found" -eq 0 ]; then
  echo "No build directory found (looked for Release, Debug, build). Build first."
  exit 1
fi

