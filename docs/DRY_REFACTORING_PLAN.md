# Parser DRY Refactoring Plan

Analysis of all eleven source files under
`classes/source/implementation/parser/` and the single header
`classes/include/implementation/parser/Default_Parser.hpp`.

---

## Identified Repetitions

### R1 — `source.save()` / `source.restore()` RAII pair (13 sites)

Every predicate that needs lookahead manually pairs a `save()` with a
`restore()`.  If an exception fires between the two the restore is skipped,
and the exact same boilerplate appears in every function.

**Files / lines affected:**
| File | Occurrences |
|------|-------------|
| `YAML_Parser_Router.cpp` | 7 (lines 20, 31, 68, 190, 209, 220 + isMapping) |
| `YAML_Parser_Scalar.cpp` | 3 (lines 25, 106, 129) |
| `YAML_Parser_Timestamp.cpp` | 2 (lines 22, 65) |
| `YAML_Parser_Util.cpp` | 1 (line 37) |
| `YAML_Parser_Dictionary.cpp` | 1 (line 109) |

**Root cause:** No RAII wrapper over `ISource::save()`/`restore()`.

---

### R2 — `moveToNext(source, {kLineFeed})` + `if (source.more()) source.next()` (5 sites)

This two-statement idiom means "discard the rest of the current line and
consume the newline itself."  It appears as copy-pasted code:

```cpp
moveToNext(source, {kLineFeed});
if (source.more()) {
    source.next();
}
```

**Files / lines affected:**
- `YAML_Parser.cpp` lines 81, 92, 98, 111  
- `YAML_Parser_Util.cpp` line 100 (inside `moveToNextIndent`)

---

### R3 — Save → extract → rightTrim → check → conditional restore pattern (3 sites)

`parseNumber`, `parseNone`, and `parseBoolean` in `YAML_Parser_Scalar.cpp`
all share the identical scaffolding:

```cpp
Node xxxNode;
source.save();
std::string token{extractToNext(source, delimiters)};
rightTrim(token);
// ... token-specific check ...
if (xxxNode.isEmpty()) {
    source.restore();
}
return xxxNode;
```

The only difference between the three functions is the token-classification
logic in the middle.

---

### R4 — Duplicate-key guard repeated in block and inline dictionary (2 sites)

```cpp
if (NRef<Dictionary>(dictionaryNode).contains(entry.getKey())) {
    throw SyntaxError(source.getPosition(),
                      "Dictionary already contains key '" +
                          std::string(entry.getKey()) + "'.");
}
NRef<Dictionary>(dictionaryNode).add(std::move(entry));
```

Appears verbatim in both `parseDictionary` (line 224) and
`parseInlineDictionary` (line 269) in `YAML_Parser_Dictionary.cpp`.

---

### R5 — Undefined-alias lookup + throw duplicated (2 sites)

```cpp
if (!yamlAliasMap.count(name)) {
    throw SyntaxError(source.getPosition(),
                      "Undefined alias '" + name + "'.");
}
const std::string unparsed{yamlAliasMap[name]};
```

Copy-pasted between `parseAlias` (line 154) and `parseOverride` (line 196)
in `YAML_Parser_Directive.cpp`.

---

### R6 — "Empty unquoted string" predicate duplicated (2 sites)

```cpp
isA<String>(node) &&
NRef<String>(node).value().empty() &&
NRef<String>(node).getQuote() == kNull
```

Used in `parseInlineArray` (`YAML_Parser_Array.cpp` line 67) and
`parseInlineKeyValue` (`YAML_Parser_Dictionary.cpp` line 201) to detect a
trailing-comma null element.

---

### R7 — Tag-coercion parse-or-throw block repeated 4× (in `YAML_Parser_Tag.cpp`)

For `!!int`, `!!float`, `!!bool`, `!!null` the code repeats the same
six-line structure:

```cpp
if (isQuotedString(source)) {
    const std::string raw = extractRawScalar();
    BufferSource bs{raw + "\n"};
    result = parseXxx(bs, {kLineFeed}, indentation);
} else {
    result = parseXxx(source, delimiters, indentation);
}
if (result.isEmpty()) {
    throw SyntaxError(source.getPosition(),
                      "Value cannot be parsed as !!xxx.");
}
```

Only `parseXxx` and the error string differ.

---

### R8 — `isDocumentStart` / `isDocumentEnd` share identical structure (2 sites)

```cpp
bool Default_Parser::isDocumentStart(ISource &source) {
    source.save();
    const bool isStart{source.match(kStartDocument)};
    source.restore();
    return isStart;
}
bool Default_Parser::isDocumentEnd(ISource &source) {
    source.save();
    const bool isEnd{source.match(kEndDocument)};
    source.restore();
    return isEnd;
}
```

Both functions differ only in the marker string they test.

---

### R9 — ISO-8601 date prefix validation duplicated between `isTimestamp` and `parseTimestamp` (2 sites, `YAML_Parser_Timestamp.cpp`)

The exact same 10-character lookahead pattern (4 digits, `-`, 2 digits, `-`,
2 digits) is copy-pasted in both the `isTimestamp` predicate (lines 35–47)
and the `parseTimestamp` parser (lines 68–78).

---

## Concrete Refactoring Steps

The steps are ordered from smallest/safest to largest impact.  Each is
self-contained and can be reviewed independently.

---

### Step 1 — Introduce `SourceGuard` RAII type (addresses R1, R8 partially)

**What:** Add a lightweight RAII scope guard inside
`Default_Parser.hpp` (or a new header `YAML_Parser_Guard.hpp`):

```cpp
class SourceGuard {
public:
    explicit SourceGuard(ISource &src) : src_(src) { src_.save(); }
    ~SourceGuard() { src_.restore(); }
    // non-copyable, non-movable
    SourceGuard(const SourceGuard &) = delete;
    SourceGuard &operator=(const SourceGuard &) = delete;
private:
    ISource &src_;
};
```

**Usage — before:**
```cpp
source.save();
const bool isStart{source.match(kStartDocument)};
source.restore();
return isStart;
```

**Usage — after:**
```cpp
SourceGuard guard(source);
return source.match(kStartDocument);
```

**Impact:** Eliminates 13 manual `source.restore()` calls and makes all
lookahead predicates exception-safe.  Touches 5 files; every change is a
mechanical substitution.

---

### Step 2 — Extract `skipLine(ISource&)` helper (addresses R2)

**What:** Add a private static helper:

```cpp
static void skipLine(ISource &source) {
    moveToNext(source, {kLineFeed});
    if (source.more()) {
        source.next();
    }
}
```

**Before (repeated 5×):**
```cpp
moveToNext(source, {kLineFeed});
if (source.more()) {
    source.next();
}
```

**After:**
```cpp
skipLine(source);
```

**Impact:** Remove 10 lines of duplication spread across `YAML_Parser.cpp`
(4 sites) and `YAML_Parser_Util.cpp` (1 site).  The new helper is declared
in `Default_Parser.hpp` alongside `moveToNext`.

---

### Step 3 — Extract `tryParseToken` helper template (addresses R3)

**What:** Factor the save/extract/trim/check/restore scaffold into a single
function template:

```cpp
template <typename Predicate>
static Node tryParseToken(ISource &source, const Delimiters &delimiters,
                          Predicate &&pred) {
    SourceGuard guard(source);   // uses Step 1
    std::string token{extractToNext(source, delimiters)};
    rightTrim(token);
    Node result = pred(token, source);
    if (!result.isEmpty()) {
        guard.release();          // SourceGuard needs an opt-in `release()`
    }
    return result;
}
```

Alternatively, add a `release()` method to `SourceGuard` that prevents
the destructor from calling `restore()`.

**Before (`parseNone`, `parseBoolean`, `parseNumber` — 30+ lines each of
identical scaffolding):**  Each function manually saves, extracts, checks,
and conditionally restores.

**After (`parseNone` example):**
```cpp
Node Default_Parser::parseNone(ISource &source, const Delimiters &delimiters,
                               unsigned long /*indentation*/) {
    return tryParseToken(source, delimiters, [](const std::string &tok, ISource &) -> Node {
        if (tok == "null" || tok == "~") return Node::make<Null>();
        return {};
    });
}
```

**Impact:** Reduces each of the three scalar parsers to a small lambda.
Ensures consistent save/restore behaviour across all three.

---

### Step 4 — Extract `addUniqueDictEntry` helper (addresses R4)

**What:** Add a private static helper:

```cpp
static void addUniqueDictEntry(Node &dictionaryNode, DictionaryEntry entry,
                               ISource &source) {
    if (NRef<Dictionary>(dictionaryNode).contains(entry.getKey())) {
        throw SyntaxError(source.getPosition(),
                          "Dictionary already contains key '" +
                              std::string(entry.getKey()) + "'.");
    }
    NRef<Dictionary>(dictionaryNode).add(std::move(entry));
}
```

**Usage:** Replace the two identical guard+add blocks in `parseDictionary`
and `parseInlineDictionary` with a single call.

**Impact:** Single definition of the duplicate-key error message; both parse
paths kept in sync automatically.

---

### Step 5 — Extract `resolveAlias(name, source)` helper (addresses R5)

**What:**

```cpp
static const std::string &resolveAlias(const std::string &name,
                                       ISource &source) {
    if (!yamlAliasMap.count(name)) {
        throw SyntaxError(source.getPosition(),
                          "Undefined alias '" + name + "'.");
    }
    return yamlAliasMap[name];
}
```

Replace the four lines duplicated in `parseAlias` and `parseOverride` with
`const std::string &unparsed = resolveAlias(name, source);`.

**Impact:** One definition of the alias-not-found error; consistent lookup.

---

### Step 6 — Extract `isNullStringNode` predicate (addresses R6)

**What:**

```cpp
static bool isNullStringNode(const Node &node) {
    return isA<String>(node) &&
           NRef<String>(node).value().empty() &&
           NRef<String>(node).getQuote() == kNull;
}
```

**Usage:** Replace the repeated three-part boolean in `parseInlineArray` and
`parseInlineKeyValue`.

**Impact:** Semantic intent is named; both call-sites updated consistently.

---

### Step 7 — Collapse tag-coercion branches into a dispatch table (addresses R7)

**What:** In `YAML_Parser_Tag.cpp`, replace the four near-identical
`!!int` / `!!float` / `!!bool` / `!!null` branches with:

```cpp
using CoerceFunc = std::function<Node(ISource &, const Delimiters &,
                                      unsigned long)>;
static const std::unordered_map<std::string, std::pair<CoerceFunc, const char *>>
    coercions{
        {"int",   {parseNumber,  "!!int"}},
        {"float", {parseNumber,  "!!float"}},
        {"bool",  {parseBoolean, "!!bool"}},
        {"null",  {parseNone,    "!!null"}},
    };

auto coerceTaggedScalar = [&](CoerceFunc fn, const char *tagName) {
    if (isQuotedString(source)) {
        const std::string raw = extractRawScalar();
        BufferSource bs{raw + "\n"};
        result = fn(bs, {kLineFeed}, indentation);
    } else {
        result = fn(source, delimiters, indentation);
    }
    if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          std::string("Value cannot be parsed as ") +
                              tagName + ".");
    }
};

if (auto it = coercions.find(tagSuffix); it != coercions.end()) {
    coerceTaggedScalar(it->second.first, it->second.second);
}
```

**Impact:** The four repetitive branches shrink to a single dispatch; adding
a new coercion type is one table entry.

---

### Step 8 — Extract `looksLikeIso8601Date` helper (addresses R9)

**What:**

```cpp
static bool looksLikeIso8601Date(const std::string &s) {
    return s.size() >= 10 &&
           std::isdigit(static_cast<unsigned char>(s[0])) &&
           std::isdigit(static_cast<unsigned char>(s[1])) &&
           std::isdigit(static_cast<unsigned char>(s[2])) &&
           std::isdigit(static_cast<unsigned char>(s[3])) &&
           s[4] == '-' &&
           std::isdigit(static_cast<unsigned char>(s[5])) &&
           std::isdigit(static_cast<unsigned char>(s[6])) &&
           s[7] == '-' &&
           std::isdigit(static_cast<unsigned char>(s[8])) &&
           std::isdigit(static_cast<unsigned char>(s[9]));
}
```

Replace the copy-pasted 10-character validation block in both `isTimestamp`
and `parseTimestamp`.

**Impact:** Date format rule has a single definition; `parseTimestamp` can
simply call `looksLikeIso8601Date(raw)`.

---

## Recommended Execution Order

| Priority | Step | Risk | Lines eliminated (approx.) |
|----------|------|------|---------------------------|
| 1 | Step 1 — `SourceGuard` | Low | ~26 |
| 2 | Step 2 — `skipLine` | Low | ~10 |
| 3 | Step 8 — `looksLikeIso8601Date` | Low | ~12 |
| 4 | Step 4 — `addUniqueDictEntry` | Low | ~8 |
| 5 | Step 5 — `resolveAlias` | Low | ~8 |
| 6 | Step 6 — `isNullStringNode` | Low | ~6 |
| 7 | Step 3 — `tryParseToken` | Medium | ~30 |
| 8 | Step 7 — tag coercion dispatch | Medium | ~25 |

Steps 1–6 are pure mechanical extractions with zero behaviour change.
Steps 7–8 introduce new helper types/lambdas and should be followed by a
full test-suite run (`[YAML][TestSuite][Sweep]`) before merging.

---

## Files to be Modified

| File | Steps |
|------|-------|
| `Default_Parser.hpp` | 1, 2, 3, 4, 5, 6, 8 (declarations) |
| `YAML_Parser_Router.cpp` | 1 |
| `YAML_Parser_Scalar.cpp` | 1, 3 |
| `YAML_Parser_Timestamp.cpp` | 1, 8 |
| `YAML_Parser_Util.cpp` | 1, 2 |
| `YAML_Parser_Dictionary.cpp` | 1, 2, 4, 6 |
| `YAML_Parser_Array.cpp` | 6 |
| `YAML_Parser_Directive.cpp` | 5 |
| `YAML_Parser_Tag.cpp` | 7 |
| `YAML_Parser.cpp` | 2 |
