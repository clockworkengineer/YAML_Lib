# DRY Refactoring Plan 6 — Parser Layer (continued)

> Prerequisite: Plans 1–5 are fully applied and committed.
> After each step, run the full regression suite
> (`YAML_Lib_Unit_Tests` — "YAML test-suite*" ≥ 715 assertions) before committing.
> Commit messages should reference this plan, e.g. "DRY Plan 6 Step N: …".

---

## Context: what Plans 1–5 have already cleaned up

| Helper / Pattern | Introduced in |
|------------------|--------------|
| `extractTrimmed`, `rightTrim` | Plan 1 |
| `matchesMarker`, `isDocumentBoundary` | Plan 1 |
| `passthroughTags` set in `parseTagged` | Plan 2 |
| `isInlineComment`, `convertOctalToDecimal` | Plan 2 |
| `parseFromBuffer`, `captureIndentedBlock` | Plan 3 |
| `upsertDictEntry`, `extractInlineCollectionAt` | Plan 3 |
| `tryParseToken<>`, `checkFlowDelimiter` | Plan 4 |
| RAII `AliasGuard` in `parseAlias` | Plan 4 |
| `parseDirective` extracted from `parse()` | Plan 5 Step 1 |
| `extractMapping` tail-recursion → loop | Plan 5 Step 2 |
| `applyOuter` lambda in `mergeOverrides` | Plan 5 Step 3 |
| `withExtras` delimiter merge helper | Plan 5 Step 4 |
| `scanToFirstBlockContent` + W9L4 fix | Plan 5 Step 5 |

---

## Remaining duplication (5 steps)

---

### Step 1 — Add `isInlineCollection` predicate

#### Pattern

The compound check `isInlineDictionary(source) || isInlineArray(source)` appears
**four times** across the parser, always with exactly the same meaning: "the next
token begins a `{...}` or `[...]` inline collection":

```cpp
// extractKey — anchor branch (YAML_Parser_Dictionary.cpp ~line 273)
if (isInlineDictionary(source) || isInlineArray(source)) {
    result += extractInlineCollectionAt(source);

// extractKey — plain-scalar / main branch (~line 285)
if (isInlineDictionary(source) || isInlineArray(source)) {
    return extractInlineCollectionAt(source);

// extractMapping (~line 101)
if (isInlineDictionary(source) || isInlineArray(source)) {
    key += extractInlineCollectionAt(source);

// parseKeyValue (~line 238)
if (source.more() && (source.getPosition().second > keyIndent ||
                      isInlineArray(source) || isInlineDictionary(source))) {
    dictionaryNode = parseDocument(source, delimiters, indentation);
```

The duplicated `||` pair mixes operand order (`Dictionary || Array` in three
places, `Array || Dictionary` in the fourth), which makes searching the codebase
unreliable.  A single named predicate fixes both the duplication and the
inconsistency.

#### New helper

```cpp
// Default_Parser.hpp — private declaration (near isInlineDictionary)
static bool isInlineCollection(const ISource &source);

// YAML_Parser_Router.cpp — implementation (one liner)
bool Default_Parser::isInlineCollection(const ISource &source) {
  return isInlineDictionary(source) || isInlineArray(source);
}
```

#### Sites to update

```cpp
// extractKey (twice):
if (isInlineCollection(source)) { result += extractInlineCollectionAt(source); }
if (isInlineCollection(source)) { return extractInlineCollectionAt(source); }

// extractMapping:
if (isInlineCollection(source)) { key += extractInlineCollectionAt(source); }

// parseKeyValue:
if (source.more() && (source.getPosition().second > keyIndent ||
                      isInlineCollection(source))) {
```

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_Router.cpp` (new impl),
`YAML_Parser_Dictionary.cpp`.  
**Net change:** −6 tokens per site × 4 sites = shorter and consistently ordered.

---

### Step 2 — Extract `keyStopDelimiters()` for the context-switching key delimiter

#### Pattern

The same conditional `Delimiters` expression appears **twice** in `extractKey`
inside `YAML_Parser_Dictionary.cpp`:

```cpp
// Anchor-value branch:
const Delimiters valStop =
    inlineDictionaryDepth > 0
        ? Delimiters{kColon, kComma, kRightCurlyBrace, kLineFeed}
        : Delimiters{kColon, kLineFeed};
result += extractToNext(source, valStop);

// Plain-scalar branch (a few lines later):
const Delimiters plainKeyDelimiters =
    inlineDictionaryDepth > 0
        ? Delimiters{kColon, kComma, kRightCurlyBrace, kLineFeed}
        : Delimiters{kColon, kLineFeed};
```

The two occurrences have different local names (`valStop` vs `plainKeyDelimiters`)
but are **byte-for-byte identical**.  The rule they encode is: in flow context
(inside `{}`), colon, comma, and `}` all stop key extraction; in block context
only colon and newline do.  That rule belongs in one place.

#### New helper

```cpp
// Default_Parser.hpp — private declaration (near withExtras)
static Delimiters keyStopDelimiters();

// YAML_Parser_Dictionary.cpp — implementation
Default_Parser::Delimiters Default_Parser::keyStopDelimiters() {
  return inlineDictionaryDepth > 0
             ? Delimiters{kColon, kComma, kRightCurlyBrace, kLineFeed}
             : Delimiters{kColon, kLineFeed};
}
```

#### Sites to update

```cpp
// extractKey — anchor branch:
result += extractToNext(source, keyStopDelimiters());

// extractKey — plain-scalar branch:
const auto plainKeyDelimiters = keyStopDelimiters();
while (source.more()) {
    key += extractToNext(source, plainKeyDelimiters);
    ...
}
```

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_Dictionary.cpp`.  
**Net change:** −5 lines; the flow-vs-block delimiter rule is named and lives in
one place.

---

### Step 3 — Simplify `isOverride` to delegate to `matchesMarker`

#### Pattern

`matchesMarker` was introduced in Plan 1 to encapsulate the `SourceGuard` +
`source.match(...)` idiom.  However `isOverride` was not updated and still
contains the identical pattern inline:

```cpp
// YAML_Parser_Router.cpp
bool Default_Parser::isOverride(ISource &source) {
  SourceGuard guard(source);          // ← identical to matchesMarker body
  return source.match("<<:");         // ←
}

bool Default_Parser::matchesMarker(ISource &source, const char *marker) {
  SourceGuard guard(source);
  return source.match(marker);
}
```

`isDocumentStart` and `isDocumentEnd` already correctly delegate to
`matchesMarker`.  `isOverride` is the lone exception.

#### After

```cpp
bool Default_Parser::isOverride(ISource &source) {
  return matchesMarker(source, "<<:");
}
```

**Files touched:** `YAML_Parser_Router.cpp` only.  
**Net change:** −3 lines; `isOverride` is now consistent with `isDocumentStart`
and `isDocumentEnd`.

---

### Step 4 — Add `extractString(ISource &)` overload to remove redundant argument

#### Pattern

`extractString` takes two parameters: `(ISource &source, char quote)`.  Every
call site passes `source.current()` as the quote character:

```cpp
// extractKey — anchor branch (YAML_Parser_Dictionary.cpp)
result += extractString(source, source.current());

// extractKey — plain-scalar branch
return extractString(source, source.current());

// extractRawScalar lambda in parseTagged (YAML_Parser_Tag.cpp)
std::string raw = extractString(source, source.current());
```

The call site is always responsible for also asserting `isQuotedString(source)`
just before, so `source.current()` is always `'` or `"` at that point.  Passing
it explicitly adds noise without adding information.

A zero-argument overload that reads the quote from `source.current()` removes the
redundancy at every call site:

#### New overload

```cpp
// Default_Parser.hpp — private declaration (next to existing extractString)
static std::string extractString(ISource &source);   // uses source.current() as quote

// YAML_Parser_Util.cpp — implementation
std::string Default_Parser::extractString(ISource &source) {
  return extractString(source, source.current());
}
```

#### Sites to update

```cpp
// All three call sites become:
result += extractString(source);
return extractString(source);
std::string raw = extractString(source);
```

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_Util.cpp`,
`YAML_Parser_Dictionary.cpp`, `YAML_Parser_Tag.cpp`.  
**Net change:** −3 tokens per site × 3 sites; clarifies that the quote char is
always the current character.

---

### Step 5 — Add `isInsideFlowContext()` to name the depth-pair check

#### Pattern

The condition `inlineArrayDepth > 0 || inlineDictionaryDepth > 0` — meaning
"the parser is currently inside at least one `[...]` or `{...}` flow collection"
— appears **three times**:

```cpp
// parsePlainFlowString (YAML_Parser_FlowString.cpp)
const bool stopAtFlowIndicator =
    (inlineArrayDepth > 0 || inlineDictionaryDepth > 0) &&
    (source.current() == kRightSquareBracket || ...);

// parsePlainFlowString (same function, a few lines later)
if ((inlineArrayDepth > 0 || inlineDictionaryDepth > 0) &&
    yamlString.size() == 1 && ...

// parseDocument (YAML_Parser.cpp)
if ((inlineArrayDepth > 0 || inlineDictionaryDepth > 0) &&
    isDocumentBoundary(source)) {
```

The condition mixes two unrelated state variables to express a single logical
concept.  A named predicate encapsulates both the concept and the fields:

#### New helper

```cpp
// Default_Parser.hpp — private declaration (near inlineArrayDepth /
//                       inlineDictionaryDepth inline static members)
[[nodiscard]] static bool isInsideFlowContext() noexcept;

// YAML_Parser_Util.cpp — implementation (or any existing .cpp; Util is best)
bool Default_Parser::isInsideFlowContext() noexcept {
  return inlineArrayDepth > 0 || inlineDictionaryDepth > 0;
}
```

#### Sites to update

```cpp
// parsePlainFlowString (two calls):
const bool stopAtFlowIndicator =
    isInsideFlowContext() &&
    (source.current() == kRightSquareBracket || ...);

if (isInsideFlowContext() && yamlString.size() == 1 && ...

// parseDocument:
if (isInsideFlowContext() && isDocumentBoundary(source)) {
```

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_Util.cpp`,
`YAML_Parser_FlowString.cpp`, `YAML_Parser.cpp`.  
**Net change:** −3 lines; the concept of "inside a flow collection" is named
once, making future changes to flow-depth tracking a one-file edit.

---

## Summary table

| Step | Change | New declaration | Files touched | Δ lines |
|------|--------|-----------------|---------------|---------|
| 1 | Add `isInlineCollection` predicate | `Default_Parser.hpp` | `Router.cpp`, `Dictionary.cpp` | −8 |
| 2 | Extract `keyStopDelimiters()` | `Default_Parser.hpp` | `Dictionary.cpp` | −5 |
| 3 | Simplify `isOverride` via `matchesMarker` | — | `Router.cpp` | −3 |
| 4 | Add `extractString(ISource&)` overload | `Default_Parser.hpp` | `Util.cpp`, `Dictionary.cpp`, `Tag.cpp` | −3 |
| 5 | Add `isInsideFlowContext()` | `Default_Parser.hpp` | `Util.cpp`, `FlowString.cpp`, `YAML_Parser.cpp` | −3 |

**Combined benefit:** Five recurring compound expressions each become a single
named call, making intent explicit and future changes (e.g. adding a new flow
collection type, or changing depth tracking) a single-point edit.

---

## Execution order

All five steps are fully independent — no step depends on any other.

Suggested sequence:

1. **Step 3** — single-line change in one file; zero regression risk.
2. **Step 1** — mechanical `||` → helper replacement; touch 2 files.
3. **Step 5** — replace 3 compound depth checks; touch 3 files.
4. **Step 4** — add overload and update 3 call sites.
5. **Step 2** — extract `keyStopDelimiters`; requires careful re-read of
   `extractKey` to confirm the two usages are truly equivalent before
   collapsing.

Each commit message should reference this plan, e.g.:  
`"DRY Plan 6 Step 1: add isInlineCollection predicate"`
