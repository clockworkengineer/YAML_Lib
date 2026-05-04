# Performance and Size Reduction Plan

> Analysis date: 2026-05-04  
> Target: reduce binary size, heap pressure, and parse/stringify runtime  
> Constraint: preserve the full yaml-test-suite regression suite (58 tests / 3157 assertions)

---

## Methodology

The library was studied file-by-file. Findings are grouped into four tiers by
expected impact. Each item names the exact file(s) affected, the current
behaviour, the proposed change, and the measurable benefit.

---

## Tier 1 — High-impact structural changes

These address the root causes of the largest overheads and touch core data
structures or hot parsing loops.

---

### T1-1  Replace `unique_ptr<Variant>` with `std::variant` inside `Node`

**Files:** `YAML_Node.hpp`, `YAML_Variant.hpp`, every `YAML_*.hpp` variant header,
`YAML_Node_Reference.hpp`, `YAML_Node_Creation.hpp`, `YAML_Node_Index.hpp`

**Current behaviour**  
Every `Node` owns a `std::unique_ptr<Variant>`.  Every scalar (Boolean, Null,
Number, String, Timestamp, Comment) therefore requires a separate heap
allocation.  Virtual dispatch (`toString()`, `toKey()`, `getNodeType()`) is
used for all type queries and conversions, requiring vtable pointer storage and
an indirect call on every access.  Pointer indirection also stresses the CPU
data cache: accessing a scalar value requires dereferencing the Node then
dereferencing the `unique_ptr`.

**Proposed change**  
Replace the `unique_ptr<Variant>` member with a `std::variant` holding all
concrete types by value:

```cpp
using NodeVariant = std::variant<
    std::monostate,   // empty / "hole" placeholder
    Boolean, Null, Number, String, Timestamp, Comment,
    Array, Dictionary, Document>;
```

- `isA<T>(node)` becomes `std::holds_alternative<T>(node.v)` — a single flag
  test, no virtual call.
- `NRef<T>(node)` becomes `std::get<T>(node.v)` — direct reference, no cast.
- The `Variant` base class and its vtable are eliminated.
- Scalar nodes no longer heap-allocate.

**Expected benefit**
- Eliminates one heap allocation per scalar node during parse (often > 90% of
  all nodes in typical YAML).
- Removes virtual-call overhead on every `isA<>` / `NRef<>` / `toString()`
  invocation.
- Reduces pointer-chase depth from 2 (Node → unique_ptr → Variant) to 0 for
  scalars.
- Reduces binary size by removing vtable entries and virtual thunks.

**Risk**  
API-breaking; requires updating every call-site of `isA<>`, `NRef<>`, the
type query helpers, and the traversal code.  Container variants (`Array`,
`Dictionary`, `Document`) still hold `std::vector<Node>` internally, so their
recursive structure is unchanged.

---

### T1-2  Store dictionary keys as `std::string`, not as `Node`

**Files:** `YAML_Dictionary.hpp`

**Current behaviour**  
`DictionaryEntry` stores two `Node` objects: `key` (a `Node` wrapping a
heap-allocated `String` variant) and `yNode` (the value).  Accessing a key
requires `dynamic_cast<String&>(key.getVariant()).value()` — a vtable lookup
followed by a cast.  Every dictionary entry therefore costs two heap
allocations (one per `Node`/`unique_ptr<Variant>` pair).

**Proposed change**  
Store the key directly as `std::string` in `DictionaryEntry`:

```cpp
struct DictionaryEntry {
    std::string key;          // plain string, no Node/unique_ptr overhead
    Node        value;
};
```

Keep the complex-key (`?`-indicator) representation as a separate `Node
complexKey` that is only populated when the key is itself a collection or
non-scalar.  For the 99% common case (plain string keys), no heap alloc beyond
the string's own SSO is needed.

**Expected benefit**
- Halves the number of heap allocations per dictionary entry in the common case.
- Eliminates the `dynamic_cast` on every `getKey()` call.
- Reduces `sizeof(DictionaryEntry)` significantly.

---

### T1-3  Switch dictionary lookup from linear scan to hash map

**Files:** `YAML_Dictionary.hpp`, `YAML_Dictionary.cpp` (findKey)

**Current behaviour**  
`findKey` performs a linear scan (`std::find_if`) over the entries vector.  On
a dictionary with n entries, `contains()` and `operator[]` are O(n).
Worse, `contains()` relies on catching the `Node::Error` exception thrown by
`findKey` to signal "not found" — exception-based control flow for normal
operations.

**Proposed change**  
Add a `std::unordered_map<std::string, std::size_t> index` member alongside
the `entries` vector.  The vector preserves insertion order (needed for
stringify round-tripping); the hash map provides O(1) key → index translation.
Replace the try/catch in `contains()` with:

```cpp
[[nodiscard]] bool contains(const std::string_view &key) const noexcept {
    return index.count(std::string(key)) != 0;
}
```

Change `findKey` to return `std::optional<Entries::iterator>` or an index,
eliminating the exception-throwing "not found" path entirely.

**Expected benefit**
- O(1) average key lookup vs O(n).
- Eliminates exception-based control flow in `contains()` (called on every
  dictionary node during parse).
- Allows the alias-merge (`<<`) and override logic to run faster on large
  dictionaries.

---

### T1-4  Load file into memory in `FileSource` — eliminate `seekg` save/restore

**Files:** `YAML_FileSource.hpp`

**Current behaviour**  
`FileSource` wraps `std::ifstream` and implements `save()`/`restore()` with
`source.seekg()`.  The parser saves and restores the stream position
frequently during speculative parses (`SourceGuard`).  Each `restore()` issues
a `seekg()` which on most OS/CRT combinations causes a kernel transition and
flushes the read-ahead buffer.

**Proposed change**  
In `FileSource`'s constructor, read the entire file into a `std::string` or
`std::vector<char>` buffer, then delegate all `current()`, `next()`, `more()`,
`save()`, `restore()` to index arithmetic on that buffer — exactly how
`BufferSource` works.  The open/read is done once; afterwards the source is
purely in-memory.

```cpp
explicit FileSource(const std::string_view &filename) {
    std::ifstream f(filename.data(), std::ios::binary | std::ios::ate);
    ...
    buffer.resize(size);
    f.read(buffer.data(), size);
}
```

**Expected benefit**
- Converts every `seekg()` in `restore()` into a plain integer assignment.
- Eliminates repeated kernel transitions for large files with many speculative
  parses.
- Simplifies the `FileSource` implementation to near-identical to
  `BufferSource` (the two classes could be merged with a `variant<string_view,
  string>` owner field).

---

### T1-5  Replace exception-based numeric parsing with `std::from_chars`

**Files:** `YAML_Number.hpp` (`stringToNumber<T>`)

**Current behaviour**  
`Number::convertNumber` tries six types (`int`, `long`, `long long`, `float`,
`double`, `long double`) in sequence.  Each `stringToNumber<T>` call wraps
`std::stoi`/`stol`/`stoll`/`stof`/`stod`/`stold` in a try/catch block.  On a
failure (e.g. overflow), an exception is constructed, thrown, caught, and
discarded before the next type is tried.  This happens for every number token
during parse.

**Proposed change**  
Use `std::from_chars` (C++17, header `<charconv>`) which returns an error code
without exceptions and is typically 3–10× faster than the `strtoX` family for
integers:

```cpp
template <typename T>
bool Number::stringToNumber(const std::string_view &number) {
    T value{};
    auto [ptr, ec] = std::from_chars(number.data(),
                                     number.data() + number.size(), value);
    if (ec != std::errc{} || ptr != number.data() + number.size())
        return false;
    yNodeNumber = value;
    return true;
}
```

Note: `std::from_chars` for floating-point types requires MSVC 2019 16.4+ or
GCC 11+ (available in this project's toolchain).

**Expected benefit**
- Eliminates up to 5 exception constructions per number token (for types that
  do not fit).
- `from_chars` integer parsing is significantly faster than `stoi`/`stol`.
- Reduces binary size by removing exception handler tables for six type
  instantiations.

---

## Tier 2 — Medium-impact targeted fixes

These are self-contained changes to specific functions without structural
redesign.

---

### T2-1  Eliminate the two-pass `validateInputCharacters` pre-scan

**Files:** `YAML_Parser_Util.cpp`, `YAML_Parser.cpp` (`parse()` entry point)

**Current behaviour**  
`Default_Parser::parse()` calls `validateInputCharacters(source)` which scans
the entire source from beginning to end checking for forbidden control
characters, then resets.  This is a full O(n) pass before parsing begins,
doubling I/O cost for large inputs.

**Proposed change**  
Remove `validateInputCharacters`.  Instead, add a single-character check
inline in `BufferSource::next()` (or in `appendCharacterToString`) that throws
`SyntaxError` the first time a forbidden byte is consumed.  A constexpr lookup
table of 256 booleans indexed by `unsigned char` keeps the check to one array
load + branch:

```cpp
static constexpr bool kForbidden[256] = { /* generated at compile time */ };

void BufferSource::next() {
    ...
    if (kForbidden[static_cast<unsigned char>(buffer[bufferPosition])])
        throw SyntaxError(getPosition(), "Disallowed control character ...");
    ...
}
```

**Expected benefit**
- Removes one full O(n) pass over every input.
- For a 1 MB YAML file the pre-scan cost is measurable (~1–2 ms on a modern CPU).

---

### T2-2  Remove `isValidKey` recursive parse

**Files:** `YAML_Parser_Dictionary.cpp` (`isValidKey`, `parseKey`)

**Current behaviour**  
`isValidKey(key)` creates a fresh `BufferSource` and calls `parseDocument` for
every candidate dictionary key encountered during parsing.  This is a
recursive invocation of the parser per key, adding O(k · parse(k)) overhead
where k is the average key length.

**Proposed change**  
Replace `isValidKey` with a direct syntactic check:

1. A key is invalid if it is empty, starts with `#`, or consists of only
   whitespace.
2. For the common case (plain scalar keys), a fast character-class scan
   suffices (reject `#` at start, reject bare `[`, `{`, `:` unless quoted).
3. Only fall back to `parseFromBuffer` for exotic complex keys (those starting
   with `?`).

```cpp
static bool isValidKey(const std::string_view &key) noexcept {
    if (key.empty()) return false;
    if (key[0] == '#') return false;
    // fast path — accept plain scalar keys directly
    if (std::all_of(key.begin(), key.end(), [](unsigned char c){
        return std::isprint(c) && c != '#';
    })) return true;
    // slow path — re-parse only for complex keys
    return isValidKeyFull(key);
}
```

**Expected benefit**
- Eliminates a parser invocation per dictionary key in the common case.
- Particularly significant for large YAML files with many keys.

---

### T2-3  Eliminate per-`Boolean` `std::string` storage

**Files:** `YAML_Boolean.hpp`

**Current behaviour**  
`Boolean` stores `bool yNodeBoolean` and `std::string booleanString` (the
original token, e.g. `"True"`, `"yes"`, `"On"`).  In YAML 1.2 strict mode
only `"true"`/`"false"` are valid, making the original-form preservation
redundant.  Even in YAML 1.1 mode the six true-forms and six false-forms can
be encoded as a 4-bit enum rather than a heap string.

**Proposed change**  
Replace `std::string booleanString` with a `uint8_t originalForm` index into a
compile-time table of the twelve valid boolean strings:

```cpp
enum class BoolForm : uint8_t {
    True=0, true_=1, Yes=2, yes=3, On=4, on=5,
    False=6, false_=7, No=8, no=9, Off=10, off=11
};
struct Boolean final : Variant {
    bool    value;
    BoolForm form;
    std::string toString() const override {
        static constexpr const char *kForms[] = {
            "True","true","Yes","yes","On","on",
            "False","false","No","no","Off","off"};
        return kForms[static_cast<int>(form)];
    }
};
```

**Expected benefit**
- Reduces `sizeof(Boolean)` from ~40 bytes (bool + std::string SSO) to ~2 bytes.
- Eliminates one heap allocation per Boolean node for non-SSO strings.

---

### T2-4  Switch `yamlAliasMap` from `std::map` to `std::unordered_map`

**Files:** `Default_Parser.hpp`, `YAML_Parser_Directive.cpp`

**Current behaviour**  
`yamlAliasMap` is declared `inline static std::map<std::string, std::string>`.
Every alias lookup (`resolveAlias`) is O(log n) in the number of anchors.

**Proposed change**  
Change to `std::unordered_map<std::string, std::string>`.  The map is keyed by
anchor name (short string), so the default hash is appropriate.  Anchor count
is typically small, but the O(1) average cost is still a win and makes future
large-anchor-count inputs robust.

**Expected benefit**
- O(1) average anchor lookup.
- Minor binary-size reduction (no comparison tree code).

---

### T2-5  Avoid `std::transform` lowercase copy in `parseNumber`

**Files:** `YAML_Parser_Scalar.cpp` (`parseNumber`)

**Current behaviour**  
For every number token, `parseNumber` creates a `std::string lower = numeric`
and calls `std::transform` to lowercase it, solely to compare against
`.inf`/`+.inf`/`-.inf`/`.nan`.

**Proposed change**  
Since all four special-float literals start with `.` or `+` or `-`, gate the
lowercase conversion behind a first-character check:

```cpp
if (!numeric.empty() && (numeric[0]=='.' || numeric[0]=='+' || numeric[0]=='-')) {
    std::string lower = numeric;
    // ... existing transform + comparison
}
```

For the vast majority of numeric tokens (plain integers, plain floats) the
branch is not taken and no copy is made.

**Expected benefit**
- Eliminates one `std::string` allocation + `std::transform` per plain numeric
  token.

---

### T2-6  Replace `splitString` stringstream with `string_view` scan in Stringify

**Files:** `Default_Stringify.hpp` (`splitString`)

**Current behaviour**  
`splitString` constructs a `std::stringstream`, reads lines with
`std::getline`, and pushes each into a `std::vector<std::string>`.  A
`std::stringstream` construction involves at least two allocations on MSVC.

**Proposed change**  
Replace with a lightweight `string_view`-based splitter:

```cpp
static std::vector<std::string_view> splitLines(std::string_view sv, char delim) {
    std::vector<std::string_view> parts;
    while (!sv.empty()) {
        auto pos = sv.find(delim);
        if (pos == std::string_view::npos) {
            parts.push_back(sv);
            break;
        }
        parts.push_back(sv.substr(0, pos + 1));
        sv.remove_prefix(pos + 1);
    }
    return parts;
}
```

**Expected benefit**
- Eliminates stringstream construction and per-line string copies during
  stringify.
- Returns `string_view` slices into the original string, avoiding allocations.

---

### T2-7  Cache indent strings in `calculateIndent` (Stringify)

**Files:** `Default_Stringify.hpp` (`calculateIndent`, `stringifyDictionary`,
`stringifyArray`)

**Current behaviour**  
`calculateIndent` returns `std::string(indent, kSpace)` on every non-newline
call, constructing a new string of spaces each time.

**Proposed change**  
Maintain a static (or thread-local) cache string.  When `indent` equals the
cached length, return a `std::string_view` into it; otherwise resize:

```cpp
static const std::string &indentString(unsigned long n) {
    static std::string buf;
    if (buf.size() < n) buf.assign(n, ' ');
    return buf;
}
```

Alternatively, write spaces directly to `destination` without constructing an
intermediate string.

**Expected benefit**
- Eliminates one `std::string` allocation per non-leaf node during stringify.

---

### T2-8  Accept `std::string_view` in `BufferSource` without copying

**Files:** `YAML_BufferSource.hpp`

**Current behaviour**  
`BufferSource(const std::string_view &buffer)` stores `std::string buffer`
— a copy of the entire input, even when the caller's string already has a
lifetime that spans the parse.

**Proposed change**  
Add a second constructor that takes ownership of a `std::string` (move), and a
third that stores only a `std::string_view` (zero-copy) when the caller
guarantees lifetime:

```cpp
explicit BufferSource(std::string owned) : buffer(std::move(owned)) {}
explicit BufferSource(std::string_view view) : bufferView(view) {}
```

Use a `std::variant<std::string, std::string_view>` or a single
`std::string_view` member pointing into either the owned string or the caller's
buffer.

**Expected benefit**
- Eliminates a full O(n) string copy when parsing from an already-live string.
- Important for the `parseFromBuffer` helper called during key validation,
  alias resolution, and anchor expansion.

---

## Tier 3 — Low-impact / code-size items

These are straightforward cleanups with smaller but measurable effects.

---

### T3-1  Remove `dynamic_cast` from `DictionaryEntry::getKey()`

**Files:** `YAML_Dictionary.hpp`

Superseded by T1-2 (store key as `std::string`).  If T1-2 is not adopted,
replace the `dynamic_cast` in `getKey()` with `static_cast` guarded by the
`Type` tag, or with `NRef<String>` which already uses `static_cast` internally
after tag checking.

---

### T3-2  Merge `Document` and `Array` implementations

**Files:** `YAML_Document.hpp`, `YAML_Array.hpp`

`Document` and `Array` are structurally identical (both wrap
`std::vector<Node>` with `add`, `size`, `value`, `operator[]`, `resize`).
The only difference is the `Variant::Type` tag.  Extract a common
`SequenceVariant<Tag>` base template:

```cpp
template <Variant::Type Tag>
struct SequenceBase : Variant {
    using Entry   = Node;
    using Entries = std::vector<Entry>;
    SequenceBase() : Variant(Tag) {}
    void add(Entry e)               { entries.emplace_back(std::move(e)); }
    std::size_t size() const        { return entries.size(); }
    Entries &value()                { return entries; }
    const Entries &value() const    { return entries; }
    Node &operator[](std::size_t i) { ... }
    void resize(std::size_t i)      { ... }
private:
    Entries entries;
};
using Array    = SequenceBase<Variant::Type::array>;
using Document = SequenceBase<Variant::Type::document>;
```

**Expected benefit**
- Reduces header-only duplicate code (~50 lines).
- Any bug fix or optimisation to the sequence logic applies automatically to
  both.

---

### T3-3  Use `std::array` for the parser dispatch table

**Files:** `Default_Parser.hpp`, `YAML_Parser.cpp`

The `parsers` member (the vector of `{predicate, parser}` pairs consulted in
`parseDocument`) has a fixed size known at compile time.  Changing it to
`std::array` avoids one heap allocation per `Default_Parser` construction and
enables better compiler optimisation (known size allows loop unrolling).

---

### T3-4  Reserve capacity on `yamlAliasMap` and `activeAliasExpansions`

**Files:** `YAML_Parser.cpp` (`parse()`)

After `yamlAliasMap.clear()`, call `yamlAliasMap.reserve(16)` (or a
configurable constant) to avoid repeated rehashing when anchors are added
during a parse.  Same for `activeAliasExpansions`.

---

### T3-5  Use `std::string_view` return in `String::value()`

**Files:** `YAML_String.hpp`

`String::value()` already returns `std::string_view`; ensure no caller
inadvertently copies via `auto` (should be `auto&` or `std::string_view`).
Audit all call-sites of `NRef<String>(node).value()`.

---

### T3-6  Pre-reserve `BufferDestination` capacity

**Files:** `YAML_BufferDestination.hpp`

Add a method `reserve(std::size_t n)` and call it from `YAML_Impl::stringify`
with a heuristic initial size (e.g. 4 × the count of nodes).  Prevents
repeated reallocations of the output string buffer.

---

## Tier 4 — Longer-term / exploratory

These require more design work and are recorded for future consideration.

---

### T4-1  Small-buffer optimisation for `String` nodes

Dictionary keys are usually short (< 23 bytes on MSVC with SSO).  After T1-2
they are plain `std::string` which already benefits from SSO.  For `String`
variant nodes (values), SSO applies as long as the stored text is ≤ 15 bytes
(MSVC) or ≤ 22 bytes (libstdc++).  No action needed beyond ensuring no
unnecessary copies are made that defeat SSO.

---

### T4-2  Arena allocator for parse-time nodes

If profiling shows allocation throughput is still the bottleneck after T1-1,
introduce a monotonic arena allocator for Node/Variant objects created during
parsing.  All nodes are allocated from a contiguous slab and freed at once when
the parser resets.  This is complex but can halve allocation cost for
deeply-nested YAML.

---

### T4-3  Lazy number parsing

Currently all number tokens are fully parsed and stored as `std::variant<int,
long, ...>` at parse time.  A lazy representation (`std::string_view` into the
source buffer + a conversion function on first `value<T>()` call) defers
conversion until the value is actually read, reducing startup cost for
config-file scenarios where most numbers are never accessed.

---

### T4-4  SIMD whitespace/newline scanning in `moveToNextIndent`

`moveToNextIndent` iterates character-by-character.  For large YAML files with
significant blank sections, a SIMD scan (`_mm_cmpeq_epi8` or
`std::string_view::find_first_not_of`) can advance 16–32 characters per cycle
instead of one.

---

## Implementation order (recommended)

| Priority | Item  | Rationale |
|----------|-------|-----------|
| 1        | T1-5  | Self-contained; immediate win, low risk |
| 2        | T2-1  | Self-contained; measurable win on large inputs |
| 3        | T2-4  | One-line change to `std::unordered_map` |
| 4        | T2-5  | Two-line guard in `parseNumber` |
| 5        | T2-6  | Replace `splitString`; no external API change |
| 6        | T2-7  | Cache indent string; no external API change |
| 7        | T2-3  | Replace `std::string` in `Boolean`; affects stringify output if canonical form changes |
| 8        | T1-4  | Make `FileSource` buffer-based; straightforward, measurable |
| 9        | T2-2  | Replace `isValidKey` recursive parse; requires test coverage |
| 10       | T1-3  | Add hash-map index to `Dictionary`; requires care around order preservation |
| 11       | T1-2  | Store dict keys as `std::string`; API-safe internal change |
| 12       | T2-8  | Zero-copy `BufferSource`; requires lifetime audit |
| 13       | T3-2  | Merge `Document`/`Array`; pure refactor |
| 14       | T1-1  | Structural `std::variant` Node redesign; highest impact, highest risk; do last |

---

## Measurement approach

Before and after each tier-1 change:

1. Run the yaml-test-suite sweep: `ctest -C Release --output-on-failure`
2. Measure binary size: `dir build\Release\YAML_Lib.dll` (or `.lib`)
3. Parse-time benchmark: parse a 1 MB generated YAML file 1000 times and
   record mean elapsed time via `std::chrono::high_resolution_clock`.
4. Allocation count: run under CRT leak-check or a custom allocator that counts
   `operator new` calls.

No performance change should be committed unless all 3157 assertions still pass.
