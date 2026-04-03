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
- **Tags:** `!!str`, `!!int`, `!!float`, `!!bool`, `!!null`, `!!seq`, `!!map`, custom `!tag`, verbatim `!<uri>` ✅ **(new)**
- **Directives:** `%YAML X.Y` version parsing/validation, `%TAG handle prefix` storage ✅ **(new)**
- **Unicode/escape sequences:** Full YAML 1.2 escape set: `\0`, `\a`, `\v`, `\e`, `\/`, `\ `, `\N` (U+0085), `\_` (U+00A0), `\L` (U+2028), `\P` (U+2029), `\U` (8 hex digits for SMP codepoints) ✅ **(new)**

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
   - Expand support for nested and recursive anchors/aliases
   - Implement merge key (`<<`) edge cases and spec-compliant behavior
2. **Tags & Directives:** ✅ **(implemented)**
   - ~~Add parsing for `%YAML` and `%TAG` directives~~ — done
   - ~~Support custom and standard tags, including type resolution~~ — done (`!!str`, `!!int`, `!!float`, `!!bool`, `!!null`, `!!seq`, `!!map`, `!custom`, `!<verbatim>`)
   - Remaining: multi-handle `%TAG` expansion chaining, core-schema binary/timestamp tags
3. **Collections:**
   - Improve handling of deeply nested flow/block collections
   - Add support for multiline keys/values and ambiguous cases
4. **Core Types:**
   - Implement parsing for timestamps, binary, and other YAML types
   - ~~Enhance Unicode/escape sequence handling~~ — done (all YAML 1.2 escapes)
5. **Error Handling:**
   - Refine syntax error reporting for more precise diagnostics
   - Add recovery strategies for common YAML mistakes
6. **Spec Compliance:**
   - Review YAML 1.2.2 spec and add tests for edge cases
   - Ensure parser passes YAML test suite (add/expand `tests/yaml-test-suite`)
7. **I/O & Streaming:**
   - Add incremental parsing and streaming support
   - Expand custom source/destination interfaces

---
This plan is based on current parser code, tests, and README. Update as features are implemented or new gaps are discovered.
