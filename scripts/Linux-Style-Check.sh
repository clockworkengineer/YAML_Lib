#!/usr/bin/env bash
set -euo pipefail

if ! command -v clang-format >/dev/null 2>&1; then
  echo "ERROR: clang-format is required but not installed."
  exit 1
fi

echo "Checking source formatting with clang-format..."
changed=0
files=$(git ls-files '*.cpp' '*.hpp' '*.h')
for file in $files; do
  if [[ -f "$file" ]]; then
    if ! clang-format --style=file "$file" | diff -u "$file" - >/dev/null; then
      echo "Formatting mismatch: $file"
      changed=1
    fi
  fi
done

if [ "$changed" -ne 0 ]; then
  echo "ERROR: Some files do not conform to .clang-format."
  exit 1
fi

echo "clang-format check passed."

if command -v clang-tidy >/dev/null 2>&1; then
  if [ ! -f build/compile_commands.json ]; then
    echo "Generating compile_commands.json for clang-tidy..."
    cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  fi
  echo "Running clang-tidy on library sources..."
  source_files=$(find classes/source -name '*.cpp' -print | tr '\n' ' ')
  if [ -n "$source_files" ]; then
    clang-tidy -p build $source_files
  else
    echo "No source files found for clang-tidy."
  fi
else
  echo "clang-tidy not installed; skipping static analysis."
fi
