# DRY Refactoring Plan 3 — Parser Layer

> Prerequisite: Plans 1 and 2 are fully applied and committed.
> All changes must be followed by a full regression run
> (`YAML_Lib_Unit_Tests.exe` — 58 tests / 3157 assertions) before each commit.

---

## Context: what is already clean

After Plans 1 and 2 the following helpers already exist and are in use:

| Helper | File | Replaced pattern |
|--------|------|-----------------|
| `extractTrimmed` | `YAML_Parser_Util.cpp` | `extractToNext` + `rightTrim` (4 sites) |
| `matchesMarker` | `YAML_Parser_Router.cpp` | `SourceGuard` + `source.match` (2 sites) |
| `isDocumentBoundary` | `YAML_Parser_Router.cpp` | `isDocumentStart(s)\|\|isDocumentEnd(s)` (2 sites) |
| `passthroughTags` set | `YAML_Parser_Tag.cpp` | 4 identical `!!seq/map/omap/pairs` branches |
| `isInlineComment` | `YAML_Parser_FlowString.cpp` | inline `#`-preceded-by-WS predicate (2 sites) |
| `convertOctalToDecimal` | `YAML_Parser_Scalar.cpp` | 2 try-blocks with identical `stoll(..., 8)` logic |

---

## Remaining duplication (6 steps)

---

### Step 1 — Extract `parseFromBuffer`

**Pattern**  
The three functions `parseAnchor`, `parseAlias`, and `parseOverride` each end with the same
two-line idiom: construct a `BufferSource` from an `std::string`, then call `parseDocument`.

```cpp
// ── current (3 occurrences in YAML_Parser_Directive.cpp) ──
BufferSource anchor{unparsed};
return parseDocument(anchor, delimiters, indentation);
```

**New helper** — add to `YAML_Parser_Util.cpp` + declare in `Default_Parser.hpp`

```cpp
static Node parseFromBuffer(const std::string &text,
                            const Delimiters &delimiters,
                            unsigned long indentation) {
    BufferSource src{text};
    return parseDocument(src, delimiters, indentation);
}
```

**Sites to update** (all in `YAML_Parser_Directive.cpp`)

| Function | Lines to replace |
|----------|-----------------|
| `parseAnchor` | `BufferSource anchor{unparsed}; return parseDocument(anchor, ...);` |
| `parseAlias` | `BufferSource anchor{unparsed}; auto result = parseDocument(...);` in try-block |
| `parseOverride` | `BufferSource anchor{unparsed}; Node parsed = parseDocument(...); return parsed;` |

**After**
```cpp
// parseAnchor
return parseFromBuffer(unparsed, delimiters, indentation);

// parseAlias (inside the try/catch)
auto result = parseFromBuffer(unparsed, delimiters, indentation);

// parseOverride
return parseFromBuffer(unparsed, delimiters, indentation);
```

**Net change** `+6 / -9` lines across YAML_Parser_Util.cpp + YAML_Parser_Directive.cpp.

---

### Step 2 — Extract `captureIndentedBlock`

**Pattern**  
Inside `parseAnchor` there is a `while` loop that rebuilds indented source text by
prepending the current column as spaces, extracting to the next newline, then advancing
past whitespace. This logic deserves a name and has no duplicate today, but it is a
dense, 4-line operation that is easily extracted for readability and future reuse.

```cpp
// ── current (YAML_Parser_Directive.cpp — parseAnchor) ──
if (anchorIndent > indentation) {
    while (source.more() && source.getPosition().second >= anchorIndent) {
        std::string indent(source.getPosition().second, kSpace);
        unparsed += indent + extractToNext(source, {kLineFeed}) + "\n";
        moveToNextIndent(source);
    }
}
```

**New helper** — add to `YAML_Parser_Util.cpp` + declare in `Default_Parser.hpp`

```cpp
// Collect all lines whose column is >= minIndent into a single string,
// preserving synthetic indentation and separating lines with '\n'.
static std::string captureIndentedBlock(ISource &source,
                                        unsigned long minIndent) {
    std::string text;
    while (source.more() && source.getPosition().second >= minIndent) {
        text += std::string(source.getPosition().second, kSpace)
              + extractToNext(source, {kLineFeed}) + "\n";
        moveToNextIndent(source);
    }
    return text;
}
```

**Site to update** (`parseAnchor` in `YAML_Parser_Directive.cpp`)

```cpp
if (anchorIndent > indentation) {
    unparsed = captureIndentedBlock(source, anchorIndent);
}
```

**Net change** `+7 / -5` lines across the two files — small but the `parseAnchor` body
becomes noticeably easier to read.

---

### Step 3 — Extract `upsertDictEntry`

**Pattern**  
`mergeOverrides` (in `YAML_Parser_Directive.cpp`) has two branches — single-alias merge
and multi-alias merge — that apply the same "insert if absent, overwrite if present"
operation on a `Dictionary`. The 4-line block is duplicated verbatim:

```cpp
// ── current (appears TWICE in mergeOverrides) ──
if (dict.contains(entry)) {
    dict[entry] = std::move(overrideEntry);
} else {
    dict.add(DictionaryEntry(entry, overrideEntry));
}
```

**New helper** — add to `YAML_Parser_Util.cpp` + declare in `Default_Parser.hpp`

```cpp
static void upsertDictEntry(Dictionary &dict,
                            const std::string &key,
                            Node value) {
    if (dict.contains(key)) {
        dict[key] = std::move(value);
    } else {
        dict.add(DictionaryEntry(key, std::move(value)));
    }
}
```

**Sites to update** (both inside `mergeOverrides`)

```cpp
// Single-alias branch — was 4 lines
upsertDictEntry(innerDictionary, entry, std::move(overrideEntry));

// Multi-alias branch — was 4 lines
upsertDictEntry(mergedDict, entry, std::move(overrideEntry));
```

**Net change** `+8 / -8` lines — same size but the two loops become single-line calls
and the intent is captured by the name.

---

### Step 4 — Extract `extractInlineCollectionAt`

**Pattern**  
`extractKey` and `extractMapping` (both in `YAML_Parser_Dictionary.cpp`) both contain
the same dispatch: if the current character is `{` call
`extractInLine(source, '{', '}')`, if it is `[` call
`extractInLine(source, '[', ']')`.

```cpp
// ── extractKey ──
if (isInlineDictionary(source)) {
    return extractInLine(source, kLeftCurlyBrace, kRightCurlyBrace);
}
if (isInlineArray(source)) {
    return extractInLine(source, kLeftSquareBracket, kRightSquareBracket);
}

// ── extractMapping ──
if (isInlineDictionary(source)) {
    key += extractInLine(source, kLeftCurlyBrace, kRightCurlyBrace);
    moveToNext(source, {kColon});
} else if (isInlineArray(source)) {
    key += extractInLine(source, kLeftSquareBracket, kRightSquareBracket);
    moveToNext(source, {kColon});
}
```

The only difference is that `extractMapping` appends a `moveToNext({kColon})` call
after the extraction; the extraction dispatch itself is identical.

**New helper** — add to `YAML_Parser_Dictionary.cpp` + declare in `Default_Parser.hpp`

```cpp
// Precondition: isInlineDictionary(source) || isInlineArray(source).
// Reads source.current() to find the open bracket, extracts the matching
// balanced span (including both bracket characters), and returns it.
static std::string extractInlineCollectionAt(ISource &source) {
    const char start = source.current();
    const char end = (start == kLeftCurlyBrace)
                         ? kRightCurlyBrace
                         : kRightSquareBracket;
    return extractInLine(source, start, end);
}
```

**Sites to update**

```cpp
// extractKey
if (isInlineDictionary(source) || isInlineArray(source))
    return extractInlineCollectionAt(source);

// extractMapping
if (isInlineDictionary(source) || isInlineArray(source)) {
    key += extractInlineCollectionAt(source);
    moveToNext(source, {kColon});
}
```

**Net change** `+7 / -10` lines across the helper definition and the two call-sites.

---

### Step 5 — Clean up `parseOverride` — replace 3× `source.next()` with `source.match`

**Pattern**  
`parseOverride` hardcodes three consecutive `source.next()` calls to consume `<<:`.
This is fragile: the character sequence is not named and the intent is invisible.
`source.match("<<:")` already exists and consumes exactly those characters when invoked
without a `SourceGuard`.

```cpp
// ── current (YAML_Parser_Directive.cpp — parseOverride) ──
source.next();   // '<'
source.next();   // '<'
source.next();   // ':'
source.ignoreWS();
```

**After**

```cpp
[[maybe_unused]] const bool consumed = source.match("<<:");
source.ignoreWS();
```

(`consumed` is always `true` here because the routing table has already confirmed
`isOverride`; casting away the result with `[[maybe_unused]]` makes the unconsumed
return-value warning go away without a bare `(void)` cast.)

**Net change** `-2` lines, intent now explicit.

---

### Step 6 — Reduce verbosity in `looksLikeIso8601Date`

**Pattern**  
The 10 `std::isdigit(static_cast<unsigned char>(s[i]))` expressions in
`looksLikeIso8601Date` (in `YAML_Parser_Timestamp.cpp`) repeat an identical
five-token cast pattern. A local lambda names the pattern once.

```cpp
// ── current ──
return s.size() >= 10 &&
       std::isdigit(static_cast<unsigned char>(s[0])) &&
       std::isdigit(static_cast<unsigned char>(s[1])) &&
       std::isdigit(static_cast<unsigned char>(s[2])) &&
       std::isdigit(static_cast<unsigned char>(s[3])) && s[4] == '-' &&
       std::isdigit(static_cast<unsigned char>(s[5])) &&
       std::isdigit(static_cast<unsigned char>(s[6])) && s[7] == '-' &&
       std::isdigit(static_cast<unsigned char>(s[8])) &&
       std::isdigit(static_cast<unsigned char>(s[9]));
```

**After** (single function: no new helper declaration needed)

```cpp
// ── after ──
if (s.size() < 10) return false;
const auto dig = [&](int i) {
    return std::isdigit(static_cast<unsigned char>(s[i])) != 0;
};
return dig(0) && dig(1) && dig(2) && dig(3) && s[4] == '-'
    && dig(5) && dig(6)  && s[7] == '-'
    && dig(8) && dig(9);
```

**Net change** `-3` lines, cast idiom is written exactly once.

---

## Summary table

| Step | Helper / change | New declaration | Files touched | ∆ lines |
|------|----------------|----------------|--------------|---------|
| 1 | `parseFromBuffer` | `Default_Parser.hpp` | Util.cpp + Directive.cpp | −3 |
| 2 | `captureIndentedBlock` | `Default_Parser.hpp` | Util.cpp + Directive.cpp | +2 |
| 3 | `upsertDictEntry` | `Default_Parser.hpp` | Util.cpp + Directive.cpp | 0 |
| 4 | `extractInlineCollectionAt` | `Default_Parser.hpp` | Dictionary.cpp | −3 |
| 5 | Replace 3× `source.next()` | — | Directive.cpp | −2 |
| 6 | Local `dig` lambda | — | Timestamp.cpp | −3 |

**Total net reduction: ≈ −11 lines, +4 named concepts.**

---

## Execution order

Perform each step as an independent commit:

1. Step 1 (`parseFromBuffer`)
2. Step 2 (`captureIndentedBlock`)  — depends on Step 1 (same `parseAnchor` body)
3. Step 3 (`upsertDictEntry`)
4. Step 4 (`extractInlineCollectionAt`)
5. Step 5 (`parseOverride` cleanup)
6. Step 6 (`looksLikeIso8601Date` lambda)

Each commit message should reference this plan (e.g. "DRY Plan 3 Step N: …").
