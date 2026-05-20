# Phase 1 Audit: API Stabilization

## Findings

- `YAML.hpp` exposes a public API surface that is mostly stable, but some return-value methods should be enforced with `[[nodiscard]]` to prevent silent misuse.
- `YAML::fromFile()` is a raw file reader returning a string, while `YAML::loadFile()` is the structured parse helper. This naming is subtle and should be documented clearly.
- `YAML::fromFileToYAML()` is a legacy file-to-YAML parser wrapper. It is still available, but it should be marked as deprecated in favor of `YAML::loadFile()`.
- `Options::secureOptions()` provides an important security configuration, and its result should not be discarded.
- The public API documentation in `docs/public_api.md` should explicitly state the difference between file helpers.

## Actions Completed

- Added `[[nodiscard]]` to:
  - `Options::secureOptions()`
  - `YAML::fromString()`
  - `YAML::fromFileToYAML()`
  - `YAML::load()`
  - `YAML::loadFile()`
  - `YAML::fromFile()`
  - `YAML::toString()`
  - `YAML::dump()`
  - `YAML::tryParse()`
  - `YAML::tryStringify()`
- Marked `YAML::fromFileToYAML()` as deprecated with a guidance message.
- Updated `docs/public_api.md` to clarify that `YAML::fromFile()` returns raw file contents, while `YAML::loadFile()` parses YAML into a `YAML` object.

## Remaining Phase 1 Opportunities

- Review `YAML_Core.hpp` installation public surface and ensure only intended headers are exported.
- Confirm `docs/api.md` method documentation is aligned with `[[nodiscard]]` and file helper semantics.
- Inspect and, if needed, simplify `YAML.hpp` aliases to make the public API more intuitive.

## Phase 1 completion

- Verified `YAML_Core.hpp` is an advanced public aggregation header and its install surface matches the intended documented public API.
- Updated `docs/api.md` to reflect the actual `YAML` class methods, constructors, and `Options` public API.
- Added a file-helper semantics note clarifying `fromFile()` versus `loadFile()`.
