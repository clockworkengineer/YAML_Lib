# DRY Refactoring Plan 9

## Summary

After Plans 1–8 the parser is largely clean. A full sweep of all source files
reveals **two remaining duplication patterns**:

1. The two-line idiom `if (isComment(source)) { skipLine(source); }` appears
   in **three different files** (`YAML_Parser_Util.cpp`,
   `YAML_Parser_BlockString.cpp`, `YAML_Parser_Dictionary.cpp`).
2. `parseDirective` in `YAML_Parser.cpp` ends every branch of its
   `if / else if / else` chain with the identical `skipLine(source)` call
   — **three copies** of the same trailing statement.

Everything else has been addressed:

| Candidate considered | Reason rejected |
|----------------------|-----------------|
| `parseAlias` / `parseOverride` "resolve + parseFromBuffer" tail | `parseAlias` adds cycle detection; combining adds complexity |
| `parseInlineArray` / `parseInlineDictionary` post-`do-while` block | Different end chars, extra `kColon` check in dictionary version |
| `source.next(); source.ignoreWS()` pairs | Context varies at every call site; no stable semantic unit |
| Parser state reset block in `parse()` | Single-use; per implementation-discipline guidelines |

---

## Step 1 — Extract `skipIfComment` helper

### Problem

The pattern `if (isComment(source)) { skipLine(source); }` appears in three
unrelated functions across three files:

```cpp
// YAML_Parser_Util.cpp — moveToNextIndent
if (isComment(source)) {
    skipLine(source);
} else {
    indentFound = true;
}

// YAML_Parser_BlockString.cpp — scanToFirstBlockContent
if (source.more() && isComment(source)) {
    skipLine(source);
}

// YAML_Parser_Dictionary.cpp — extractMapping
if (!isComment(source)) {
    break;
}
skipLine(source);
```

The two-function sequence has a name — "skip the rest of this line if it is a
comment" — but no single-function expression of it.

### Fix

Add a `skipIfComment` helper that returns `true` when a comment was skipped,
`false` otherwise.  Its boolean return makes each of the three call sites
shorter and clearer:

```cpp
// YAML_Parser_Util.cpp — new definition
bool Default_Parser::skipIfComment(ISource &source) {
  if (isComment(source)) {
    skipLine(source);
    return true;
  }
  return false;
}
```

Call sites become:

```cpp
// moveToNextIndent
if (!skipIfComment(source)) {
    indentFound = true;
}

// scanToFirstBlockContent
if (source.more()) {
    skipIfComment(source);
}

// extractMapping
if (!skipIfComment(source)) {
    break;
}
```

### Files changed

| File | Change |
|------|--------|
| `classes/include/implementation/parser/Default_Parser.hpp` | Add `static bool skipIfComment(ISource &source);` declaration |
| `classes/source/implementation/parser/YAML_Parser_Util.cpp` | Add definition; update `moveToNextIndent` |
| `classes/source/implementation/parser/YAML_Parser_BlockString.cpp` | Update `scanToFirstBlockContent` |
| `classes/source/implementation/parser/YAML_Parser_Dictionary.cpp` | Update `extractMapping` |

### Commit message

```
DRY Plan 9 Step 1: skipIfComment helper replaces inline isComment+skipLine pairs
```

---

## Step 2 — Hoist trailing `skipLine` out of `parseDirective` branches

### Problem

Every branch of the `if / else if / else` in `parseDirective` ends with the
same statement:

```cpp
if (source.match("YAML")) {
    // ...parse %YAML directive...
    skipLine(source);           // ← copy 1
} else if (source.match("TAG")) {
    // ...parse %TAG directive...
    skipLine(source);           // ← copy 2
} else {
    // Unknown directive — skip to end of line
    skipLine(source);           // ← copy 3
}
```

The `skipLine` at the end of each branch is invariant: every directive line,
whether recognised or not, must be consumed before the parser moves on.

### Fix

Remove the identical trailing `skipLine` from all three branches and place a
single call after the entire `if / else if / else` block:

```cpp
if (source.match("YAML")) {
    // ...parse %YAML directive...
} else if (source.match("TAG")) {
    // ...parse %TAG directive...
} else {
    // Unknown directive — YAML spec says warn and ignore
}
skipLine(source);   // always advance past the directive line
```

Throws in the `%YAML` branch (`major != 1`, duplicate directive, bad version)
propagate before reaching `skipLine`, which is the same behaviour as before.

### Files changed

| File | Change |
|------|--------|
| `classes/source/implementation/parser/YAML_Parser.cpp` | Remove 3 `skipLine(source)` calls from branch ends; add one after the `if / else if / else` |

### Commit message

```
DRY Plan 9 Step 2: hoist skipLine out of parseDirective branches
```

---

## Completion state

After these two steps every `isComment → skipLine` idiom will be expressed
through a single named function, and `parseDirective` will carry one
authoritative call to `skipLine` instead of three redundant copies.
