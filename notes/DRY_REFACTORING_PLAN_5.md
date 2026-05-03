# DRY Refactoring Plan 5 — Parser Layer (continued)

> Prerequisite: Plans 1–4 are fully applied and committed.
> After each step, run the full regression suite
> (`YAML_Lib_Unit_Tests` — ≥ 58 tests / ≥ 655 assertions) before committing.
> Commit messages should reference this plan, e.g. "DRY Plan 5 Step N: …".

---

## Context: what Plans 1–4 have already cleaned up

| Helper / Pattern | File | Replaced |
|------------------|------|---------|
| `extractTrimmed` | `YAML_Parser_Util.cpp` | `extractToNext` + `rightTrim` |
| `matchesMarker` | `YAML_Parser_Router.cpp` | `SourceGuard` + `source.match` |
| `isDocumentBoundary` | `YAML_Parser_Router.cpp` | `isDocumentStart(s)\|\|isDocumentEnd(s)` |
| `passthroughTags` set | `YAML_Parser_Tag.cpp` | 4 identical `!!seq/map/omap/pairs` branches |
| `isInlineComment` | `YAML_Parser_FlowString.cpp` | inline `#`-preceded-by-WS predicate |
| `convertOctalToDecimal` | `YAML_Parser_Scalar.cpp` | duplicated `stoll(...,8)` blocks |
| `parseFromBuffer` | `YAML_Parser_Util.cpp` | `BufferSource{x}` + `parseDocument` in anchor/alias/override |
| `captureIndentedBlock` | `YAML_Parser_Util.cpp` | manual while-loop in `parseAnchor` |
| `upsertDictEntry` | `YAML_Parser_Util.cpp` (impl) | insert-or-overwrite pattern in `mergeOverrides` |
| `extractInlineCollectionAt` | `YAML_Parser_Dictionary.cpp` | dual `isInlineDict/Array` + `extractInLine` dispatch |
| `tryParseToken<>` | `Default_Parser.hpp` | `SourceGuard` + extract + trim + predicate in `parseNumber`/`parseNone`/`parseBoolean`/`parseTimestamp` |
| `checkFlowDelimiter` | `YAML_Parser_Util.cpp` | duplicate post-close delimiter check in `parseInlineArray`/`parseInlineDictionary` |
| RAII `AliasGuard` | `YAML_Parser_Directive.cpp` | try/catch duplicating `activeAliasExpansions.erase` |

---

## Remaining duplication (5 steps)

---

### Step 1 — Extract `parseDirective` helper from `parse()`

#### Pattern

`Default_Parser::parse()` (`YAML_Parser.cpp`) contains ~35 lines of inline directive
parsing embedded in the main document-scanning loop.  The `%YAML` and `%TAG` branches
are functionally independent of the loop logic: they either update parser state
(`yamlDirectiveMinor`, `yamlTagPrefixes`) or throw.  Their presence inflates `parse()`
from an ~18-line dispatch loop to a ~55-line function that mixes concerns.

#### Current code (abridged, `YAML_Parser.cpp`)

```cpp
std::vector<Node> Default_Parser::parse(ISource &source) {
  // ... reset state ...
  for (bool inDocument = false; source.more();) {
    if (isDirective(source)) {
      if (inDocument) {
        throw SyntaxError(source.getPosition(),
                          "Directives must appear before document start.");
      }
      source.next(); // consume '%'
      if (source.match("YAML")) {
        source.ignoreWS();
        std::string version{extractToNext(source, {kLineFeed, ' '})};
        const auto dot = version.find('.');
        if (dot == std::string::npos) {
          throw SyntaxError(source.getPosition(),
                            "%YAML directive missing version number.");
        }
        const int major = std::stoi(version.substr(0, dot));
        const int minor = std::stoi(version.substr(dot + 1));
        if (major != 1) {
          throw SyntaxError(source.getPosition(), "%YAML directive: ...");
        }
        if (yamlDirectiveSeen) {
          throw SyntaxError(source.getPosition(), "%YAML directive appears more than once ...");
        }
        yamlDirectiveSeen = true;
        yamlDirectiveMinor = minor;
        skipLine(source);
      } else if (source.match("TAG")) {
        source.ignoreWS();
        std::string handle{extractToNext(source, {' '})};
        source.ignoreWS();
        std::string prefix{extractToNext(source, {kLineFeed, ' '})};
        yamlTagPrefixes[handle] = prefix;
        skipLine(source);
      } else {
        skipLine(source); // unknown directive – skip
      }
    } else if (isDocumentStart(source)) {
    // ...
    }
  }
}
```

#### After

```cpp
// ── New private static in Default_Parser.hpp ──
static void parseDirective(ISource &source, bool inDocument);

// ── New implementation in YAML_Parser.cpp ──
void Default_Parser::parseDirective(ISource &source, const bool inDocument) {
  if (inDocument) {
    throw SyntaxError(source.getPosition(),
                      "Directives must appear before document start.");
  }
  source.next(); // consume '%'
  if (source.match("YAML")) {
    source.ignoreWS();
    std::string version{extractToNext(source, {kLineFeed, ' '})};
    const auto dot = version.find('.');
    if (dot == std::string::npos) {
      throw SyntaxError(source.getPosition(),
                        "%YAML directive missing version number.");
    }
    const int major = std::stoi(version.substr(0, dot));
    const int minor = std::stoi(version.substr(dot + 1));
    if (major != 1) {
      throw SyntaxError(source.getPosition(),
                        "%YAML directive: unsupported major version " +
                            std::to_string(major) + ".");
    }
    if (yamlDirectiveSeen) {
      throw SyntaxError(
          source.getPosition(),
          "%YAML directive appears more than once for the same document.");
    }
    yamlDirectiveSeen = true;
    yamlDirectiveMinor = minor;
    skipLine(source);
  } else if (source.match("TAG")) {
    source.ignoreWS();
    std::string handle{extractToNext(source, {' '})};
    source.ignoreWS();
    std::string prefix{extractToNext(source, {kLineFeed, ' '})};
    yamlTagPrefixes[handle] = prefix;
    skipLine(source);
  } else {
    skipLine(source);
  }
}

// ── parse() loop reduces to ──
for (bool inDocument = false; source.more();) {
  if (isDirective(source)) {
    parseDirective(source, inDocument);
  } else if (isDocumentStart(source)) {
    // ...
  }
  // ...
}
```

**Files touched:** `YAML_Parser.cpp`, `Default_Parser.hpp`.  
**Net change:** `parse()` shrinks from ~55 to ~20 lines; directive logic is
testable and named.

---

### Step 2 — Replace `extractMapping` tail-recursion with a `while` loop

#### Pattern

`extractMapping` (`YAML_Parser_Dictionary.cpp`) skips comment lines by recursing:

```cpp
std::string Default_Parser::extractMapping(ISource &source) {
  std::string key;
  source.next();
  while (source.more() && source.current() == kSpace) { ... }
  if (isComment(source)) {
    moveToNext(source, {kLineFeed});
    return extractMapping(source);   // ← tail-recursion
  }
  // ...
}
```

This is unbounded: a YAML document with N consecutive comment lines inside an
explicit-key `?` value causes N stack frames.  `extractMapping` is already
called indirectly from `isKey` (which is called inside `parseDictionary`'s
loop), so the call depth can compound further.

The recursion is trivially avoidable because it is tail-recursive and the only
work the recursive call does is re-run the same preamble.  A standard
"loop-until-not-a-comment" idiom replaces it exactly.

#### After

```cpp
std::string Default_Parser::extractMapping(ISource &source) {
  std::string key;
  do {
    key.clear();
    source.next();
    while (source.more() && source.current() == kSpace) {
      key += kSpace;
      source.next();
    }
  } while (isComment(source) &&
           (moveToNext(source, {kLineFeed}), true)); // skip comment, retry
```

Or equivalently (more readable):

```cpp
std::string Default_Parser::extractMapping(ISource &source) {
  while (true) {
    std::string key;
    source.next();
    while (source.more() && source.current() == kSpace) {
      key += kSpace;
      source.next();
    }
    if (isComment(source)) {
      moveToNext(source, {kLineFeed}); // consume comment to EOL; loop again
      continue;
    }
    if (isInlineDictionary(source) || isInlineArray(source)) {
      key += extractInlineCollectionAt(source);
      moveToNext(source, {kColon});
    } else if (isArray(source)) {
      key += extractToNext(source, {kColon});
    } else {
      key += extractToNext(source, {kLineFeed});
      key += kColon;
    }
    return key;
  }
}
```

**Files touched:** `YAML_Parser_Dictionary.cpp` only (no header change needed).  
**Net change:** 0 lines (same count) but recursion eliminated; no stack overflow
risk for pathological input.

---

### Step 3 — Deduplicate the `overrideKeys` upsert loop in `mergeOverrides`

#### Pattern

`mergeOverrides` (`YAML_Parser_Directive.cpp`) handles two cases: single-alias
merge (`<<: *alias`) and multi-alias merge (`<<: [*a, *b, ...]`).  Both cases end
with the **identical** 3-line loop that applies explicit outer keys over the base:

```cpp
// Single-alias branch:
for (auto &entry : overrideKeys) {
  auto overrideEntry = mergeOverrides(dictionary[entry]);   // ← same
  upsertDictEntry(innerDictionary, entry, std::move(overrideEntry));  // ← same
}
overrideRoot = std::move(overrideValue);

// Multi-alias branch:
for (auto &entry : overrideKeys) {
  auto overrideEntry = mergeOverrides(dictionary[entry]);   // ← same
  upsertDictEntry(mergedDict, entry, std::move(overrideEntry));       // ← same
}
overrideRoot = std::move(mergedBase);
```

The target dictionary node differs (`innerDictionary` vs `mergedDict`) but the
loop body is word-for-word identical.  Extract a private static lambda or a
small helper.

#### After — local lambda inside `mergeOverrides`

```cpp
Node Default_Parser::mergeOverrides(Node &overrideRoot) {
  if (isA<Dictionary>(overrideRoot) &&
      NRef<Dictionary>(overrideRoot).contains(kOverride)) {
    auto &dictionary = NRef<Dictionary>(overrideRoot);
    std::set<std::string> overrideKeys;
    for (auto &entry : dictionary.value()) {
      if (entry.getKey() != kOverride) {
        overrideKeys.insert(std::string(entry.getKey()));
      }
    }

    // Shared: apply explicit outer keys on top of a base dictionary.
    const auto applyOuter = [&](Dictionary &base) {
      for (auto &entry : overrideKeys) {
        auto merged = mergeOverrides(dictionary[entry]);
        upsertDictEntry(base, entry, std::move(merged));
      }
    };

    Node &overrideValue = dictionary[kOverride];
    if (isA<Dictionary>(overrideValue)) {
      applyOuter(NRef<Dictionary>(overrideValue));
      overrideRoot = std::move(overrideValue);
    } else if (isA<Array>(overrideValue)) {
      auto mergedBase = Node::make<Dictionary>();
      auto &mergedDict = NRef<Dictionary>(mergedBase);
      for (auto &element : NRef<Array>(overrideValue).value()) {
        if (!isA<Dictionary>(element)) {
          throw SyntaxError(
              "Merge key '<<' sequence must contain only mappings.");
        }
        for (auto &entry : NRef<Dictionary>(element).value()) {
          const std::string key{entry.getKey()};
          if (!mergedDict.contains(key)) {
            mergedDict.add(DictionaryEntry(key, entry.getNode()));
          }
        }
      }
      applyOuter(mergedDict);
      overrideRoot = std::move(mergedBase);
    }
  }
  return std::move(overrideRoot);
}
```

**Files touched:** `YAML_Parser_Directive.cpp` only (no header change needed).  
**Net change:** `−5` lines; the `applyOuter` lambda makes the "overlay logic"
a named, single-source concept.

---

### Step 4 — Add `Delimiters withExtras(base, {…})` helper to unify delimiter merging

#### Pattern

The 2-line "copy base delimiters then insert additional stops" idiom appears in
three places:

```cpp
// parseAnchor (YAML_Parser_Directive.cpp)
Delimiters inlineStop = delimiters;
inlineStop.insert(kLineFeed);

// parseInlineDictionary (YAML_Parser_Dictionary.cpp)
Delimiters inLineDictionaryDelimiters = {delimiters};
inLineDictionaryDelimiters.insert({kComma, kRightCurlyBrace});

// parseInlineArray (YAML_Parser_Array.cpp)
Delimiters inLineArrayDelimiters = {delimiters};
inLineArrayDelimiters.insert({kComma, kRightSquareBracket});
```

All three follow the same shape: name a copy, mutate it, pass it downstream.
A simple static helper makes the intent read left-to-right and eliminates the
mutation:

#### New helper

```cpp
// Default_Parser.hpp — private declaration (near checkFlowDelimiter)
static Delimiters withExtras(const Delimiters &base,
                             std::initializer_list<char> extras);

// YAML_Parser_Util.cpp — implementation
Delimiters Default_Parser::withExtras(const Delimiters &base,
                                      std::initializer_list<char> extras) {
  Delimiters result{base};
  result.insert(extras);
  return result;
}
```

#### Sites to update

```cpp
// parseAnchor:
const auto &inlineStop = withExtras(delimiters, {kLineFeed});

// parseInlineDictionary:
const auto &inLineDictionaryDelimiters =
    withExtras(delimiters, {kComma, kRightCurlyBrace});

// parseInlineArray:
const auto &inLineArrayDelimiters =
    withExtras(delimiters, {kComma, kRightSquareBracket});
```

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_Util.cpp`,
`YAML_Parser_Directive.cpp`, `YAML_Parser_Dictionary.cpp`,
`YAML_Parser_Array.cpp`.  
**Net change:** `−3` lines; mutation removed; delimiter composition is explicit
and named.

---

### Step 5 — Extract `scanToFirstBlockContent` for W9L4 blank-line validation

#### Context

Test W9L4 requires that a block scalar (`|` or `>`) throws a `SyntaxError` when
a blank line before the first content line has more leading spaces than the
block's indentation level.  The fix requires replacing the current single call to
`moveToNextIndent` in `parseBlockString` with a custom pre-content scan that:

1. skips blank lines (recording the maximum leading-space count seen),
2. stops at the first non-blank, non-EOF line (the content line), and
3. validates the recorded max against `blockIndent`.

That scan logic is ~30 lines when written inline.  Placing it inline in
`parseBlockString` mixes parsing mechanics with W9L4 error detection, and makes
`parseBlockString` noticeably harder to read.  Extract it as a named helper.

#### New helper

```cpp
// Default_Parser.hpp — private declaration
/// Advance source past blank lines and whitespace to the first non-blank
/// content line of a block scalar.  Populates `maxBlankSpaces` with the
/// maximum leading-space count seen on any blank line encountered.
/// Returns true if a content line was found (false if EOF / all blank).
/// Throws SyntaxError if a leading tab is found at line start.
static bool scanToFirstBlockContent(ISource &source,
                                    unsigned long &maxBlankSpaces);

// YAML_Parser_BlockString.cpp — implementation
bool Default_Parser::scanToFirstBlockContent(ISource &source,
                                             unsigned long &maxBlankSpaces) {
  maxBlankSpaces = 0;
  while (source.more()) {
    if (source.current() == kLineFeed) {
      source.next();
      continue;
    }
    // Count leading spaces on this line.
    unsigned long spaces = 0;
    while (source.more() && source.current() == kSpace) {
      ++spaces;
      source.next();
    }
    if (!source.more() || source.current() == kLineFeed) {
      // Blank line (only spaces, then newline or EOF).
      maxBlankSpaces = std::max(maxBlankSpaces, spaces);
      continue; // kLineFeed consumed on next outer iteration
    }
    if (source.current() == '\t') {
      throw SyntaxError(source.getPosition(),
                        "Tab character not allowed in YAML block indentation.");
    }
    // First real content character found.  Source is positioned at it.
    return true;
  }
  return false; // reached EOF without finding content
}
```

#### `parseBlockString` after the W9L4 fix + this extraction

```cpp
std::string Default_Parser::parseBlockString(ISource &source,
                                             const Delimiters &delimiters,
                                             unsigned long indentation,
                                             const char fillerDefault) {
  const auto [chomping, explicitIndent] = parseBlockChomping(source);
  if (source.more() && isComment(source)) {
    throw SyntaxError(source.getPosition(),
                      "Block scalar comment must be preceded by whitespace.");
  }
  moveToNext(source, delimiters);

  unsigned long maxBlankSpaces = 0;
  const bool hasContent = scanToFirstBlockContent(source, maxBlankSpaces);

  const unsigned long blockIndent =
      (explicitIndent > 0)
          ? indentation + static_cast<unsigned long>(explicitIndent)
          : source.getPosition().second;

  // YAML 1.2: blank lines may not have more leading spaces than blockIndent.
  if (hasContent && maxBlankSpaces > blockIndent - 1) {
    throw SyntaxError(
        source.getPosition(),
        "Block scalar blank line has more spaces than block indentation.");
  }

  // ... rest of content loop unchanged ...
}
```

`parseBlockString` stays at the same line count as before the fix (the inline
~30-line scan is entirely in the helper), and the validation rule is named.

**Files touched:** `Default_Parser.hpp`, `YAML_Parser_BlockString.cpp`.  
**Net change for `parseBlockString`:** 0 lines vs the fully-inline version; `+1`
clearly-named concept; `scanToFirstBlockContent` is independently testable and
reusable if block scalars are parsed in other contexts in future.

**Important:** This step depends on first stabilising the W9L4 fix (the
`scanToFirstBlockContent` logic must pass R4YG, P2AD, XW4D, and JEF9 without
regression).  Resolve those regressions before extracting the helper — the helper
boundary makes the regression easier to identify and fix.

---

## Summary table

| Step | Change | New declaration | Files touched | Δ lines |
|------|--------|-----------------|---------------|---------|
| 1 | Extract `parseDirective` from `parse()` loop | `Default_Parser.hpp` | `YAML_Parser.cpp` | −30 in `parse()` |
| 2 | Replace `extractMapping` tail-recursion with loop | — | `YAML_Parser_Dictionary.cpp` | 0 (quality) |
| 3 | Deduplicate `overrideKeys` upsert loop in `mergeOverrides` | — | `YAML_Parser_Directive.cpp` | −5 |
| 4 | Add `withExtras` delimiter merge helper | `Default_Parser.hpp` | `Util.cpp`, `Directive.cpp`, `Dictionary.cpp`, `Array.cpp` | −3 |
| 5 | Extract `scanToFirstBlockContent` for W9L4 fix | `Default_Parser.hpp` | `YAML_Parser_BlockString.cpp` | 0 (clarity) |

**Combined benefit:** `parse()` is significantly more readable; the only recursive
parser function becomes a safe loop; two duplication hot-spots consolidate; and the
W9L4 blank-line rule is encapsulated in a testable named unit.

---

## Execution order

Steps 1–4 are fully independent of each other and of step 5.

Suggested sequence:

1. **Step 2** first — recursion elimination is a correctness risk (stack safety) and
   is the lowest-risk mechanical change.
2. **Step 3** — pure refactor inside one function; very low regression risk.
3. **Step 4** — mechanical copy→helper substitution; touch 4 files but each change
   is a 2→1 line replacement.
4. **Step 1** — extract `parseDirective`; verify `%YAML`/`%TAG` directive tests pass
   (tests `5TYM`, `6LVF` in the suite).
5. **Step 5 last** — depends on W9L4 being fixed and stable first.

Each commit message should reference this plan, e.g.:  
`"DRY Plan 5 Step 1: extract parseDirective from parse() loop"`
