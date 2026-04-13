# DRY Refactoring Plan 10

## Summary

After Plans 1–9, a full re-sweep of all 11 parser source files reveals **one
remaining structural duplication**: the three-line block that closes an inline
collection (call `source.ignoreWS()`, then conditionally call
`checkFlowDelimiter`) appears identically in `parseInlineArray` and
`parseInlineDictionary`, with only the depth counter differing.

No other duplication was found at this stage.  Candidates considered and
rejected:

| Candidate | Reason rejected |
|-----------|-----------------|
| `if (unparsed.empty()) { return Null; } return parseFromBuffer(...)` in `parseAnchor` / `parseAlias` | `parseAlias` has cycle-detection lines between the two statements; a helper would only apply to `parseAnchor` — single use |
| `if (source.more()) { source.next(); }` in `skipLine` / `parseComment` | Two-line expression of a trivial guard; extracting it adds a layer with no conceptual gain |
| `parseKeyValue` / `parseInlineKeyValue` both call `parseKey` | Logic after key extraction differs completely; no common sub-task |
| `yNodeTree.push_back(Node::make<Document>())` in `parse()` | Surrounding context differs; the three sites are not the same operation |

---

## Step 1 — Extract `checkAtFlowClose` helper

### Problem

The closing block of `parseInlineArray` (`YAML_Parser_Array.cpp`):

```cpp
checkForEnd(source, kRightSquareBracket);
source.ignoreWS();                          // ← duplicated
if (inlineArrayDepth == 0) {                // ← duplicated
    checkFlowDelimiter(source, delimiters); // ← duplicated
}
```

And the closing block of `parseInlineDictionary` (`YAML_Parser_Dictionary.cpp`):

```cpp
checkForEnd(source, kRightCurlyBrace);
if (source.current() == kColon) { throw SyntaxError(...); }
source.ignoreWS();                             // ← duplicated
if (inlineDictionaryDepth == 0) {              // ← duplicated
    checkFlowDelimiter(source, delimiters);    // ← duplicated
}
```

Both functions end with the same semantic operation — "now that the closing
bracket has been consumed, skip whitespace and, if this was the outermost
inline collection, verify the next character is a valid delimiter" — expressed
as the same three lines with different depth-counter names.

### Fix

Add a `checkAtFlowClose` helper that takes the post-decrement depth value:

```cpp
// YAML_Parser_Util.cpp — new definition
void Default_Parser::checkAtFlowClose(ISource &source,
                                      const Delimiters &delimiters,
                                      const long depth) {
  source.ignoreWS();
  if (depth == 0) {
    checkFlowDelimiter(source, delimiters);
  }
}
```

Call sites become single lines:

```cpp
// parseInlineArray:
checkForEnd(source, kRightSquareBracket);
checkAtFlowClose(source, delimiters, inlineArrayDepth);

// parseInlineDictionary:
checkForEnd(source, kRightCurlyBrace);
if (source.current() == kColon) { throw SyntaxError(...); }
checkAtFlowClose(source, delimiters, inlineDictionaryDepth);
```

### Files changed

| File | Change |
|------|--------|
| `classes/include/implementation/parser/Default_Parser.hpp` | Add `static void checkAtFlowClose(ISource &source, const Delimiters &delimiters, long depth);` |
| `classes/source/implementation/parser/YAML_Parser_Util.cpp` | Add `checkAtFlowClose` definition |
| `classes/source/implementation/parser/YAML_Parser_Array.cpp` | Replace 3-line idiom with single call |
| `classes/source/implementation/parser/YAML_Parser_Dictionary.cpp` | Replace 3-line idiom with single call |

### Commit message

```
DRY Plan 10 Step 1: checkAtFlowClose helper replaces duplicated ignoreWS+checkFlowDelimiter idiom
```

---

## Completion state

After this step, every inline-collection close path will share one named
expression of the "skip whitespace and validate outermost flow delimiter"
rule.  At that point the parser contains no further mechanical duplication
addressable without increasing complexity beyond the benefit gained.
