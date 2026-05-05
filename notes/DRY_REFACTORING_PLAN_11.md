# DRY Refactoring Plan 11 — Library-Wide Sweep

> Prerequisite: Plans 1–10 committed. The parser layer is already clean.
> This plan covers the **IO layer**, **stringify layer**, **node/variant layer**,
> and **infrastructure layer** — areas untouched by earlier plans.
>
> After each section: build → test → commit before moving on.
> Baseline: 60 test cases / 53 passing / 7 pre-existing failures (file-path).

---

## Overview: patterns found

| # | Pattern | Files affected | Duplicate lines (approx.) |
|---|---------|---------------|--------------------------|
| 1 | `ISource::next()` forbidden-char + line tracking | BufferSource, SpanSource, FileSource | ~24 |
| 2 | `ISource` save/restore/discardSave/backup/reset | BufferSource, SpanSource, FileSource, StreamSource | ~40 |
| 3 | `IDestination` char-wise add overloads | FixedDestination, CallbackDestination | ~18 |
| 4 | `stringifyNodes` dispatch chain | JSON, XML, Bencode, Default stringifiers | ~40 |
| 5 | `stringifyDocument` body | JSON, XML, Bencode stringifiers | ~9 |
| 6 | `checkNode<T>` if-constexpr chain | YAML_Node_Reference.hpp | ~22 |
| 7 | `ToStringVisitor` / `ToKeyVisitor` struct pair | YAML_Node_Reference.hpp | ~14 |
| 8 | Bencode len-prefix pattern | Bencode_Stringify.hpp | ~4 |
| 9 | `DictionaryEntry` `&`/`&&` constructor pairs | YAML_Dictionary.hpp | ~8 |

Plans are ordered: **low-risk first, high-reward second**.

---

## Section 1 — ISource concrete classes: shared buffer-backed boilerplate

### Problem

`BufferSource`, `SpanSource`, and `FileSource` each contain byte-for-byte
identical implementations of six methods:

| Method | Identical body |
|--------|---------------|
| `next()` | forbidden-char check + lineNo/column tracking + `bufferPosition++` |
| `save()` | `contexts.push_back(Context(lineNo, column, bufferPosition))` |
| `restore()` | pop Context, restore `lineNo`, `column`, `bufferPosition` |
| `discardSave()` | `contexts.pop_back()` |
| `reset()` | `bufferPosition = 0; lineNo = 1; column = 1;` |
| `backup(n)` | column underflow check + `bufferPosition -= n; column -= n;` |

Only the error message for "read past end" and the actual buffer type
(`std::string`, `std::span`, `const char*`) differ.

`StreamSource` shares `save()`, `restore()`, `discardSave()`, and `backup()`
but differs in `next()` (uses `stream.get()` / `seekg`) and `reset()`
(seeks to 0).

### Fix: introduce `BufferedSourceBase`

Add a new protected CRTP base
`classes/include/implementation/io/YAML_BufferedSourceBase.hpp` that provides
a concrete implementation of all six methods for the
"contiguous byte buffer with a `bufferPosition` index" pattern.
`BufferSource`, `SpanSource`, and `FileSource` inherit from it.

```
ISource (abstract)
└── BufferedSourceBase      ← NEW: save/restore/discardSave/reset/backup, next()
    ├── BufferSource        ← overrides: current(), more(); adds ownedBuffer/bufferView
    ├── SpanSource          ← overrides: current(), more(); adds data_/len_
    └── FileSource          ← overrides: current(), more(); loads file into buffer
```

`BufferedSourceBase` needs a virtual hook for the end-of-input error string and
for the actual byte read, so `next()` can be shared:

```cpp
/// YAML_BufferedSourceBase.hpp
template <typename Derived>
class BufferedSourceBase : public ISource {
protected:
    // Subclass supplies the "read past end" message (string literal)
    [[nodiscard]] virtual const char *endOfInputMessage() const noexcept = 0;

public:
    void next() override {
        const auto uc = static_cast<unsigned char>(current());
        if (kForbiddenChar[uc]) {
            char buf[5];
            std::snprintf(buf, sizeof(buf), "%04X", uc);
            YAML_THROW_POS(*this,
                std::string("Disallowed control character U+") + buf +
                " in YAML stream.");
        }
        if (current() == kLineFeed) {
            lineNo++;
            column = 1;
        } else {
            column++;
        }
        if (!ISource::more()) {   // calls derived more() via override
            YAML_THROW(Error, endOfInputMessage());
        }
        bufferPosition++;
    }

    void save() override {
        contexts.push_back(Context(lineNo, column, bufferPosition));
    }
    void restore() override {
        const Context ctx{contexts.back()};
        contexts.pop_back();
        lineNo          = ctx.lineNo;
        column          = ctx.column;
        bufferPosition  = ctx.bufferPosition;
    }
    void discardSave() override { contexts.pop_back(); }

    void reset() override {
        bufferPosition = 0;
        lineNo = 1;
        column = 1;
    }

    [[nodiscard]] std::size_t position() override { return bufferPosition; }

protected:
    void backup(const unsigned long length) override {
        if (column - length < 1) {
            YAML_THROW(Error, "Backup past start column.");
        }
        bufferPosition -= length;
        column         -= length;
    }
};
```

After this:

```cpp
// BufferSource.hpp — only needs to implement current(), more(), endOfInputMessage()
class BufferSource final : public BufferedSourceBase<BufferSource> {
    [[nodiscard]] const char *endOfInputMessage() const noexcept override {
        return "Tried to read past end of buffer.";
    }
    [[nodiscard]] char current() const override { ... }
    [[nodiscard]] bool more()    const override { ... }
    // save/restore/discardSave/reset/backup/next → inherited
};
```

**Files changed:**
- `classes/include/implementation/io/YAML_BufferedSourceBase.hpp` — **new**
- `classes/include/implementation/io/YAML_BufferSource.hpp` — inherit base, remove 6 methods
- `classes/include/implementation/io/YAML_SpanSource.hpp` — inherit base, remove 6 methods
- `classes/include/implementation/io/YAML_FileSource.hpp` — inherit base, remove 6 methods
- `classes/include/implementation/io/YAML_Sources.hpp` — include new header
- `classes/include/YAML_Core.hpp` — no change (YAML_Sources.hpp already included)

**Net reduction:** ~64 duplicate lines removed across the three files.

**Risk:** Low. Same logic, just relocated. `StreamSource` is NOT changed here
(its `next()` is genuinely different). Tests cover all ISource paths.

---

## Section 2 — IDestination concrete classes: char-wise add overloads

### Problem

`FixedDestination` and `CallbackDestination` each implement:

```cpp
void add(const std::string_view &bytes) override { for (const char ch : bytes) add(ch); }
void add(const std::string      &bytes) override { for (const char ch : bytes) add(ch); }
void add(const char             *bytes) override { while (*bytes) add(*bytes++); }
```

These three bodies are byte-for-byte identical. Both destinations can express
the string/view/C-string overloads purely in terms of `add(char)`.

`BufferDestination` and `StreamDestination` use slightly different strategies
(batch-write to string/stream) so they are NOT changed here.

### Fix: default implementations in `IDestination`

Move the three char-loop bodies into `IDestination` as non-pure concrete
methods. Because `add(char)` is still pure virtual, each concrete class must
still implement `add(char)` — but the other three overloads become inherited:

```cpp
// IDestination.hpp
class IDestination {
public:
    virtual void add(char ch) = 0;                                    // pure
    virtual void add(const std::string_view &bytes) {                 // concrete
        for (const char ch : bytes) add(ch);
    }
    virtual void add(const std::string &bytes) {                      // concrete
        for (const char ch : bytes) add(ch);
    }
    virtual void add(const char *bytes) {                             // concrete
        while (*bytes) add(*bytes++);
    }
    // ...
};
```

`BufferDestination` and `StreamDestination` override all four for performance
(batch-write).  `FixedDestination` and `CallbackDestination` only override
`add(char)` and inherit the rest.

**Files changed:**
- `classes/include/interface/IDestination.hpp` — add three concrete methods
- `classes/include/implementation/io/YAML_FixedDestination.hpp` — remove 3 overloads
- `classes/include/implementation/io/YAML_CallbackDestination.hpp` — remove 3 overloads

**Net reduction:** ~18 duplicate lines removed.

**Risk:** Very low. `virtual` + concrete body is idiomatic C++. The removed
overloads are byte-for-byte identical to what IDestination would provide.

---

## Section 3 — Bencode_Stringify: len-prefix pattern

### Problem

In `Bencode_Stringify.hpp` the Bencode string format (`<len>:<content>`)
is assembled twice with identical structure:

```cpp
// stringifyString:
destination.add(std::to_string(static_cast<int>(yamlString.length())) +
                ":" + std::string(yamlString));

// stringifyTimestamp:
destination.add(std::to_string(static_cast<int>(ts.length())) +
                ":" + std::string(ts));
```

A third occurrence exists in `stringifyDictionary` for keys.

### Fix: private static helper

```cpp
// In Bencode_Stringify, private section:
static void addBencodeString(IDestination &dest, const std::string_view sv) {
    dest.add(std::to_string(sv.length()) + ":" + std::string(sv));
}
```

Then:
```cpp
static void stringifyString(const Node &yNode, IDestination &dest) {
    addBencodeString(dest, NRef<String>(yNode).value());
}
static void stringifyTimestamp(const Node &yNode, IDestination &dest) {
    addBencodeString(dest, NRef<Timestamp>(yNode).value());
}
// In stringifyDictionary:
addBencodeString(dest, entry.getKey());   // replaces the manual to_string+colon pattern
```

**Files changed:**
- `classes/include/implementation/stringify/Bencode_Stringify.hpp` — add helper, update 3 sites

**Net reduction:** ~6 duplicate lines, plus `static_cast<int>` removed (`.length()`
already returns `size_t`, which `to_string` accepts directly).

**Risk:** Very low. Self-contained within one file.

---

## Section 4 — `DictionaryEntry`: collapse `&`/`&&` constructor pairs

### Problem

`YAML_Dictionary.hpp` has four constructors arranged as two `&`/`&&` pairs:

```cpp
DictionaryEntry(const std::string_view &key, Node  &yNode, char quote = kNull)
    : yNodeKey(key), yNodeKeyQuote(quote), yNode(std::move(yNode)) {}
DictionaryEntry(const std::string_view &key, Node &&yNode, char quote = kNull)
    : yNodeKey(key), yNodeKeyQuote(quote), yNode(std::move(yNode)) {}
```

Both bodies are identical: `yNode(std::move(yNode))`.  The two overloads exist
only to handle lvalue/rvalue at the call site, but since `Node` is
move-only (copy-deleted), passing by value already forces a move from rvalues:

```cpp
DictionaryEntry(const std::string_view &key, Node yNode, char quote = kNull)
    : yNodeKey(key), yNodeKeyQuote(quote), yNode(std::move(yNode)) {}
```

The same applies to the `Node`/`Node`-key-extraction pair.

### Fix: collapse each pair to a single by-value constructor

```cpp
// First pair → one constructor
DictionaryEntry(const std::string_view &key, Node yNode, char quote = kNull)
    : yNodeKey(key), yNodeKeyQuote(quote), yNode(std::move(yNode)) {}

// Second pair → one constructor  
DictionaryEntry(Node &keyNode, Node yNode)
    : yNodeKey(std::get<String>(keyNode.getVariant()).toString()),
      yNodeKeyQuote(std::get<String>(keyNode.getVariant()).getQuote()),
      yNode(std::move(yNode)) {}
```

**Files changed:**
- `classes/include/implementation/variants/YAML_Dictionary.hpp` — remove 2 constructors

**Risk:** Low. `Node` is move-only; all callers already construct a temporary
or explicitly move into the entry. Must verify no call site passes an lvalue
expecting it NOT to be moved (search for `DictionaryEntry(key, node)` where
`node` is reused after the call).

---

## Section 5 — `checkNode<T>`: collapse if-constexpr chain

### Problem

`YAML_Node_Reference.hpp` contains a 50-line `checkNode<T>` function with
ten almost-identical `if constexpr` branches:

```cpp
template <typename T> void checkNode(const Node &yNode) {
    if constexpr (std::is_same_v<T, String>) {
        if (!isA<T>(yNode)) YAML_THROW(Node::Error, "Node not a string.");
    } else if constexpr (std::is_same_v<T, Number>) {
        if (!isA<T>(yNode)) YAML_THROW(Node::Error, "Node not a number.");
    } // ... 8 more branches ...
}
```

Every branch does exactly one thing: check `isA<T>` and throw with a
type-specific label.

### Fix: `nodeName<T>()` + single-body `checkNode<T>()`

Add a `consteval` name-lookup function and simplify `checkNode`:

```cpp
template <typename T>
consteval std::string_view nodeName() noexcept {
    if constexpr (std::is_same_v<T, String>)     return "string";
    else if constexpr (std::is_same_v<T, Number>)     return "number";
    else if constexpr (std::is_same_v<T, Array>)      return "array";
    else if constexpr (std::is_same_v<T, Dictionary>) return "dictionary";
    else if constexpr (std::is_same_v<T, Boolean>)    return "boolean";
    else if constexpr (std::is_same_v<T, Null>)       return "null";
    else if constexpr (std::is_same_v<T, Hole>)       return "hole";
    else if constexpr (std::is_same_v<T, Comment>)    return "comment";
    else if constexpr (std::is_same_v<T, Document>)   return "document";
    else if constexpr (std::is_same_v<T, Timestamp>)  return "timestamp";
    else return "unknown";
}

template <typename T>
void checkNode(const Node &yNode) {
    if (!isA<T>(yNode)) {
        // Concatenated at compile time via std::string; no extra allocation
        YAML_THROW(Node::Error,
            std::string("Node not a ").append(nodeName<T>()).append("."));
    }
}
```

**Files changed:**
- `classes/include/implementation/node/YAML_Node_Reference.hpp`

**Net reduction:** ~38 lines → ~18 lines (plus the new `nodeName` function
of ~14 lines = net saving of ~10 lines, with better extensibility).

**Risk:** Low. The error message changes from `"Node not a string."` to
`"Node not a string."` — identical. Any test that checks the exact error
string must be verified (grep for `"Node not a "`).

---

## Section 6 — `ToStringVisitor` / `ToKeyVisitor`: merge into one

### Problem

`YAML_Node_Reference.hpp` defines two visitor structs that are structurally
identical. Only the generic template operator body differs:

```cpp
struct ToStringVisitor {
    std::string operator()(const std::monostate &)               const { return ""; }
    std::string operator()(const std::unique_ptr<Array> &p)      const { return p->toKey(); }
    std::string operator()(const std::unique_ptr<Dictionary> &p) const { return p->toKey(); }
    std::string operator()(const std::unique_ptr<Document> &)    const { return ""; }
    template <typename T>
    std::string operator()(const T &v) const { return v.toString(); }  // ← differs
};
struct ToKeyVisitor {
    // ... identical first four overloads ...
    template <typename T>
    std::string operator()(const T &v) const { return v.toKey(); }     // ← differs
};
```

The four concrete overloads are byte-for-byte duplicated.

### Fix: single `NodeTextVisitor` template

```cpp
namespace detail {

enum class TextMode { ToString, ToKey };

template <TextMode Mode>
struct NodeTextVisitor {
    std::string operator()(const std::monostate &)               const { return ""; }
    std::string operator()(const std::unique_ptr<Array> &p)      const { return p->toKey(); }
    std::string operator()(const std::unique_ptr<Dictionary> &p) const { return p->toKey(); }
    std::string operator()(const std::unique_ptr<Document> &)    const { return ""; }
    template <typename T>
    std::string operator()(const T &v) const {
        if constexpr (Mode == TextMode::ToString) return v.toString();
        else                                      return v.toKey();
    }
};

} // namespace detail

inline std::string Node::toString() const {
    return std::visit(detail::NodeTextVisitor<detail::TextMode::ToString>{}, yNodeVariant);
}
inline std::string Node::toKey() const {
    return std::visit(detail::NodeTextVisitor<detail::TextMode::ToKey>{}, yNodeVariant);
}
```

**Files changed:**
- `classes/include/implementation/node/YAML_Node_Reference.hpp`

**Net reduction:** ~14 duplicate lines removed.

**Risk:** Very low. Pure refactor of two private `detail::` structs. No
observable behaviour change; tested end-to-end by all stringify tests.

---

## Section 7 — Stringifier dispatch: extract `stringifyDocument` free function

### Problem

`JSON_Stringify`, `XML_Stringify`, and `Bencode_Stringify` each contain:

```cpp
static void stringifyDocument(const Node &yNode, IDestination &destination,
                              const long indent) {
    stringifyNodes(NRef<Document>(yNode)[0], destination, indent);
}
```

These three bodies are identical. `Default_Stringify::stringifyDocument` is
different (emits `---` / `...` markers) so it is NOT touched here.

### Fix: shared free function in a detail namespace

In a new header `classes/include/implementation/stringify/YAML_Stringify_Detail.hpp`
(or inline at the top of each stringifier — small enough either way), add:

```cpp
namespace YAML_Lib::detail {
/// Unwrap a Document node and dispatch to the per-format stringifyNodes.
/// Used by JSON, XML, and Bencode stringifiers which do not emit --- / ...
template <typename StringifyFn>
inline void stringifyDocumentContent(const Node &yNode, IDestination &dest,
                                     long indent, StringifyFn &&fn) {
    fn(NRef<Document>(yNode)[0], dest, indent);
}
} // namespace
```

Or, since each stringifier calls its own `stringifyNodes`, a simpler fix is
a private static shared helper via a free function in the existing anonymous
namespace:

```cpp
// Inside each stringifier class — just call the helper directly:
static void stringifyDocument(const Node &yNode, IDestination &dest, long indent) {
    stringifyNodes(NRef<Document>(yNode)[0], dest, indent);  // already a one-liner
}
```

The body IS already one line. The duplication is of the boilerplate
(function signature + braces), not logic. The cleanest resolution is to
**document it as an intentional parallel** and add a comment in each
stringifier rather than over-abstracting. However, if the team wants strict
DRY:

Add to `IStringify`:
```cpp
// Default document-unwrap used by most stringifiers
// (Default_Stringify overrides to emit --- / ... markers)
static void unwrapDocument(const Node &yNode, IDestination &dest,
                           unsigned long indent,
                           void (*stringifyNodes)(const Node &, IDestination &, unsigned long)) {
    stringifyNodes(NRef<Document>(yNode)[0], dest, indent);
}
```

**Recommendation:** This is a borderline case (one-liner body). Accept the
duplication in `stringifyDocument` as a necessary parallel structure and
instead focus on the higher-value items. Document with `// Parallel to
JSON_Stringify::stringifyDocument — intentional.`

---

## Section 8 — Stringifier dispatch chain: shared `throwUnknownNodeType()`

### Problem

All four stringifiers end their `stringifyNodes` dispatch chains with:

```cpp
YAML_THROW(Error, "Unknown Node type encountered during stringification.");
```

This identical string literal is duplicated 4 times.

### Fix: one-line helper function in a shared header or in `IStringify`

```cpp
// In IStringify.hpp or a shared stringify detail header:
[[noreturn]] inline void throwUnknownNodeType() {
    YAML_THROW(Error, "Unknown Node type encountered during stringification.");
}
```

Each stringifier then calls `throwUnknownNodeType()` in its `else` branch.

**Files changed:**
- `classes/include/interface/IStringify.hpp` — add `throwUnknownNodeType()`
- `classes/include/implementation/stringify/JSON_Stringify.hpp` — update `else` branch
- `classes/include/implementation/stringify/XML_Stringify.hpp` — update `else` branch
- `classes/include/implementation/stringify/Bencode_Stringify.hpp` — update `else` branch
- `classes/include/implementation/stringify/Default_Stringify.hpp` — update `else` branch

**Net reduction:** 3 duplicate literal strings eliminated; single point to
update if the message changes.

**Risk:** Very low.

---

## Section 9 — `Error` struct boilerplate

### Problem

The same `struct Error final : std::runtime_error { ... }` pattern appears in:

| Location | Prefix string |
|----------|--------------|
| `YAML_Error.hpp` | `"YAML Error: "` |
| `ISource.hpp` | `"ISource Error: "` |
| `IDestination.hpp` | `"IDestination Error: "` |
| `IAction.hpp` | `"IAction Error: "` |
| `YAML_Node.hpp` | `"Node Error: "` |

### Fix: `YAML_MAKE_ERROR` macro or base-class template

```cpp
// In a shared header (e.g., YAML_Error.hpp):
#define YAML_MAKE_ERROR(StructName, Prefix)                             \
    struct StructName final : std::runtime_error {                      \
        explicit StructName(const std::string_view &message)            \
            : std::runtime_error(                                        \
                std::string(Prefix ": ").append(message)) {}            \
    }
```

Usage:
```cpp
// ISource.hpp
YAML_MAKE_ERROR(Error, "ISource Error");

// IDestination.hpp
YAML_MAKE_ERROR(Error, "IDestination Error");
```

OR: a CRTP template:
```cpp
template <const char *Prefix>
struct PrefixedError : std::runtime_error {
    explicit PrefixedError(const std::string_view &msg)
        : std::runtime_error(std::string(Prefix).append(": ").append(msg)) {}
};
```

**Recommendation:** The macro approach is simpler and idiomatic for nested
struct declarations. The CRTP approach requires NTTP string literals which have
subtle linkage issues in C++20. Use the macro.

**Files changed:** `YAML_Error.hpp` (add macro), then 4 header files to use it.

**Risk:** Low. Error message format is unchanged. Test any test that does
`REQUIRE(e.what() == "ISource Error: ...")`—there are a few in the test suite.

---

## Implementation order (recommended)

| Priority | Section | Effort | Risk | Reward |
|----------|---------|--------|------|--------|
| 1 | §3 Bencode len-prefix | 15 min | Very low | Medium |
| 2 | §8 `throwUnknownNodeType()` | 15 min | Very low | Low |
| 3 | §4 DictionaryEntry constructors | 20 min | Low | Medium |
| 4 | §6 ToStringVisitor/ToKeyVisitor merge | 20 min | Low | Medium |
| 5 | §5 `checkNode<T>` + `nodeName<T>()` | 30 min | Low | High |
| 6 | §2 IDestination default add overloads | 30 min | Low | Medium |
| 7 | §1 `BufferedSourceBase` | 60 min | Medium | High |
| 8 | §9 `YAML_MAKE_ERROR` macro | 30 min | Low | Low |
| 9 | §7 `stringifyDocument` | accept as parallel structure | — | — |

---

## Metrics to track

Before starting: `wc -l classes/include/**/*.hpp classes/source/**/*.cpp`

| File cluster | Lines before | Lines after (target) |
|--------------|-------------|---------------------|
| IO sources (Buffer/Span/File) | ~330 | ~220 (−110) |
| IO destinations (Fixed/Callback) | ~190 | ~155 (−35) |
| Stringify headers (all 4) | ~490 | ~475 (−15) |
| Node reference/variant headers | ~300 | ~255 (−45) |
| Dictionary | ~110 | ~100 (−10) |
| **Total** | **~1420** | **~1205 (−~215)** |

Test pass count must remain 53/60 throughout.

---

## What was deliberately excluded

| Candidate | Reason not included |
|-----------|---------------------|
| `stringifyDocument` body (one-liner) | Too small to abstract without adding complexity |
| Scalar struct rule-of-five boilerplate | Standard C++ pattern; compilers generate it; no runtime cost |
| `NRef<T>` const/non-const overloads | Cannot collapse without `const_cast` or proxy; not worth the complexity |
| Stringifier `stringifyNodes` chain | Structurally parallel but logic in each branch is format-specific; a shared dispatch would require a visitor interface that is more complex than the duplication it removes |
| `inline static std::unique_ptr<ITranslator>` in JSON/XML | Different translator types (`Default_Translator` vs `XML_Translator`); cannot share without type erasure |
| `isInsideFlowContext()` | Already extracted in Plan 6 |
| Parser `is*` / `parse*` naming convention | Style, not duplication |
