# DRY Refactoring Plan 7 — Parser Layer (continued)

> Prerequisite: Plans 1–6 are fully applied and committed.
> After each step, run the full regression suite
> (`YAML_Lib_Unit_Tests` — "YAML test-suite*" ≥ 715 assertions) before committing.
> Commit messages should reference this plan, e.g. "DRY Plan 7 Step N: …".

---

## Context: what Plans 1–6 have already cleaned up

| Helper / Pattern | Plan |
|------------------|------|
| `extractTrimmed`, `rightTrim`, `matchesMarker`, `isDocumentBoundary` | 1 |
| `passthroughTags` set, `isInlineComment`, `convertOctalToDecimal` | 2 |
| `parseFromBuffer`, `captureIndentedBlock`, `upsertDictEntry`, `extractInlineCollectionAt` | 3 |
| `tryParseToken<>`, `checkFlowDelimiter`, RAII `AliasGuard` | 4 |
| `parseDirective`, `extractMapping` loop, `applyOuter`, `withExtras`, `scanToFirstBlockContent` | 5 |
| `isInlineCollection`, `keyStopDelimiters`, `isOverride→matchesMarker`, `extractString()`, `isInsideFlowContext` | 6 |

---

## Remaining duplication (3 steps)

---

### Step 1 — Extract `addInlineDictEntry` to parallel `addUniqueDictEntry`

#### Pattern

`parseDictionary` uses `addUniqueDictEntry` to add each key/value pair, throwing
on duplicate plain keys.  `parseInlineDictionary` has no equivalent helper: the
same logic — with an extra branch for _complex_ (collection) keys that uses
last-wins semantics — is embedded inline in the `do…while` loop body as an
8-line ad-hoc block:

```cpp
// parseInlineDictionary (YAML_Parser_Dictionary.cpp ~line 315)
const std::string keyStr{entry.getKey()};
auto &dict = NRef<Dictionary>(dictionaryNode);
const bool isComplexKey =
    (!keyStr.empty() && (keyStr.front() == kLeftSquareBracket ||
                         keyStr.front() == kLeftCurlyBrace));
if (dict.contains(keyStr) && !isComplexKey) {
  throw SyntaxError(source.getPosition(),
                    "Dictionary already contains key '" + keyStr + "'.");
} else if (dict.contains(keyStr)) {
  dict[keyStr] = std::move(entry.getNode());
} else {
  dict.add(std::move(entry));
}
```

This is the inline counterpart of `addUniqueDictEntry` with complex-key
awareness, but it is never named – making the intentional difference from
`addUniqueDictEntry` invisible.

#### New helper

```cpp
// Default_Parser.hpp — private declaration (next to addUniqueDictEntry)
static void addInlineDictEntry(Dictionary &dict, DictionaryEntry entry,
                               ISource &source);

// YAML_Parser_Dictionary.cpp — implementation
void Default_Parser::addInlineDictEntry(Dictionary &dict,
                                        DictionaryEntry entry,
                                        ISource &source) {
  const std::string keyStr{entry.getKey()};
  const bool isComplexKey =
      !keyStr.empty() && (keyStr.front() == kLeftSquareBracket ||
                          keyStr.front() == kLeftCurlyBrace);
  if (dict.contains(keyStr) && !isComplexKey) {
    throw SyntaxError(source.getPosition(),
                      "Dictionary already contains key '" + keyStr + "'.");
  } else if (dict.contains(keyStr)) {
    dict[keyStr] = std::move(entry.getNode());
  } else {
    dict.add(std::move(entry));
  }
}
```

#### Site to update

```cpp
// parseInlineDictionary loop body replaces the 8-line block with:
addInlineDictEntry(NRef<Dictionary>(dictionaryNode), std::move(entry), source);
```

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_Dictionary.cpp`.  
**Net change:** −8 lines in the loop body + 12 new lines for the named function;
the important gain is that the two entry-addition strategies (unique-only vs
collection-key-aware) are now both named and symmetrically structured.

---

### Step 2 — Use `skipLine` in `extractMapping`

#### Pattern

`skipLine` was introduced to encapsulate the two-step idiom:

```cpp
moveToNext(source, {kLineFeed});
if (source.more()) { source.next(); }
```

`extractMapping` in `YAML_Parser_Dictionary.cpp` still uses the raw two-liner:

```cpp
// extractMapping (~line 92)
moveToNext(source, {kLineFeed}); // advance to but not past '\n'
source.next(); // consume '\n'; next iteration re-scans spaces
```

The semantics are identical to `skipLine`: advance past `kLineFeed` delimiters
and consume the newline.  (`skipLine` guards `source.next()` with
`source.more()`; the raw version omits the guard but the comment shows it is
always called when `source.more()` is true in this loop context, so behaviour
is the same.)

#### After

```cpp
skipLine(source);   // advance to and consume '\n'
```

**Files touched:** `YAML_Parser_Dictionary.cpp` only.  
**Net change:** −1 line; the site is now consistent with every other
newline-skip call in the parser.

---

### Step 3 — `DepthGuard` RAII for inline collection depth counters

#### Pattern

`parseInlineArray` and `parseInlineDictionary` each manually increment a
static depth counter on entry and decrement it on exit:

```cpp
// parseInlineArray (YAML_Parser_Array.cpp)
inlineArrayDepth++;
// ...do...while loop (can throw)...
inlineArrayDepth--;           // ← NOT reached on exception

// parseInlineDictionary (YAML_Parser_Dictionary.cpp)
inlineDictionaryDepth++;
// ...do...while loop (can throw)...
inlineDictionaryDepth--;      // ← NOT reached on exception
```

The manual `++`/`--` pairing is:
1. **Duplicated** — the increment/decrement idiom is written out twice.
2. **Not exception-safe** — `parseDocument` can throw inside either loop.
   If it does, the depth counter is left permanently incremented, silently
   corrupting all subsequent parses in the same process.

A RAII guard (the canonical C++ solution for paired open/close operations)
fixes both issues in one addition.

#### New inner class

```cpp
// Default_Parser.hpp — private inner class (alongside the existing SourceGuard)
class DepthGuard {
public:
  explicit DepthGuard(long &depth) : depth_(depth) { ++depth_; }
  ~DepthGuard() { --depth_; }
  DepthGuard(const DepthGuard &) = delete;
  DepthGuard &operator=(const DepthGuard &) = delete;
  DepthGuard(DepthGuard &&) = delete;
  DepthGuard &operator=(DepthGuard &&) = delete;

private:
  long &depth_;
};
```

#### Sites to update

```cpp
// parseInlineArray — replace manual ++ / -- with:
DepthGuard depthGuard(inlineArrayDepth);
// (remove: inlineArrayDepth++; and inlineArrayDepth--;)

// parseInlineDictionary — replace manual ++ / -- with:
DepthGuard depthGuard(inlineDictionaryDepth);
// (remove: inlineDictionaryDepth++; and inlineDictionaryDepth--;)
```

The `if (inlineArrayDepth == 0)` / `if (inlineDictionaryDepth == 0)` checks
**after** the loop still read the static member directly and need no change
(the depth has already been decremented by the time the guard destructs at
scope exit, i.e. just before `return`).

**Files touched:** `Default_Parser.hpp` (new inner class),
`YAML_Parser_Array.cpp` (2-line change), `YAML_Parser_Dictionary.cpp` (2-line
change).  
**Net change:** −2 lines across the two call sites; depth tracking is now
exception-safe at zero additional runtime cost.

---

## Summary table

| Step | Change | New symbol | Files touched | Notes |
|------|--------|------------|---------------|-------|
| 1 | Extract `addInlineDictEntry` | `Default_Parser.hpp` | `Dictionary.cpp` | Mirrors `addUniqueDictEntry`; names complex-key semantics |
| 2 | `skipLine` in `extractMapping` | — | `Dictionary.cpp` | −1 line; consistent with every other newline-skip call |
| 3 | `DepthGuard` RAII inner class | `Default_Parser.hpp` | `Array.cpp`, `Dictionary.cpp` | Exception-safe depth tracking; matches existing `SourceGuard` pattern |

**Combined benefit:** Two previously unnamed / exception-unsafe patterns are
named and hardened; the inline-dict entry path gains a named helper that
explicitly communicates the intentional difference from block-dict insertion.

---

## Execution order

All three steps are independent.

Suggested sequence:

1. **Step 2** — single-line change, zero risk.
2. **Step 1** — mechanically lift 8 lines into a named function.
3. **Step 3** — add `DepthGuard`, then remove the two `++`/`--` pairs.

Each commit message should reference this plan, e.g.:  
`"DRY Plan 7 Step 1: extract addInlineDictEntry helper"`
