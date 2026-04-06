# YAML 1.2 Compliance Analysis & Action Plan

**Date:** April 5, 2026  
**Library:** YAML_Lib (C++20)  
**Spec reference:** YAML 1.2.2 — https://yaml.org/spec/1.2.2/

---

## 1. Executive Summary

YAML_Lib implements the bulk of the YAML 1.2 specification and covers all major features needed for everyday use: block and flow collections, block scalars, anchors/aliases, merge keys, tags, directives, timestamps, unicode escape sequences, and multi-document streams. It passes 15+ hand-curated yaml-test-suite cases.

However, several compliance gaps remain — ranging from spec-breaking bugs to missing optional features. This document classifies each gap, assigns a priority, and lists concrete implementation tasks.

**Estimated overall YAML 1.2 compliance: ~75–80%.**  
The remaining gaps are well-defined and actionable.

---

## 2. What Is Already Compliant

| Area | Status | Notes |
|------|--------|-------|
| Core scalar types: `true`/`false` booleans | ✅ | Canonical 1.2 forms recognised |
| Core scalar types: `null` / `~` null | ✅ | Lowercase only (correct per 1.2) |
| Integers: decimal, hex `0x`, octal `0o` | ✅ | `0o` prefix normalised before parsing |
| Special floats: `.inf`, `+.inf`, `-.inf`, `.nan` | ✅ | Case-insensitive match |
| Block scalars `\|` and `>` | ✅ | All three chomping modes (clip/strip/keep) |
| Flow scalars: double-quoted, single-quoted, plain | ✅ | Escape translation via translator |
| Full YAML 1.2 escape set in double-quoted strings | ✅ | `\0 \a \v \e \  \/ \N \_ \L \P \Uxxxxxxxx` |
| Block sequences and mappings | ✅ | Indent-aware, nested |
| Flow sequences and mappings | ✅ | Includes trailing commas, deeply nested |
| Anchors & aliases (`&anchor` / `*alias`) | ✅ | Nested, in-sequence, in-mapping |
| Merge key `<<` | ✅ | Single and multi-merge; priority-correct |
| Tags: `!!str !!int !!float !!bool !!null !!seq !!map !!timestamp !!binary` | ✅ | With type coercion |
| Custom `!tag` and verbatim `!<uri>` tags | ✅ | Stored on node |
| Named tag handles via `%TAG` | ✅ | Multi-handle, reset between docs |
| `%YAML X.Y` directive: version check | ✅ | Rejects non-1.x major versions |
| Unknown directives silently ignored | ✅ | Per spec warning-only rule |
| Multi-document streams (`---` / `...`) | ✅ | Consecutive starts OK |
| Explicit `?` mapping keys | ✅ | `? key\n: value` block form |
| Plain scalar keys beginning with `:` `-` `?` | ✅ | No following space = plain scalar |
| Tab as block indentation: throws `SyntaxError` | ✅ | Per spec §6.1 |
| Undefined alias throws `SyntaxError` | ✅ | Descriptive message |
| Duplicate key detection | ✅ | Block and inline |
| Timestamps (ISO 8601 date/datetime) | ✅ | `Timestamp` node type |
| `!!binary` base64 preservation | ✅ | Raw value stored |
| Streaming I/O: `StreamSource` / `StreamDestination` | ✅ | Wraps any `std::istream`/`std::ostream` |
| `null` as dictionary key | ✅ | Maps to empty-string key |
| yaml-test-suite integration (curated subset) | ✅ | ~16 cases (229Q…3RLN, invalid 236B/6JTT/2CMS) |

---

## 3. Compliance Gaps

Gaps are classified by severity:

- **🔴 HIGH** — Spec-breaking; incorrect results or rejected valid YAML
- **🟡 MEDIUM** — Significant missing feature or lossy round-trip
- **🟢 LOW** — Edge-case or cosmetic non-compliance

---

### 3.1 ~~🔴 HIGH — C-style octal literals treated as octal (spec break)~~ ✅ FIXED (2026-04-06)

**Location:** `classes/include/implementation/variants/YAML_Number.hpp`, `stringToNumber()`, line ~88  
**Problem:**  
```cpp
} else if (number.starts_with("0")) {
    integerConversionBase = 8;   // ← treated "0123" as octal (YAML 1.1 behaviour)
}
```
YAML 1.2 core schema defines octal **only** as `0o[0-7]+`. The legacy form `0NNN` is not valid in YAML 1.2 and should be parsed as a plain decimal integer (or as a string if it contains non-digit characters).  
**Fix applied:**  
- Removed the `starts_with("0")` base-8 branch from `Number::stringToNumber` in `YAML_Number.hpp`.  
- Changed `YAML_Parser_Scalar.cpp`: instead of normalising `0o17` to `017` (C-style octal), the octal digits are now parsed directly with `std::stoll(..., 8)` and the decimal equivalent string (e.g. `"15"`) is passed to `Number{}` for type-fitting.  
- Updated `YAML_Lib_Tests_Parse_Numeric.cpp`: old `04444`-as-octal test replaced with a YAML 1.2 `0o4444` test (value 2340) and a new `04444`-as-decimal test (value 4444).  
**Verification:** All 2258 test assertions pass.

---

### 3.2 ~~🔴 HIGH — Block scalar explicit indentation indicator not parsed~~ ✅ FIXED (2026-04-06)

**Location:** `classes/source/implementation/parser/YAML_Parser_BlockString.cpp`, `parseBlockChomping()`  
**Fix applied:**  
- `parseBlockChomping` signature changed to `std::pair<BlockChomping, int>` — returns both the chomping mode and the explicit indent `m` (0 = auto-detect).  
- Parses both orderings allowed by YAML 1.2 §8.1.1: digit-then-indicator (`|2-`, `>3+`) and indicator-then-digit (`|-2`).  
- `parseBlockString`: if `explicitIndent > 0`, uses `indentation + explicitIndent` as `blockIndent`; otherwise auto-detects from first content line (unchanged behaviour).  
- Also fixed a latent crash: `yamlString.back()` in the more-indented branch was called on an empty string (can happen when the very first content line is more-indented than `blockIndent`). Added `!yamlString.empty()` guard.  
- `Default_Parser.hpp` updated to reflect new `parseBlockChomping` return type.  
- `YAML_Lib_Tests_Parse_Collections.cpp`: 5 new `[ExplicitIndent]` sections.  
**Verification:** All 2290 test assertions pass.

---

### 3.3 ~~🔴 HIGH — Special float stringify is platform-non-compliant~~ ✅ FIXED (2026-04-06)

**Location:** `classes/include/implementation/variants/YAML_Number.hpp`, `numberToString()`  
**Fix applied:**  
Added an early-return guard at the top of the `numberToString<T>` template body: when `T` is a floating-point type, `std::isinf` and `std::isnan` are tested before the `std::ostringstream` path. Results:  
- `+inf` → `.inf`  
- `-inf` → `-.inf`  
- NaN → `.nan`  
Applies to `float`, `double`, and `long double` (all covered by `if constexpr (std::is_floating_point_v<T>)`).  
`YAML_Lib_Tests_Stringify_RoundTrip.cpp`: 7 new `[SpecialFloat]` sections — 3 stringify-only tests (`.inf`, `-.inf`, `.nan`) and 3 full round-trip tests plus negative-infinity round-trip.  
**Verification:** All 2319 test assertions pass.

---

### 3.4 ✅ FIXED — YAML 1.1 boolean forms accepted unconditionally

**Location:** `classes/include/implementation/variants/YAML_Boolean.hpp`, `isTrue`/`isFalse` sets  
**Problem:**  
YAML 1.2 core schema defines exactly two boolean literals: `true` and `false` (lowercase only). The library additionally recognises `yes`, `no`, `on`, `off`, `Yes`, `No`, `On`, `Off`, `True`, `False` (YAML 1.1 forms). There is no mechanism to suppress these based on the `%YAML` directive version.  
**Impact:** Strings that are meant to be plain scalars (e.g. config keys `on`, `off`, `yes`, `no`) silently become boolean nodes in YAML 1.2 documents. This is one of the most common real-world YAML bugs and has broken many projects.  
**Fix (option A — strict mode flag):** Add a `bool strictBooleans` option (default `false` for backward compat). When `true`, or when `%YAML 1.2` is declared, only `true`/`false` are accepted.  
**Fix (option B — version-driven):** When `yamlDirectiveMinor >= 2`, strip YAML 1.1 forms from `isTrue`/`isFalse` during parsing. Reset on each new document stream.  
Option B is more spec-correct; option A is more practical and non-breaking.  
**Resolution:** Implemented option A. `Default_Parser::strictBooleans` flag (default `false`) added. `YAML::setStrictBooleans(bool)` exposed on public API. `isBoolean` and `parseBoolean` both check the flag; when set, only `"true"`/`"false"` are recognised. Existing YAML 1.1-style tests continue to pass; 12 new strict-mode test sections added.

---

### 3.5 🟡 MEDIUM — `#` starts a comment even without preceding whitespace in plain scalars

**Location:** `classes/source/implementation/parser/YAML_Parser.cpp`, `parse()` — delimiter set `{kLineFeed, '#'}`; propagated to `parsePlainFlowString` via `extractToNext`  
**Problem:**  
YAML 1.2 spec §6.8 states that `#` introduces a comment **only when preceded by a whitespace character**. The parser passes `'#'` as a hard delimiter which causes `foo#bar` to be split at the `#` sign, yielding `foo` as the value and silently discarding `bar # rest`. Plain scalars like `C#`, `foo#1`, `http://host#anchor` are incorrectly truncated.  
**Impact:** Real-world values containing unquoted `#` are silently truncated.  
**Fix:** Change `extractToNext` (or introduce a new variant `extractToNextComment`) that only stops at `#` when the character immediately preceding the current position is a space or tab. Alternatively, process the extracted plain scalar text afterward and strip only a `#` that is preceded by whitespace.

---

### 3.6 🟡 MEDIUM — Stringify does not preserve or emit tags

**Location:** `classes/include/implementation/stringify/Default_Stringify.hpp`  
**Problem:**  
The `Variant` base class stores a tag string (`getTag()`/`setTag()`), but `Default_Stringify` never emits tags. A document parsed with:
```yaml
!!str 42
```
stringifies back as `42` — without the `!!str` tag — so the round-trip semantics are lost.  
Also: comments, folded-block-string markers (`>`), timestamps, and custom tags are all silently dropped or altered during stringify.  
**Impact:** Lossless round-trip is not achievable for tagged nodes.  
**Fix:** Before stringifying a node's value, check `node.getVariant().getTag()` and emit the tag prefix if set. Handle special cases for `Timestamp` (tag optional as the type is self-describing) and `!!binary`.

---

### 3.7 🟡 MEDIUM — Block scalar folded (`>`) stringified as literal (`|`)

**Location:** `Default_Stringify.hpp`, `stringifyAnyBlockStyle()`  
**Problem:**  
```cpp
static void stringifyAnyBlockStyle(IDestination &destination, const Node &yNode) {
  if (isA<String>(yNode)) {
    if (const auto quote = NRef<String>(yNode).getQuote(); quote == '>' || quote == '|') {
      destination.add("|");   // ← always emits '|' even when quote == '>'
```
The `>` (folded) block style is always output as `|` (literal). This change in semantics alters the round-trip representation.  
**Impact:** Folded block strings become literal block strings after one stringify cycle.  
**Fix:** Emit `destination.add(std::string(1, quote));` (use the actual quote character) instead of the hard-coded `"|"`.

---

### 3.8 🟡 MEDIUM — yaml-test-suite coverage is too sparse (~16 of ~280 cases)

**Location:** `tests/source/parse/YAML_Lib_Tests_Parse_YamlTestSuite.cpp`  
**Problem:**  
The yaml-test-suite ships with the repository (`tests/yaml-test-suite/yaml-test-suite/src/`, ~280 `.yaml` files) but only ~16 cases are exercised in the integration test. Many important spec-coverage areas are not tested at all (flow indicator rules, complex block structures, character set validation, spec examples from every chapter).  
**Impact:** Unknown failures in untested cases; no regression guard for new parser changes.  
**Fix:** Expand the test file to run all (or at least all *valid*) test suite cases programmatically, reading expected output from the `.yaml` files' `out:` field and comparing against the library's parse result.

---

### 3.9 🟡 MEDIUM — `%YAML` version directive has no effect on parsing rules

**Location:** `Default_Parser.hpp`, `yamlDirectiveMinor`; `YAML_Parser.cpp`  
**Problem:**  
`yamlDirectiveMinor` is stored correctly (2 for `%YAML 1.2`, 1 for `%YAML 1.1`) but is never consulted during parsing to switch schema rules. A `%YAML 1.2` document is parsed with exactly the same rules as a `%YAML 1.1` document.  
**Impact:**  
- YAML 1.2 files may have YAML 1.1 booleans silently coerced (see §3.4).  
- `%YAML 1.1` files with C-style octal (`0777`) should be treated as octal; after the §3.1 fix they would incorrectly become decimal.  
**Fix:** Consult `yamlDirectiveMinor` in `parseBoolean` (restrict to `true`/`false` when `>= 2`) and `stringToNumber` (allow base-8 `0NNN` only when `== 1`).

---

### 3.10 🟢 LOW — Recursive anchor expansion has no cycle guard

**Location:** `classes/source/implementation/parser/YAML_Parser_Dictionary.cpp` and array parsing  
**Problem:**  
If YAML contains a self-referencing anchor (directly or indirectly), alias expansion will recurse infinitely and crash with a stack overflow. YAML 1.2 explicitly prohibits recursive structures but the parser does not detect the cycle.  
**Fix:** Maintain a `std::set<std::string> activeAnchors` during alias expansion and throw `SyntaxError` if an alias resolves to an anchor currently being expanded.

---

### 3.11 🟢 LOW — `%YAML` directive may appear multiple times without error

**Location:** `YAML_Parser.cpp`, directive handling  
**Problem:**  
YAML 1.2 spec (§9.2) says at most one `%YAML` directive may appear per document. The parser overwrites `yamlDirectiveMinor` silently on each `%YAML` seen.  
**Fix:** Track whether a `%YAML` directive has been seen for the current document stream and throw `SyntaxError` on a second occurrence.

---

### 3.12 ~~🟢 LOW — Flow scalar line folding does not strip trailing whitespace before newline~~ ✅ FIXED (2026-04-06)

**Location:** `YAML_Parser_FlowString.cpp`, `appendCharacterToString()`, `parsePlainFlowString()`  
**Problem:**  
YAML spec §7.3.1 / §6.5 says trailing whitespace before a line break in a flow scalar must be stripped. The `appendCharacterToString` helper replaced `\n` with a space but did not first trim trailing spaces/tabs from the already-accumulated buffer. Additionally `parsePlainFlowString` extracted the first line including trailing whitespace before adding the fold-space.  
**Fix applied:**  
- `appendCharacterToString`: before appending either the fold-space (non-empty continuation line) or the line-break character (empty-line path), all trailing `space` and `tab` characters are stripped from `yamlString` with a `while`-pop loop.  
- `parsePlainFlowString`: changed the first-line extraction from `extractToNext(...) + kSpace` to `extractToNext(...)` → `rightTrim(...)` → `+= kSpace`, so trailing whitespace on the first line is stripped before the fold-space is added.  
- `YAML_Lib_Tests_Parse_String.cpp`: added four new `[LineFold]` sections covering double-quoted, single-quoted, plain scalars with trailing spaces, and double-quoted with trailing tab.  
**Verification:** All 2270 test assertions pass.

---

### 3.13 🟢 LOW — No validation of disallowed control characters in YAML content

**Location:** Parser generally  
**Problem:**  
YAML 1.2 spec §5.1 defines the character set for YAML streams. Control characters (U+0000-U+0008, U+000B, U+000C, U+000E-U+001F) are not permitted outside of escaped contexts. The parser does not validate raw input bytes.  
**Fix:** Add a character-set validation pass, or validate as part of `extractToNext` / `extractString`. Throw `SyntaxError` on out-of-spec control characters.

---

### 3.14 🟢 LOW — `!!omap` and `!!pairs` have no dedicated node type

**Location:** `classes/include/implementation/variants/`  
**Problem:**  
YAML 1.2 schema includes `!!omap` (ordered map) and `!!pairs` (sequence of key-value pairs allowing duplicate keys). Both are parsed as generic dictionaries/sequences. Ordered insertion is already preserved by the `Dictionary` type (it uses a vector, not a hash map), so `!!omap` round-trips correctly in practice, but there is no node-level distinction.  
**Fix (minimal):** Store the `!!omap`/`!!pairs` tag on the resulting node (already possible via `setTag`) and document the limitation. This is tracked in §3.6 (tag preservation).  
**Fix (full):** Add `OrderedMap` and `Pairs` variant types for semantic distinction.  

---

### 3.15 🟢 LOW — Single-quoted scalars: `''` escape only handled at outer parse, not inside key extraction

**Location:** `YAML_Parser_Util.cpp`, `extractString()`  
**Problem:**  
`extractString` scans until it finds the closing quote without handling the `''` (escaped apostrophe) inside single-quoted strings. Single-quoted key extraction (`extractKey`) uses `extractString`, so a single-quoted key with an embedded `''` is likely mis-parsed.  
**Fix:** In `extractString`, when the closing-quote character is found, peek one position ahead; if the next char is also a quote, consume both and continue rather than stopping.

---

## 4. Concrete Action Plan

Items are ordered by impact. Each item lists the affected files and the acceptance criterion.

---

### P1 — ~~Fix C-style octal (BREAKING BUG)~~ ✅ DONE (2026-04-06)
**Files:** `classes/include/implementation/variants/YAML_Number.hpp`, `classes/source/implementation/parser/YAML_Parser_Scalar.cpp`, `tests/source/parse/YAML_Lib_Tests_Parse_Numeric.cpp`  
**Removed** `starts_with("0")` → base-8 branch from `Number::stringToNumber`. Changed `0o` normalisation to compute the decimal value directly. Updated tests.  
All 2258 assertions pass.

---

### ~~P2 — Fix block scalar explicit indentation indicator~~ ✅ DONE (2026-04-06)
**Changes applied:**
- `parseBlockChomping` returns `std::pair<BlockChomping, int>` — chomping + explicit indent m (0 = auto).
- Parses both orderings: digit-then-indicator (`|2-`) and indicator-then-digit (`|-2`).
- `parseBlockString`: `blockIndent = indentation + explicitIndent` when m > 0, else auto-detect.
- Fixed latent crash: `!yamlString.empty()` guard before `yamlString.back()` in more-indented branch.
- `Default_Parser.hpp` updated.
- `YAML_Lib_Tests_Parse_Collections.cpp`: 5 new `[ExplicitIndent]` tests.
**Verification:** All 2290 assertions pass.

---

### ~~P3 — Fix special-float stringify~~ ✅ DONE (2026-04-06)
**Changes applied:**
- `YAML_Number.hpp` (`numberToString<T>`): added `if constexpr (std::is_floating_point_v<T>) { isinf → ".inf"/"-.inf"; isnan → ".nan"; }` before `ostringstream` path.
- `YAML_Lib_Tests_Stringify_RoundTrip.cpp`: 7 new `[SpecialFloat]` tests.
**Verification:** All 2319 assertions pass.

---

### P4 — Fix folded block string stringifies as literal
**Files:** `classes/include/implementation/stringify/Default_Stringify.hpp`, `stringifyAnyBlockStyle()`  
**Task:** Change:
```cpp
destination.add("|");
```
to:
```cpp
destination.add(std::string(1, quote));
```
**Test:** Parse `key: >\n  line\n`, stringify, verify output starts with `key: >`.  
**Acceptance:** Round-trip preserves `>` vs `|` distinction.

---

### P5 — Fix plain scalar `#` comment detection
**Files:** `classes/source/implementation/parser/YAML_Parser_FlowString.cpp`, `YAML_Parser_Util.cpp`  
**Task:**  
Model A (simpler): In `parsePlainFlowString`, after extracting the first-line portion, post-process the accumulated scalar string. Find `" #"` (space-hash) as a comment boundary and truncate there; leave bare `#` inside the word intact.  
Model B (precise): Split `extractToNext` into `extractToNextComment` that stops at `#` only when the immediately preceding character was a space/tab.  
**Test:** `foo#bar: value` — key is `"foo#bar"`; `foo #comment` — value is `"foo"`.  
**Acceptance:** yaml-test-suite cases that involve inline comments on plain scalars pass.

---

### P6 ✅ DONE — Strict YAML 1.2 boolean option
**Files:** `classes/include/implementation/variants/YAML_Boolean.hpp`, `classes/source/implementation/parser/YAML_Parser_Scalar.cpp`, `classes/include/implementation/parser/Default_Parser.hpp`  
**Task:**  
1. Add a `static bool strictBooleans` flag to `Default_Parser` (default `false`).  
2. Expose it via `YAML::setStrictBooleans(bool)` on the public API.  
3. In `parseBoolean`, when `strictBooleans || yamlDirectiveMinor >= 2`, use a reduced set containing only `"true"` and `"false"` (both lowercase).  
4. Document that the default is backward-compatible (permissive).  
**Test:** Document with `%YAML 1.2` and value `yes` is parsed as a plain string, not a boolean. Without `%YAML` directive the same document parses `yes` as a boolean.  
**Acceptance:** New tests pass without breaking existing tests.  
**Resolution:** `strictBooleans` flag implemented; 2367 assertions pass (53 test cases).

---

### P7 — Version-directive-sensitive schema
**Files:** `YAML_Parser.cpp`, `YAML_Parser_Scalar.cpp`, `YAML_Number.hpp`  
**Task:** After P1, P6 are in place: consult `yamlDirectiveMinor` to decide schema rules consistently:
- `>= 2` (1.2): strict booleans only; no C-style octal.
- `== 1` (1.1): YAML 1.1 booleans allowed; `0NNN` is octal.
- Unset (no `%YAML`): default to 1.2 behaviour (as per spec §9.1.4).  
**Test:** `%YAML 1.1` document with `0777` parses as 511 (octal); same file without directive parses as 777 (decimal).  
**Acceptance:** Tests pass; yamlDirectiveMinor is consulted in all affected parsers.

---

### P8 — Recursive anchor cycle guard
**Files:** `classes/source/implementation/parser/YAML_Parser_Array.cpp` or wherever alias expansion resolves  
**Task:** Maintain a `thread_local std::set<std::string> activeAnchors`. When beginning to expand an alias, insert the anchor name; remove it on return. If already present, throw `SyntaxError("Recursive anchor detected: <name>")`.  
**Test:** A YAML fragment that would infinitely recurse instead throws a `SyntaxError`.  
**Acceptance:** No stack overflow; descriptive error message.

---

### P9 — Fix single-quoted key `''` escape in `extractString`
**Files:** `classes/source/implementation/parser/YAML_Parser_Util.cpp`, `extractString()`  
**Task:** When scanning the quoted content and the closing quote is found, save the position, advance one character, and check if it is also the same quote. If yes: emit a single quote and continue; if no: restore and stop.  
**Test:** `'can''t': value` parses with key `"can't"`.  
**Acceptance:** New test passes.

---

### ~~P10 — Fix flow scalar line folding (strip trailing whitespace before newline)~~ ✅ DONE (2026-04-06)
**Files:** `classes/source/implementation/parser/YAML_Parser_FlowString.cpp`  
**Changes applied:**
- `appendCharacterToString()`: trim trailing space/tab from `yamlString` before appending fold-space or empty-line newline.
- `parsePlainFlowString()`: `rightTrim` the first-line `extractToNext` result before `+= kSpace` in multi-line branch.
- `YAML_Lib_Tests_Parse_String.cpp`: 4 new `[LineFold]` tests.
**Verification:** All 2270 test assertions pass.

---

### P11 — Tag preservation in stringify
**Files:** `classes/include/implementation/stringify/Default_Stringify.hpp`, all `stringify*` methods  
**Task:**  
1. After P4 (folded/literal fix): in `stringifyNodes`, check `yNode.getVariant().getTag()` for non-empty tags and emit them before the value (`"<tag> "`).  
2. Emit `!!timestamp` before `Timestamp` nodes.  
3. Document that `!!binary` values are output as-is (raw base64 string).  
**Test:** A node created with `!!str 42` stringifies as `!!str 42`; round-trip preserves the string type.  
**Acceptance:** Tagged-node round-trip tests pass.

---

### P12 — Validate disallowed control characters
**Files:** `classes/source/implementation/parser/YAML_Parser_Util.cpp`  
**Task:** In `extractToNext` and `extractString`, after consuming a character, check its value against the YAML 1.2 forbidden character set (U+0000–U+0008, U+000B, U+000C, U+000E–U+001F excluding TAB U+0009, LF U+000A). Throw `SyntaxError` on violation.  
**Test:** A buffer containing a raw `\x01` byte outside a quoted string throws `SyntaxError`.  
**Acceptance:** New test passes; no valid YAML regresses.

---

### P13 — Expand yaml-test-suite coverage
**Files:** `tests/source/parse/YAML_Lib_Tests_Parse_YamlTestSuite.cpp`  
**Task:**  
1. Write a helper that reads a yaml-test-suite `.yaml` file, extracts the `yaml:` block (document under test) and determines `fail: true` vs expected-output.  
2. Run all ~280 suite files, skipping those that require features beyond the scope of the current library (label with comments).  
3. Record failing cases in a `KNOWN_FAILURES.md` rather than silently skipping.  
**Target pass rate:** ≥ 200/280 cases after all P1–P12 fixes.  
**Acceptance:** CI does not regress; new failures are caught before merge.

---

### P14 — `%YAML` duplicate directive detection
**Files:** `YAML_Parser.cpp`  
**Task:** Add a `bool seenYamlDirective = false` flag per-stream. Set it on first `%YAML`; throw `SyntaxError` if seen again before the next `---`.  
**Test:** Two `%YAML 1.2` lines before a single `---` throw.  
**Acceptance:** New test passes.

---

## 5. Priority Order Summary

| Priority | ID | Area | Severity | Effort |
|----------|----|------|----------|--------|
| 1 | P1 | C-style octal bug | ✅ DONE | — |
| 2 | ~~P3~~ | ~~Special float stringify~~ | ✅ DONE | — |
| 3 | P4 | Folded block stringify | 🔴 HIGH | Very low (1-char change) |
| 4 | ~~P2~~ | ~~Block scalar explicit indent~~ | ✅ DONE | — |
| 5 | P5 | Plain scalar `#` comment rule | 🟡 MEDIUM | Medium |
| 6 | P9 | Single-quoted `''` in `extractString` | 🟡 MEDIUM | Low |
| 7 | ~~P10~~ | ~~Flow scalar trailing-space fold~~ | ✅ DONE | — |
| 8 | P6 | Strict boolean mode | 🟡 MEDIUM | Medium |
| 9 | P7 | Version-directive schema switch | 🟡 MEDIUM | Medium (depends on P1, P6) |
| 10 | P11 | Tag preservation in stringify | 🟡 MEDIUM | Medium–High |
| 11 | P8 | Recursive anchor guard | 🟢 LOW | Low |
| 12 | P14 | Duplicate `%YAML` detection | 🟢 LOW | Very low |
| 13 | P12 | Control character validation | 🟢 LOW | Low–Medium |
| 14 | P13 | yaml-test-suite expansion | 🟡 MEDIUM | High |

---

## 6. Out of Scope (Intentional Limitations)

The following are either outside the library's design scope or deliberately deferred:

- **`!!omap` / `!!pairs` distinct node types** — The existing `Dictionary` (insertion-ordered vector) already preserves order. Dedicated types add API complexity for minimal gain. Track tags via P11 to distinguish semantically.
- **YAML streaming / incremental parse** — The library requires full-document buffering. Incremental push-parsing is a significant architectural change.
- **SAX-style events** — Not part of the library's visitor/traversal model.
- **YAML 1.1 full compatibility mode** — P7 covers the most impactful differences; full YAML 1.1 re-implementation is not planned.

---

*This document supersedes the ad-hoc tracking in `PARSER_FEATURES_AND_PLAN.md` for compliance-specific items. Update this file as each P-item is completed.*
