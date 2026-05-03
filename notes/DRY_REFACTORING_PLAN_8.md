# DRY Refactoring Plan 8

## Summary

After Plans 1–7, the parser is largely clean. A full sweep of every parser
source file reveals **one remaining raw depth-counter `++`/`--` pair**: the
`arrayIndentLevel` counter in `parseArray` (`YAML_Parser_Array.cpp`).

Every other tracked depth counter is already managed by `DepthGuard`:
- `inlineArrayDepth` → `DepthGuard` added in Plan 7 Step 3 (`parseInlineArray`)
- `inlineDictionaryDepth` → `DepthGuard` added in Plan 7 Step 3 (`parseInlineDictionary`)
- `arrayIndentLevel` → **still raw `++`/`--`** ← this plan

No other cross-function duplication was found in the remaining files
(`YAML_Parser_Router.cpp`, `YAML_Parser_FlowString.cpp`,
`YAML_Parser_Scalar.cpp`, `YAML_Parser_BlockString.cpp`,
`YAML_Parser_Timestamp.cpp`, `YAML_Parser_Tag.cpp`,
`Default_Parser.hpp`). Candidates considered and rejected:

| Candidate | Reason rejected |
|-----------|-----------------|
| `extractRawScalar` lambda in `parseTagged` | Used only inside `parseTagged`; already named as a local lambda; one-off usage |
| `AliasGuard` local struct in `parseAlias` | Single-use; operates on `std::set` not a counter; already named and correctly scoped |
| `parseFoldedBlockString` / `parseLiteralBlockString` | Already delegate to `parseBlockString` — clean |
| `isFoldedBlockString` / `isPipedBlockString` | Trivial one-liners; nothing to factor |
| `isDocumentStart` / `isDocumentEnd` | Already delegate to `matchesMarker` — clean |
| `parseAnchor` / `parseAlias` name extraction | Delimiter sets differ; combining would add complexity not reduce it |

---

## Step 1 — Apply `DepthGuard` to `arrayIndentLevel` in `parseArray`

### Problem

`parseArray` still uses a raw manual increment/decrement pair:

```cpp
// YAML_Parser_Array.cpp — parseArray (current)
arrayIndentLevel++;
while (isArray(source) && arrayIndent == source.getPosition().second) {
    // ...calls parseDocument which can throw...
}
arrayIndentLevel--;
if (isArray(source) && arrayIndentLevel == 0 &&
    arrayIndent > source.getPosition().second) {
    throw SyntaxError(...);
}
```

If `parseDocument` throws, `arrayIndentLevel--` is never reached, leaving
the counter permanently incremented — the same exception-safety bug that
`DepthGuard` was introduced to fix in `parseInlineArray` and
`parseInlineDictionary`.

### Fix

Wrap the `while` loop in a scoped block and replace the raw `++`/`--` with a
`DepthGuard`, exactly as was done for `parseInlineArray` (Plan 7 Step 3 fix):

```cpp
// YAML_Parser_Array.cpp — parseArray (after)
{
    DepthGuard depthGuard(arrayIndentLevel);
    while (isArray(source) && arrayIndent == source.getPosition().second) {
        // ...
    }
} // arrayIndentLevel decremented here (even on exception)
if (isArray(source) && arrayIndentLevel == 0 &&
    arrayIndent > source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Invalid indentation for array element.");
}
```

### Files changed

| File | Change |
|------|--------|
| `classes/source/implementation/parser/YAML_Parser_Array.cpp` | Replace raw `arrayIndentLevel++` / `arrayIndentLevel--` with `DepthGuard depthGuard(arrayIndentLevel)` in a scoped block |

No header change is needed — `DepthGuard` is already declared in
`Default_Parser.hpp`.

### Commit message

```
DRY Plan 8 Step 1: DepthGuard for arrayIndentLevel in parseArray
```

---

## Completion state

After this single step, all three parser depth counters
(`arrayIndentLevel`, `inlineArrayDepth`, `inlineDictionaryDepth`) will be
managed exclusively through `DepthGuard` — no raw `++`/`--` pairs remain.
