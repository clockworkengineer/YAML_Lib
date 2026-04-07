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

### 3.5 ✅ FIXED — `#` starts a comment even without preceding whitespace in plain scalars

**Location:** `classes/source/implementation/parser/YAML_Parser.cpp`, `parse()` — delimiter set `{kLineFeed, '#'}`; propagated to `parsePlainFlowString` via `extractToNext`  
**Problem:**  
YAML 1.2 spec §6.8 states that `#` introduces a comment **only when preceded by a whitespace character**. The parser passes `'#'` as a hard delimiter which causes `foo#bar` to be split at the `#` sign, yielding `foo` as the value and silently discarding `bar # rest`. Plain scalars like `C#`, `foo#1`, `http://host#anchor` are incorrectly truncated.  
**Impact:** Real-world values containing unquoted `#` are silently truncated.  
**Fix:** Change `extractToNext` (or introduce a new variant `extractToNextComment`) that only stops at `#` when the character immediately preceding the current position is a space or tab. Alternatively, process the extracted plain scalar text afterward and strip only a `#` that is preceded by whitespace.  
**Resolution:** In `parsePlainFlowString`, after the initial `extractToNext` call, a while-loop extension consumes any `#` that is NOT preceded by whitespace as a literal character and continues extraction until the next real delimiter. Handles both first-line and multi-line continuation cases. 8 new `[InlineComment]` test sections cover `C#`, `foo#bar`, URL fragments, mixed literal+comment cases, and keys containing `#`. All 2394 assertions pass.

---

### 3.6 ✅ FIXED — Stringify does not preserve or emit tags

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
**Resolution:** Added `tagToEmitForm()` helper that converts stored full URIs (`tag:yaml.org,2002:str`) back to short YAML form (`!!str`), or emits local/verbatim tags as-is. `stringifyNodes()` now emits the tag before any non-block scalar. `stringifyAnyBlockStyle()` emits the tag before the block marker (`|`/`>`) for block strings. Collection (Array/Dictionary) tag emission deferred to gap 3.7/P11 full work. 10 new `[Stringify][Tags]` test sections (round-trip for `!!str`, `!!int`, `!!bool`, `!!null`, `!!float`, `!custom`, dict value, array element, full round-trip, `!!timestamp`). All 2429 assertions pass.

---

### 3.7 ✅ FIXED — Block scalar folded (`>`) stringified as literal (`|`)

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
**Resolution:** Changed `destination.add("|")` to `destination.add(std::string(1, quote))` in `stringifyAnyBlockStyle()`. Updated two previously incorrect test expectations (both expected `>` to become `|`). Added a dedicated `[BlockScalar]` TEST_CASE with 3 sections: folded emits `>`, literal emits `|`, and a full round-trip value preservation check. All 2439 assertions pass (56 test cases).

---

### 3.8 ✅ FIXED — yaml-test-suite coverage is too sparse (~16 of ~351 cases)

**Location:** `tests/source/parse/YAML_Lib_Tests_Parse_YamlTestSuite.cpp`  
**Problem:**  
The yaml-test-suite ships with the repository (`tests/yaml-test-suite/yaml-test-suite/src/`, 351 `.yaml` files) but only ~16 cases were exercised in the integration test. Many important spec-coverage areas were not tested at all (flow indicator rules, complex block structures, character set validation, spec examples from every chapter).  
**Impact:** Unknown failures in untested cases; no regression guard for new parser changes.  
**Fix:** Expanded the test file to run all 351 suite cases programmatically via a `[Sweep]` TEST_CASE. Added `YAML_SUITE_SRC_DIR` compile definition (via `tests/CMakeLists.txt`), `extractYamlField()` and `isFail()` helpers, and a loop using `CHECK_NOTHROW`/`CHECK_THROWS` so failures are reported per-file rather than aborting the suite.  
**Resolution:** All 351 test-suite files now run without crashing. Fixed a critical `parseAnchor` bug: the old `do-while` loop spun infinitely at EOF (OOM crash triggered by anchor-as-mapping-key inputs like `2SXE`); fixed to `while(source.more() && ...)`. Fixed a second `parseAnchor` bug: anchors with empty (null) values at sibling-key indentation level now correctly resolve to null instead of consuming the next sibling key (which caused infinite recursion / stack overflow for `6KGN`). 109 of 351 files fail — these are known parser limitations (65 over-strict rejections, 44 under-strict acceptances) and serve as a regression baseline. All 3143 assertions pass in the 56 pre-existing test cases.  
**Known failing suite IDs (109):** 26DV, 2EBW, 2G84, 2JQS, 2SXE, 3HFZ, 4EJS, 4JVG, 4Q9F, 4RWC, 55WF, 565N, 5GBF, 5LLU, 5TRB, 5U3A, 5WE3, 6BCT, 6BFJ, 6FWR, 6HB6, 6PBE, 753E, 7BMT, 7FWL, 7LBH, 7TMG, 7W2P, 7ZZ5, 8UDB, 8XDJ, 93WF, 9C9N, 9JBA, 9KBC, 9MMA, 9MMW, AZ63, B3HG, B63P, BF9H, CN3R, CQ3W, CT4Q, CVW2, CXX2, D49Q, DC7X, F2C7, FH7J, G5U8, G992, GDY7, H2RW, H7TQ, HMQ5, HRE5, J3BT, JKF3, JTV5, JY7Z, K3WX, K527, KK5P, L24T, LHL4, LP6E, M29M, M9B4, MJS9, MUS6, MYW6, NHX8, NKF9, NP9H, P76L, P94K, Q4CL, Q8AD, QB6E, QF4Y, QLJ7, RLU9, RXY3, RZP5, S3PD, S4GJ, S98Z, S9E8, SF5V, SKE5, SR86, SU5Z, SU74, SY6V, T5N4, TS54, U3XV, U99R, W5VH, W9L4, X38W, X4QW, XV9V, XW4D, Y79Y, YJV2, ZK9H, ZWK4

---

### 3.9 ~~🟡 MEDIUM — `%YAML` version directive has no effect on parsing rules~~ ✅ FIXED (2026-04-07)

**Location:** `Default_Parser.hpp`, `yamlDirectiveMinor`; `YAML_Parser.cpp`  
**Problem:**  
`yamlDirectiveMinor` is stored correctly (2 for `%YAML 1.2`, 1 for `%YAML 1.1`) but is never consulted during parsing to switch schema rules. A `%YAML 1.2` document is parsed with exactly the same rules as a `%YAML 1.1` document.  
**Impact:**  
- YAML 1.2 files may have YAML 1.1 booleans silently coerced (see §3.4).  
- `%YAML 1.1` files with C-style octal (`0777`) should be treated as octal; after the §3.1 fix they would incorrectly become decimal.  
**Fix applied:**  
- `YAML_Parser_Router.cpp` (`isBoolean`): condition changed from `if (strictBooleans)` to `if (strictBooleans || yamlDirectiveMinor >= 2)`. When `yamlDirectiveMinor >= 2` (YAML 1.2), only `t` and `f` are candidate boolean starts.  
- `YAML_Parser_Scalar.cpp` (`parseBoolean`): `strictMode = strictBooleans || yamlDirectiveMinor >= 2` selects between the YAML 1.2 set (`true`/`false` only) and the YAML 1.1 set (`True`/`False`/`Yes`/`No`/`On`/`Off`/…).  
- `YAML_Parser_Scalar.cpp` (`parseNumber`): added YAML 1.1 C-style octal branch — when `yamlDirectiveMinor == 1` and the numeric string matches `^0[0-7]+$`, it is converted as base-8 and stored as its decimal equivalent (e.g. `0777` → `511`).  
- Default (no `%YAML` directive): `yamlDirectiveMinor` defaults to `2`, so YAML 1.2 strict rules apply.  
- 14 test files updated for regressions; 11 new `[Directive]` test sections added across `YAML_Lib_Tests_Parse_Boolean.cpp` and `YAML_Lib_Tests_Parse_Numeric.cpp`.  
**Verification:** 57/58 test cases pass (2483 assertions); 1 failing = yaml-test-suite sweep with the same 109 known failures as pre-implementation baseline.

---

### 3.10 ~~🟢 LOW — Recursive anchor expansion has no cycle guard~~ ✅ FIXED (2026-04-07)

**Location:** `classes/source/implementation/parser/YAML_Parser_Directive.cpp`, `parseAlias()`  
**Problem:**  
If YAML contains a self-referencing anchor (directly or indirectly), alias expansion will recurse infinitely and crash with a stack overflow. YAML 1.2 explicitly prohibits recursive structures but the parser does not detect the cycle.  
**Fix applied:**  
- Added `inline static std::set<std::string> activeAliasExpansions{}` to `Default_Parser.hpp`; reset in `parse()`.  
- In `parseAlias()`: before re-parsing the stored anchor text, check if `name` is already in `activeAliasExpansions` — if so, throw `SyntaxError("Recursive anchor detected: '<name>'")`. Insert `name` on entry; erase on return (try/catch guarantees cleanup on exception).  
- Added three `[RecursiveAnchor]` sections to `YAML_Lib_Tests_Parse_ErrorHandling.cpp`: direct self-reference, indirect two-anchor cycle, and message content check.  
**Verification:** 57/58 test cases pass (2486 assertions); sweep unchanged at 109 known failures.

---

### 3.11 ~~🟢 LOW — `%YAML` directive may appear multiple times without error~~ ✅ FIXED (2026-04-07)

**Location:** `YAML_Parser.cpp`, directive handling  
**Problem:**  
YAML 1.2 spec (§9.2) says at most one `%YAML` directive may appear per document. The parser overwrote `yamlDirectiveMinor` silently on each `%YAML` seen.  
**Fix applied:**  
- Added `inline static bool yamlDirectiveSeen{false}` to `Default_Parser.hpp`; reset in `parse()` and when `inDocument` becomes `false` (document end `...`).  
- In `%YAML` handler: throw `SyntaxError("%YAML directive appears more than once for the same document.")` when `yamlDirectiveSeen` is already `true`; then set it to `true`.  
- Added four `[Directive]` sections to `YAML_Lib_Tests_Parse_ErrorHandling.cpp`: duplicate same-version throws, duplicate different-version throws + message check, and `%YAML` once per document in a two-doc stream does not throw.  
**Verification:** 57/58 test cases pass (2489 assertions); sweep unchanged at 109 known failures.

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

### 3.13 ~~🟢 LOW — No validation of disallowed control characters in YAML content~~ ✅ FIXED (2026-04-07)

**Location:** Parser generally  
**Problem:**  
YAML 1.2 spec §5.1 defines the character set for YAML streams. Control characters (U+0000–U+0008, U+000B, U+000C, U+000E–U+001F, U+007F) are not permitted outside of escaped contexts. The parser did not validate raw input bytes.  
**Fix applied:**  
- Added `static void validateInputCharacters(ISource &source)` to `Default_Parser` (declared in `Default_Parser.hpp`, implemented in `YAML_Parser_Util.cpp`).  
- The function uses `source.save()/restore()` to scan the entire source before parsing: any byte matching the forbidden range (0x00–0x08, 0x0B, 0x0C, 0x0E–0x1F, 0x7F) throws `SyntaxError` with position info and the hex code point. Bytes ≥0x80 (UTF-8 multi-byte sequences) are allowed.  
- Called at the top of `parse()` before the main loop.  
- Also fixed a pre-existing off-by-one in `Default_Translator.cpp` `decodeUTF8()`: condition changed from `numberOfCharacters >= 4` to `>= 3` (need `x` + 2 hex digits = 3 chars, not 4).  
- `YAML_Lib_Tests_Stringify_XML.cpp`: two tests that injected raw control bytes (forbidden) updated to use YAML `\xNN` escape sequences.  
- `YAML_Lib_Tests_Parse_ErrorHandling.cpp`: 6 new `[ControlChar]` sections: NUL, VT, FF, DEL throw; error message contains hex code point; TAB/LF/CR don’t throw.  
**Verification:** 57/58 test cases pass (2495 assertions); sweep improved from 109 → 108 known failures (decodeUTF8 fix resolved one suite case).

---

### 3.14 ✅ FIXED (2026-04-07) — `!!omap` and `!!pairs` have no dedicated node type

**Location:** `classes/include/implementation/variants/`  
**Problem:**  
YAML 1.2 schema includes `!!omap` (ordered map) and `!!pairs` (sequence of key-value pairs allowing duplicate keys). Both are parsed as generic dictionaries/sequences. Ordered insertion is already preserved by the `Dictionary` type (it uses a vector, not a hash map), so `!!omap` round-trips correctly in practice, but there is no node-level distinction.  
**Fix (minimal):** Added explicit `!!omap` and `!!pairs` branches in `parseTagged()` (`YAML_Parser_Tag.cpp`). Tag is stored on the resulting node via `setTag`. Three tests added to `YAML_Lib_Tests_Parse_Tags.cpp` confirming tag preservation.  
**Fix (full):** Add `OrderedMap` and `Pairs` variant types for semantic distinction. Not in scope — see §"Out of Scope" note below.  

---

### 3.15 ✅ FIXED (2026-04-07) — Single-quoted scalars: `''` escape only handled at outer parse, not inside key extraction

**Location:** `YAML_Parser_Util.cpp`, `extractString()`  
**Problem:**  
`extractString` scans until it finds the closing quote without handling the `''` (escaped apostrophe) inside single-quoted strings. Single-quoted key extraction (`extractKey`) uses `extractString`, so a single-quoted key with an embedded `''` is likely mis-parsed.  
**Fix:** In `extractString`, when a `'` is encountered and `quote == kApostrophe`, advance past it and peek: if the next char is also `'`, emit both quotes into the raw string and continue scanning; otherwise the real closing quote was already consumed, so append the closing quote and return. Two tests added to `YAML_Lib_Tests_Parse_String.cpp` verifying `''` in keys with one and multiple escapes (`[SingleQuoteKey]`).

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

### P5 ✅ DONE — Fix plain scalar `#` comment detection
**Files:** `classes/source/implementation/parser/YAML_Parser_FlowString.cpp`, `YAML_Parser_Util.cpp`  
**Task:**  
Model A (simpler): In `parsePlainFlowString`, after extracting the first-line portion, post-process the accumulated scalar string. Find `" #"` (space-hash) as a comment boundary and truncate there; leave bare `#` inside the word intact.  
Model B (precise): Split `extractToNext` into `extractToNextComment` that stops at `#` only when the immediately preceding character was a space/tab.  
**Test:** `foo#bar: value` — key is `"foo#bar"`; `foo #comment` — value is `"foo"`.  
**Acceptance:** yaml-test-suite cases that involve inline comments on plain scalars pass.  
**Resolution:** Used inline while-loop extension in `parsePlainFlowString` (variant of Model B). 2394 assertions pass.

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

### P7 ✅ DONE (2026-04-07) — Version-directive-sensitive schema
**Files:** `YAML_Parser_Router.cpp`, `YAML_Parser_Scalar.cpp`  
**Changes applied:**
- `isBoolean()`: condition `strictBooleans || yamlDirectiveMinor >= 2` gates strict-1.2 boolean recognition.
- `parseBoolean()`: `strictMode = strictBooleans || yamlDirectiveMinor >= 2` selects boolean vocabulary.
- `parseNumber()`: `yamlDirectiveMinor == 1` branch handles `0NNN` as base-8 (C-style octal).
- No-directive default: `yamlDirectiveMinor = 2` (YAML 1.2 strict).
**Verification:** `%YAML 1.1` + `0777` → 511; no directive + `0777` → 777 (decimal). 57/58 test cases pass (2483 assertions).

---

### ~~P8 — Recursive anchor cycle guard~~ ✅ DONE (2026-04-07)
**Files:** `Default_Parser.hpp`, `YAML_Parser_Directive.cpp` (`parseAlias`), `YAML_Parser.cpp`  
**Changes applied:**
- `activeAliasExpansions` set added; cleared in `parse()`.
- `parseAlias()`: cycle check before expansion; insert/erase with try/catch cleanup.
- `YAML_Lib_Tests_Parse_ErrorHandling.cpp`: 3 new `[RecursiveAnchor]` sections.
**Verification:** Self-referencing and cross-referencing anchors throw `SyntaxError` naming the anchor; 2486 assertions pass.

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

### P11 ✅ DONE (partial) — Tag preservation in stringify
**Files:** `classes/include/implementation/stringify/Default_Stringify.hpp`, all `stringify*` methods  
**Task:**  
1. After P4 (folded/literal fix): in `stringifyNodes`, check `yNode.getVariant().getTag()` for non-empty tags and emit them before the value (`"<tag> "`).  
2. Emit `!!timestamp` before `Timestamp` nodes.  
3. Document that `!!binary` values are output as-is (raw base64 string).  
**Test:** A node created with `!!str 42` stringifies as `!!str 42`; round-trip preserves the string type.  
**Acceptance:** Tagged-node round-trip tests pass.  
**Resolution:** Scalar tags emitted via `tagToEmitForm()` helper. Block-string tags emitted in `stringifyAnyBlockStyle()`. `!!timestamp` tag preserved when set. Array/Dictionary tag emission remains as future work. 2429 assertions pass.

---

### ~~P12 — Validate disallowed control characters~~ ✅ DONE (2026-04-07)
**Files:** `Default_Parser.hpp`, `YAML_Parser_Util.cpp`, `YAML_Parser.cpp`, `Default_Translator.cpp`  
**Changes applied:**
- `validateInputCharacters(ISource &)`: pre-scan using save/restore; throws `SyntaxError` on any forbidden ASCII control byte.
- Called at top of `parse()` before anything else.
- `decodeUTF8()` in translator: fixed off-by-one (`>= 4` → `>= 3`).
- `YAML_Lib_Tests_Stringify_XML.cpp`: raw byte tests converted to `\xNN` YAML escapes.
- 6 new `[ControlChar]` error-handling test sections.
**Verification:** NUL/VT/FF/DEL/SOH throw; TAB/LF/CR do not. 2495 assertions pass. Sweep: 108 known failures (improved from 109).

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

### ~~P14 — `%YAML` duplicate directive detection~~ ✅ DONE (2026-04-07)
**Files:** `Default_Parser.hpp`, `YAML_Parser.cpp`  
**Changes applied:**
- `yamlDirectiveSeen` flag added; cleared in `parse()` and on document end (`...`).
- `%YAML` handler: throws `SyntaxError` if `yamlDirectiveSeen` is already `true`.
- `YAML_Lib_Tests_Parse_ErrorHandling.cpp`: 4 new `[Directive]` sections.
**Verification:** Two `%YAML` lines before one `---` throw; one per document in multi-doc stream does not. 2489 assertions pass.

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
| 9 | ~~P7~~ | ~~Version-directive schema switch~~ | ✅ DONE | — |
| 10 | P11 | Tag preservation in stringify | 🟡 MEDIUM | Medium–High |
| 11 | ~~P8~~ | ~~Recursive anchor guard~~ | ✅ DONE | — |
| 12 | ~~P14~~ | ~~Duplicate `%YAML` detection~~ | ✅ DONE | — |
| 13 | ~~P12~~ | ~~Control character validation~~ | ✅ DONE | — |
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
