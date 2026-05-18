# Contributing to YAML_Lib

Thank you for contributing! This project values clean code, clear reviews, and reliable builds.

## Branch and PR workflow

1. Fork the repository.
2. Create a descriptive branch:
   ```bash
git checkout -b feature/your-feature-name
```
3. Make focused commits. Keep each commit small and meaningful.
4. Push your branch and open a pull request against `main`.

## Commit message guidelines

- Use present-tense verbs: `Add`, `Fix`, `Update`, `Refactor`, ...
- Include a short summary and optional body if more detail is needed.
- Example:
  ```
  Refactor YAML_Impl into smaller implementation units
  ```

## Local checks before opening a PR

Run the repository's style and static analysis checks:

```bash
chmod +x ./scripts/Linux-Style-Check.sh
./scripts/Linux-Style-Check.sh
```

Build and test the library:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target YAML_Lib
cmake --build build --target YAML_Lib_Unit_Tests
cd build/tests
ctest --output-on-failure
```

## Code review checklist

- [ ] Code is well-factored and easy to understand
- [ ] Public API changes are documented in `README.md` or `docs/guide.md`
- [ ] New functionality includes tests
- [ ] Style checks pass with `.clang-format`
- [ ] Static analysis warnings are addressed
- [ ] `CHANGELOG` or release notes updated if appropriate

## Style and tooling

This repository uses `.clang-format` for C++ source formatting.
Run `clang-format --style=file` on modified `.cpp` and `.hpp` files before submitting.

Static analysis is run in CI via `clang-tidy` when available. The style check script will create or reuse `build/compile_commands.json`.

## PR description

Include the following in your PR description:

- What changed
- Why it changed
- How the change was tested
- Any build or compatibility considerations
