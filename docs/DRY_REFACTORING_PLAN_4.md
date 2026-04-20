# DRY Refactoring Plan 4 — Parser Layer (continued)

> Prerequisite: Plans 1, 2 and 3 are fully applied and committed.  
> All six steps from Plan 3 have been verified present in the codebase:
> `parseFromBuffer`, `captureIndentedBlock`, `upsertDictEntry`,
> `extractInlineCollectionAt`, `matchesMarker`/`parseOverride` cleanup,
> and the `looksLikeIso8601Date` local `dig` lambda.
>
> After each step, run the full regression suite
> (`YAML_Lib_Unit_Tests` — 58 tests / 3157 assertions) before committing.

---

## Context: what is already clean

After Plans 1–3 the following helpers are in use:

| Helper | File | Replaced pattern |
|--------|------|-----------------|
| `extractTrimmed` | `YAML_Parser_Util.cpp` | `extractToNext` + `rightTrim` |
| `matchesMarker` | `YAML_Parser_Router.cpp` | `SourceGuard` + `source.match` |
| `isDocumentBoundary` | `YAML_Parser_Router.cpp` | `isDocumentStart(s)\|\|isDocumentEnd(s)` |
| `passthroughTags` set | `YAML_Parser_Tag.cpp` | 4 identical branch enum |
| `isInlineComment` | `YAML_Parser_FlowString.cpp` | inline `#`-preceded-by-WS predicate |
| `convertOctalToDecimal` | `YAML_Parser_Scalar.cpp` | duplicated `stoll(..., 8)` blocks |
| `parseFromBuffer` | `YAML_Parser_Util.cpp` | `BufferSource` + `parseDocument` (anchor/alias/override) |
| `captureIndentedBlock` | `YAML_Parser_Util.cpp` | inline while-loop in `parseAnchor` |
| `upsertDictEntry` | `YAML_Parser_Util.cpp` | insert-or-overwrite pattern in `mergeOverrides` |
| `extractInlineCollectionAt` | `YAML_Parser_Dictionary.cpp` | dual `isInlineDict/Array` + `extractInLine` dispatch |
| `tryParseToken` | `Default_Parser.hpp` | `SourceGuard` + extract + trim + predicate pattern |

---

## Remaining duplication (5 steps)

---

### Step 1 — Apply `parseFromBuffer` at the two remaining `BufferSource` sites in `YAML_Parser_Dictionary.cpp`

**Pattern**  
`parseFromBuffer` was introduced in Plan 3 to eliminate the
`BufferSource src{text}; return parseDocument(src, delimiters, indentation);` idiom.
Two early-call sites in `YAML_Parser_Dictionary.cpp` were not updated at that time
and still use the raw idiom.

```cpp
// ── current: convertYAMLToStringNode (line ~50) ──
BufferSource yamlKey{std::string(yamlString) + kLineFeed};
auto keyNode = parseDocument(yamlKey, {kLineFeed}, 0);

// ── current: isValidKey (line ~69) ──
BufferSource yamlKey{std::string(key) + kLineFeed};
const Node keyNode = parseDocument(yamlKey, {kLineFeed}, 0);
```

**After**

```cpp
// convertYAMLToStringNode
auto keyNode = parseFromBuffer(std::string(yamlString) + kLineFeed, {kLineFeed}, 0);

// isValidKey
const Node keyNode = parseFromBuffer(std::string(key) + kLineFeed, {kLineFeed}, 0);
```

**Files touched:** `YAML_Parser_Dictionary.cpp` only (no header change needed).

**Net change:** `−4` lines (both local `BufferSource` variables removed).

---

### Step 2 — Replace four manual `while` character-accumulation loops in `parseTagged` with `extractToNext`

**Pattern**  
`parseTagged` (`YAML_Parser_Tag.cpp`) contains four `while` loops that each accumulate
characters from the source stream one character at a time. All four follow the identical
low-level pattern that `extractToNext` already abstracts:

```cpp
while (source.more() && <stop-condition>) {
    name += source.current();
    source.next();
}
```

The four occurrences and their natural `extractToNext` equivalents:

| Location in `parseTagged` | Current loop stops when… | Replacement |
|---|---|---|
| Verbatim tag content (`<…>`) | `source.current() == '>'` | `extractToNext(source, {'>'})` |
| Secondary `!!` suffix | space or LF | `extractToNext(source, {kSpace, kLineFeed})` |
| Pre-`!` scan (`!ns!suffix`) | `'!'`, space, or LF | `extractToNext(source, {'!', kSpace, kLineFeed})` |
| Named-handle suffix (after 2nd `!`) | space or LF | `extractToNext(source, {kSpace, kLineFeed})` |

```cpp
// ── current: verbatim tag ──
while (source.more() && source.current() != '>') {
    tagHandle += source.current();
    source.next();
}
if (!source.more() || source.current() != '>') {
    throw SyntaxError(source.getPosition(), "Unclosed verbatim tag '<'.");
}

// ── after ──
tagHandle = extractToNext(source, {'>'});
if (!source.more() || source.current() != '>') {
    throw SyntaxError(source.getPosition(), "Unclosed verbatim tag '<'.");
}
```

```cpp
// ── current: secondary !! suffix ──
while (source.more() && source.current() != kSpace &&
       source.current() != kLineFeed) {
    tagSuffix += source.current();
    source.next();
}

// ── after ──
tagSuffix = extractToNext(source, {kSpace, kLineFeed});
```

```cpp
// ── current: pre-! scan ──
while (source.more() && source.current() != '!' &&
       source.current() != kSpace && source.current() != kLineFeed) {
    preExcl += source.current();
    source.next();
}

// ── after ──
preExcl = extractToNext(source, {'!', kSpace, kLineFeed});
```

```cpp
// ── current: named-handle suffix ──
while (source.more() && source.current() != kSpace &&
       source.current() != kLineFeed) {
    tagSuffix += source.current();
    source.next();
}

// ── after ──
tagSuffix = extractToNext(source, {kSpace, kLineFeed});
```

**Files touched:** `YAML_Parser_Tag.cpp` only (no header change needed).

**Net change:** `−12` lines (4 × 3-line loops replaced by single-line calls).

---

### Step 3 — Use `tryParseToken` in `parseTimestamp`

**Pattern**  
`parseTimestamp` (`YAML_Parser_Timestamp.cpp`) manually implements the exact save/
extract/trim/verify/release pattern that `tryParseToken` already abstracts.
Three other scalar parsers (`parseNumber`, `parseNone`, `parseBoolean`) already use
`tryParseToken`; `parseTimestamp` was written independently before the helper existed.

```cpp
// ── current ──
Node Default_Parser::parseTimestamp(
    ISource &source, const Delimiters &delimiters,
    [[maybe_unused]] unsigned long indentation) {
  SourceGuard guard(source);
  const std::string raw{extractTrimmed(source, delimiters)};
  if (looksLikeIso8601Date(raw)) {
    guard.release();
    return Node::make<Timestamp>(raw);
  }
  return {};
}
```

`tryParseToken` does exactly: `SourceGuard` + `extractToNext` + `rightTrim` (≡
`extractTrimmed`) + predicate + conditional `guard.release()`.

```cpp
// ── after ──
Node Default_Parser::parseTimestamp(
    ISource &source, const Delimiters &delimiters,
    [[maybe_unused]] unsigned long indentation) {
  return tryParseToken(source, delimiters, [](const std::string &tok) -> Node {
    if (looksLikeIso8601Date(tok))
      return Node::make<Timestamp>(tok);
    return {};
  });
}
```

**Files touched:** `YAML_Parser_Timestamp.cpp` only (no header change needed).

**Net change:** `−4` lines. `parseTimestamp` now follows the same shape as the other
scalar parsers, making the pattern explicit and consistent.

---

### Step 4 — Extract `checkFlowDelimiter` to eliminate the duplicate post-close check in inline collections

**Pattern**  
After closing an inline collection (array or dictionary), both `parseInlineArray`
(`YAML_Parser_Array.cpp`) and `parseInlineDictionary`
(`YAML_Parser_Dictionary.cpp`) contain identical 4-line blocks that throw a
`SyntaxError` when the next character is not an expected delimiter at the outermost
nesting level:

```cpp
// ── parseInlineArray (lines ~86-90) ──
if (source.more() && inlineArrayDepth == 0) {
    if (!delimiters.contains(source.current())) {
        throw SyntaxError("Unexpected flow sequence token '" +
                          std::string(1, source.current()) + "'.");
    }
}

// ── parseInlineDictionary (lines ~296-300) ──
if (source.more() && inlineDictionaryDepth == 0) {
    if (!delimiters.contains(source.current())) {
        throw SyntaxError("Unexpected flow sequence token '" +
                          std::string(1, source.current()) + "'.");
    }
}
```

The depth variable is always `0` at the call site (both functions decrement their
respective depth variable immediately before this block), so it adds no information;
the guard is structurally identical.

**New helper** — add `private` static in `Default_Parser.hpp`; implement in
`YAML_Parser_Util.cpp` (alongside the other small helpers):

```cpp
// Default_Parser.hpp (private section, near checkForEnd)
static void checkFlowDelimiter(ISource &source, const Delimiters &delimiters);

// YAML_Parser_Util.cpp
/// <summary>
/// At the end of an outermost inline collection, verify that the next
/// character belongs to the caller's delimiter set; throw SyntaxError if not.
/// </summary>
void Default_Parser::checkFlowDelimiter(ISource &source,
                                        const Delimiters &delimiters) {
  if (source.more() && !delimiters.contains(source.current())) {
    throw SyntaxError("Unexpected flow sequence token '" +
                      std::string(1, source.current()) + "'.");
  }
}
```

**Sites to update**

```cpp
// parseInlineArray — replace 4 lines with:
if (inlineArrayDepth == 0) {
    checkFlowDelimiter(source, delimiters);
}

// parseInlineDictionary — replace 4 lines with:
if (inlineDictionaryDepth == 0) {
    checkFlowDelimiter(source, delimiters);
}
```

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_Util.cpp`,
`YAML_Parser_Array.cpp`, `YAML_Parser_Dictionary.cpp`.

**Net change:** `+5 / −8` lines. The error message string is written exactly once,
and the intent ("enforce delimiter at collection boundary") is named.

---

### Step 5 — Replace try/catch duplication in `parseAlias` with an RAII scope guard

**Pattern**  
`parseAlias` (`YAML_Parser_Directive.cpp`) inserts `name` into
`activeAliasExpansions` to detect recursive alias expansion, then removes it after
the recursive parse. The removal call is duplicated in the normal path and the
exception path:

```cpp
// ── current ──
activeAliasExpansions.insert(name);
try {
    auto result = parseFromBuffer(unparsed, delimiters, indentation);
    activeAliasExpansions.erase(name);   // ← path 1
    return result;
} catch (...) {
    activeAliasExpansions.erase(name);   // ← path 2 (identical)
    throw;
}
```

A local RAII guard eliminates the duplication:

```cpp
// ── after ──
activeAliasExpansions.insert(name);
// RAII: always remove 'name' on scope exit, whether normal or exception.
const struct AliasGuard {
    explicit AliasGuard(const std::string &n) : name_(n) {}
    ~AliasGuard() { Default_Parser::activeAliasExpansions.erase(name_); }
    const std::string &name_;
} aliasGuard{name};

return parseFromBuffer(unparsed, delimiters, indentation);
```

Alternatively, if access to the `static` member from a local struct is considered
too tightly coupled, an immediately-invoked lambda with a `std::unique_ptr` custom
deleter is idiomatic:

```cpp
activeAliasExpansions.insert(name);
auto aliasGuard = std::unique_ptr<std::string, void(*)(std::string *)>(
    const_cast<std::string *>(&name),
    [](std::string *n) { activeAliasExpansions.erase(*n); });

return parseFromBuffer(unparsed, delimiters, indentation);
```

The simplest production-quality approach is a plain lambda scope guard using the
same RAII principle but without introducing a new type:

```cpp
activeAliasExpansions.insert(name);
auto eraseOnExit = [&name_ref = name]() noexcept {
    Default_Parser::activeAliasExpansions.erase(name_ref);
};
// C++20: no RAII lambda wrapper in stdlib — use scope_exit from <scope> (gcc/clang
// extension) or implement inline below.
struct ScopeExit {
    std::function<void()> fn;
    ~ScopeExit() { fn(); }
} guard{eraseOnExit};

return parseFromBuffer(unparsed, delimiters, indentation);
```

Since C++20 does not include `std::scope_exit` in the standard library (it is in the
Library Fundamentals TS), the recommended approach for this codebase is the local
RAII struct pattern shown first above, which is self-contained and requires no helper
outside the function.

**Files touched:** `YAML_Parser_Directive.cpp` only (no header change needed).

**Net change:** `−3` lines (try/catch block eliminated; the erase logic is written
exactly once). Correctness is unconditional — exceptions from `parseFromBuffer` can
no longer silently leave a stale entry in `activeAliasExpansions`.

---

## Summary table

| Step | Change | New declaration | Files touched | Δ lines |
|------|--------|-----------------|--------------|---------|
| 1 | `parseFromBuffer` at 2 remaining `BufferSource` sites | — | `Dictionary.cpp` | −4 |
| 2 | Replace 4 manual `while` loops in `parseTagged` with `extractToNext` | — | `Tag.cpp` | −12 |
| 3 | Use `tryParseToken` in `parseTimestamp` | — | `Timestamp.cpp` | −4 |
| 4 | Extract `checkFlowDelimiter` helper | `Default_Parser.hpp` | `Util.cpp`, `Array.cpp`, `Dictionary.cpp` | −3 |
| 5 | RAII scope guard in `parseAlias` | — | `Directive.cpp` | −3 |

**Total net reduction: ≈ −26 lines, +1 named concept.**

---

## Execution order

Each step is independent and can be committed separately.  
Suggested sequence (low risk to high):

1. Step 1 (`parseFromBuffer` at Dictionary sites) — pure text swap, zero behaviour change.
2. Step 2 (`extractToNext` in `parseTagged`) — mechanical loop-to-call replacement; verify tag test cases pass.
3. Step 3 (`tryParseToken` in `parseTimestamp`) — scalar-parser alignment; verify timestamp tests.
4. Step 4 (`checkFlowDelimiter`) — new helper + 2 call sites; verify inline array/dict tests.
5. Step 5 (RAII in `parseAlias`) — correctness improvement; verify anchor/alias/recursive tests.

Each commit message should reference this plan (e.g. "DRY Plan 4 Step N: …").
