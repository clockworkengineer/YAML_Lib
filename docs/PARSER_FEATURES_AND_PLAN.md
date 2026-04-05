# YAML_Lib Parser Feature Analysis & Action Plan

## Supported YAML Features (from code & tests)
- Block string parsing (literal `|` and folded `>`)
- Block chomping indicators (`|`, `>`, `|+`, `|-`, `>+`, `>-`)
- Flow scalars (quoted, plain)
- Arrays (inline and block)
- Dictionaries (inline and block)
- Null, Boolean, Number scalars
- Multiple documents (`---`, `...`)
- Comments
- Merge key (`<<`) for dictionary overrides/extensions
- Anchors and aliases (basic support)
- **Tags:** `!!str`, `!!int`, `!!float`, `!!bool`, `!!null`, `!!seq`, `!!map`, custom `!tag`, verbatim `!<uri>`, named handles `!ns!tag` ✅ **(new)**
- **Directives:** `%YAML X.Y` version parsing/validation, `%TAG handle prefix` storage, unknown directives silently ignored ✅ **(new)**
- **Unicode/escape sequences:** Full YAML 1.2 escape set: `\0`, `\a`, `\v`, `\e`, `\/`, `\ `, `\N` (U+0085), `\_` (U+00A0), `\L` (U+2028), `\P` (U+2029), `\U` (8 hex digits for SMP codepoints) ✅ **(new)**
- **Timestamps:** `Timestamp` variant type, automatic detection & parsing of ISO 8601 dates/datetimes, `!!timestamp` tag ✅ **(new)**
- **Binary:** `!!binary` tag preserves base64 string value ✅ **(new)**
- **Error handling:** Undefined alias (via `*name`) throws `SyntaxError` with descriptive message ✅ **(new)**
- **Explicit `?` mapping keys:** `? key\n: value` block mapping form ✅ **(new)**
- **Plain scalar keys with special chars:** Keys starting with `:`, `-`, or `?` (without space) are valid ✅ **(new)**
- **YAML test-suite integration:** Curated test cases from yaml-test-suite in `YAML_Lib_Tests_Parse_YamlTestSuite.cpp` ✅ **(new)**

## Missing/Incomplete YAML Features
- **Complex Anchors/Aliases:** Nested, recursive, and merge key interactions
- **Tags:** Custom tag URI resolution, full type coercion for all core schema types
- **Directives:** Multiple `%TAG` handle lookups chained; `%YAML` on multi-doc streams
- **Flow collections:** Deeply nested flow collections, edge cases
- **Block collections:** Complex indentation, multiline keys, and values
- **Multi-document streams:** Edge cases, document boundary handling
- **Timestamp, binary, and other core types:** Parsing and serialization
- **Error handling:** More granular syntax error reporting, recovery
- **Spec compliance:** YAML 1.2.2 edge cases, especially ambiguous parsing scenarios
- **Custom source/destination:** Advanced I/O, streaming, and incremental parsing

## Concrete Action Plan
1. **Anchors & Aliases:**
   - ~~Expand support for nested and recursive anchors/aliases~~ — done ✅
   - ~~Implement merge key (`<<`) edge cases and spec-compliant behavior~~ — done ✅ (multi-merge `<<: [*a, *b]` with correct priority; `parseAlias` fixed for inline array/dict context)
   - ~~Undefined alias throws `SyntaxError`~~ — done ✅
2. **Tags & Directives:** ✅ **(implemented)**
   - ~~Add parsing for `%YAML` and `%TAG` directives~~ — done
   - ~~Support custom and standard tags, including type resolution~~ — done (`!!str`, `!!int`, `!!float`, `!!bool`, `!!null`, `!!seq`, `!!map`, `!custom`, `!<verbatim>`); now also supports coercion from quoted strings (e.g. `!!int "99"`, `!!bool "yes"`) ✅
   - ~~Named tag handle expansion (`!ns!tag`)~~ — done ✅ **(new)**: `!m!foo` with `%TAG !m! !my-` expands to `!my-foo`
   - ~~Unknown directives silently ignored~~ — done ✅ **(new)**
   - ~~Multiple named `%TAG` handles in one document stream~~ — done ✅ **(new)**: tested in Directives tests
   - ~~`%TAG` handle resets between documents~~ — done ✅ **(new)**
3. **Collections:** ✅ **(implemented)**
   - ~~Improve handling of deeply nested flow/block collections~~ — done ✅ **(new)**: 3-level block+flow, block sequence of flow dicts, compact block notation tests
   - ~~Add support for multiline keys/values and ambiguous cases~~ — done ✅ **(new)**: http://url:port keys, multi-line plain scalar folding, block string in mapping
4. **Core Types:** ✅ **(implemented)**
   - ~~Implement parsing for timestamps~~ — done (`Timestamp` variant, ISO 8601, `!!timestamp` tag)
   - ~~`!!binary` tag~~ — done (preserves raw base64 string)
   - ~~Enhance Unicode/escape sequence handling~~ — done (all YAML 1.2 escapes)
   - ~~Lowercase booleans (`true`/`false`/`yes`/`no`/`on`/`off`)~~ — done ✅
   - ~~YAML 1.2 octal prefix `0o`~~ — done ✅ (`0o17` → 15)
   - ~~Special floats (`.inf`, `+.inf`, `-.inf`, `.nan`)~~ — done ✅ (uses `std::numeric_limits<double>`)
5. **Error Handling:** ✅ **(implemented)**
   - ~~Undefined alias detection~~ — done ✅
   - ~~Tab character in block indentation throws `SyntaxError`~~ — done ✅
   - ~~Refine syntax error reporting for more precise diagnostics~~ — done ✅ **(new)**: SyntaxError message content tests (major version, undefined alias messages)
   - ~~Duplicate key detection (block and inline)~~ — done ✅ **(new tests)**
   - ~~Mapping indentation error detection~~ — done ✅ **(new tests)**
6. **Spec Compliance:**
   - ~~Review YAML 1.2.2 spec and add tests for edge cases~~ — done ✅ **(new)**: explicit `?` keys, plain scalar keys with `:`, `-`, `?` prefixes, multi-doc plain scalars stopping at `---`/`...`
   - ~~Ensure parser passes YAML test suite (add/expand `tests/yaml-test-suite`)~~ — done ✅ **(new)**: curated test cases (229Q, 2AUY, 4GC6, 5C5M, 5TYM, 6LVF, 26DV, 2EBW, 2XXW, 35KP, S4JQ, P94K, 3RLN, invalid 236B/6JTT/2CMS)
7. **Stringify Round-Trip:** ✅ **(new)**
   - ~~Add round-trip tests for scalar, timestamp, timestamp-in-dict, multi-doc, nested dict, sequence-of-dicts, mixed types~~ — done ✅ `YAML_Lib_Tests_Stringify_RoundTrip.cpp`
8. **Examples:** ✅ **(new)**
   - ~~`YAML_Error_Handling_Demo.cpp`~~ — done ✅: demonstrates SyntaxError catch, safe key/type guards, structure validation
   - ~~`YAML_Advanced_Types_Demo.cpp`~~ — done ✅: demonstrates timestamps, standard tags, named handles, verbatim tags, binary, anchors/merge, multi-doc, explicit keys
9. **I/O & Streaming:** ✅ **(implemented)**
   - ~~Add streaming support backed by `std::istream`/`std::ostream`~~ — done ✅ **(new)**: `StreamSource` (wraps any seekable `std::istream`) and `StreamDestination` (wraps any `std::ostream`) added to `classes/include/implementation/io/`
   - ~~Expand custom source/destination interfaces~~ — done ✅ **(new)**: `YAML_Sources.hpp` and `YAML_Destinations.hpp` now expose `StreamSource` / `StreamDestination` alongside `BufferSource`, `FileSource`, `BufferDestination`, `FileDestination`
   - ~~Tests~~ — done ✅ **(new)**: `YAML_Lib_Tests_ISource_Stream.cpp` and `YAML_Lib_Tests_IDestination_Stream.cpp` covering construction, `current()`, `next()`, `more()`, `position()`, `save()`/`restore()`, `match()`, parse round-trip, stringify round-trip, error states

---
This plan is based on current parser code, tests, and README. Update as features are implemented or new gaps are discovered.
