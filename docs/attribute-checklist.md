# YAML_Lib Attribute Checklist

This document summarizes the library attributes audited and implemented as part of the Phase 7 release review.

## 1. Baseline Public API Audit

- Public header install set is defined in `CMakeLists.txt`.
- Primary public headers:
  - `classes/include/YAML.hpp`
  - `classes/include/YAML_Core.hpp`
  - `classes/include/YAML_Config.hpp`
  - `classes/include/YAML_Interfaces.hpp`
  - `classes/include/YAML_Stringify.hpp`
- Optional public headers exposed by specific build options:
  - `classes/include/implementation/common/YAML_SAX.hpp`
  - `classes/include/implementation/common/YAML_Config.hpp`

## 2. API and Header Cleanup

- Documented public API in `docs/public_api.md`.
- Ensured install list matches documented public headers.
- Added tests in `tests/source/misc/YAML_Lib_Tests_PublicAPI.cpp`.

## 3. Documentation and Guidance

- Updated `docs/testing.md` with secure build and test guidance.
- Added a focused attribute checklist to support release review.
- Aligned examples and header semantics with current API behavior.

## 4. Security and Reliability Hardening

- Parser security limits are enforced in `classes/source/implementation/parser/YAML_Parser.cpp`.
- New parser security tests are added in `tests/source/parse/YAML_Lib_Tests_Parse_SecurityLimits.cpp`.
- Coverage includes:
  - `Options::max_documents`
  - `Options::max_parse_depth`
  - `Options::max_alias_expansions`
  - recursive alias loop detection

## 5. Performance Validation

- Existing `examples/YAML_Performance_Profile` supports performance validation for release.
- Build validation includes:
  - default library configuration
  - `-DYAML_LIB_FILE_IO=OFF`
  - `-DYAML_LIB_NO_EXCEPTIONS=ON`

## 6. Release Review

- Confirmed tests and docs are aligned with the refactor plan.
- This checklist serves as a final review artifact for Phase 7.
