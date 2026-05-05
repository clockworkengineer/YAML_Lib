# YAML_Lib Embedded Systems Enhancement Plan

> Analysis date: 2026-05-04
> Target: make YAML_Lib viable on resource-constrained embedded systems
> Baseline: C++23 library, full STL, dynamic allocation, exception-based errors

---

## Current Library Characteristics

| Aspect | Current State | Embedded Impact |
|--------|--------------|-----------------|
| C++ standard | C++23 required | Needs GCC 12+/Clang 15+ — acceptable on Cortex-A, problematic on Cortex-M with legacy toolchains |
| RTTI | None — `std::variant` + `isA<T>` | ✅ Safe — no dynamic_cast, no typeid |
| Exceptions | ~100+ throw sites | ❌ Incompatible with `-fno-exceptions` builds |
| Dynamic allocation | Every string, container, parser init | ❌ Incompatible with no-heap bare-metal |
| `std::function` | Parser dispatch table (16 entries) | ⚠️ Small overhead; avoidable |
| Global/static state | Parser and stringifier singletons; alias map; parser state flags | ❌ Not re-entrant, not thread-safe |
| File I/O | `std::ifstream` in FileSource/FileDestination | ❌ No filesystem on bare-metal |
| STL containers | `vector`, `unordered_map`, `set`, `string` throughout | ❌ Requires OS-backed allocator |
| Output | `BufferDestination` accumulates to `std::string` | ❌ Heap-backed; unbounded growth |

---

## Target Embedded Profiles

The plan addresses three distinct levels. Implement in order; each level builds on the previous.

### Profile A — "Linux-class embedded" (Cortex-A, RISC-V with OS)
Raspberry Pi, i.MX8, BeagleBone, Yocto/buildroot Linux.
Full STL available. The main concerns are: binary size, heap fragmentation, thread safety.

### Profile B — "RTOS embedded" (Cortex-M with FreeRTOS/Zephyr)
STM32, NXP i.MX RT, ESP32.
Limited heap (tens–hundreds of KB). `std::string`/`std::vector` exist but are expensive.
No `std::ifstream`. Exceptions often disabled.

### Profile C — "Bare-metal" (Cortex-M0/M0+, AVR)
No OS, no heap, no exceptions, fixed RAM. YAML parsing from flash/ROM buffers only.
Read-only parse into caller-supplied fixed-size structures.

---

## Enhancement Phases

---

### E1 — Exception-free error reporting (Profile A, B, C)

**Files:** all parser CPPs, `IParser.hpp`, `IDestination.hpp`, `ISource.hpp`, `YAML.hpp`, `YAML_Impl.hpp`

**Current behaviour**
All error signalling uses `throw` with a custom exception hierarchy rooted at
`std::runtime_error`. The parser has over 100 `throw SyntaxError(...)` sites.
`-fno-exceptions` (required on most Cortex-M toolchains) breaks the build.

**Proposed change**

1. Add a `Result<T>` type alias for `std::expected<T, Error>` (C++23) for
   operations that can fail:

   ```cpp
   // In YAML_Error.hpp
   enum class ErrorCode : uint8_t {
       Ok = 0,
       SyntaxError,
       FileNotFound,
       InvalidIndex,
       InvalidKey,
       BadEncoding,
       // ...
   };
   struct Error {
       ErrorCode code;
       const char *message;   // points to string literal, no allocation
       unsigned long line;
       unsigned long column;
   };
   template <typename T>
   using Result = std::expected<T, Error>;
   ```

2. Add a compile-time feature flag:

   ```cmake
   option(YAML_LIB_NO_EXCEPTIONS "Disable C++ exceptions" OFF)
   if(YAML_LIB_NO_EXCEPTIONS)
       target_compile_definitions(YAML_Lib PUBLIC YAML_LIB_NO_EXCEPTIONS)
       target_compile_options(YAML_Lib PUBLIC -fno-exceptions -fno-rtti)
   endif()
   ```

3. Under `YAML_LIB_NO_EXCEPTIONS`, replace all `throw X(...)` with a macro
   `YAML_THROW(code, msg)` that either throws or calls a registered error
   callback + returns an error sentinel:

   ```cpp
   #ifdef YAML_LIB_NO_EXCEPTIONS
   #  define YAML_THROW(code, msg)  do { errorCallback_(ErrorCode::code, msg, getLine(), getCol()); return {}; } while(0)
   #else
   #  define YAML_THROW(code, msg)  throw SyntaxError(msg, getLine(), getCol())
   #endif
   ```

4. Parser functions return `Result<Node>` instead of `Node`; callers check with
   `if (!result) { ... }`.

**Benefit**
- Enables `-fno-exceptions` builds for Cortex-M toolchains.
- No overhead when exceptions are enabled (same throw path).
- User retains error detail (code + message + position) without exceptions.

**Risk / effort:** High — ~100 throw sites. Systematic but mechanical.

---

### E2 — Remove global/static parser state (Profile A, B, C)

**Files:** `Default_Parser.hpp`, all `YAML_Parser_*.cpp` files, `YAML_Impl.hpp`

**Current behaviour**
`Default_Parser` uses `inline static` members for all parser state:
`yamlAliasMap`, `activeAliasExpansions`, `arrayIndentLevel`, `inlineArrayDepth`,
`inlineDictionaryDepth`, `blockFlowValueIndent`, `yamlDirectiveMinor`,
`yamlDirectiveSeen`, `yamlTagPrefixes`, `parsers` (routing table).

This makes the parser:
- **Non-re-entrant:** only one parse can run at a time.
- **Not thread-safe:** concurrent parses from multiple threads corrupt state.
- **Not suitable for RTOS tasks** running YAML parsing in separate tasks.

**Proposed change**

1. Introduce a `ParseContext` struct that aggregates all per-parse mutable state:

   ```cpp
   struct ParseContext {
       std::unordered_map<std::string, std::string> aliasMap;
       std::set<std::string>                        activeAliasExpansions;
       long    arrayIndentLevel{0};
       long    inlineArrayDepth{0};
       long    inlineDictionaryDepth{0};
       unsigned long blockFlowValueIndent{0};
       int     yamlDirectiveMinor{2};
       bool    yamlDirectiveSeen{false};
       std::unordered_map<std::string, std::string> tagPrefixes;
   };
   ```

2. Change all parser free functions to accept `ParseContext &ctx` as their
   first argument. Remove all `inline static` mutable state from
   `Default_Parser`.

3. Keep `inline static const std::array<ParserEntry, 16> parsers` as immutable
   (the routing table itself is read-only after init).

4. `YAML_Impl::parse()` constructs a `ParseContext` on the stack and passes it
   down.

**Benefit**
- Enables multiple concurrent parses (e.g., separate RTOS tasks).
- Removes hidden global state — deterministic behaviour.
- Stack allocation of `ParseContext` avoids heap allocation for parser state.

**Risk / effort:** High — every parser function signature changes. No semantic change.

---

### E3 — Pluggable allocator support (Profile B, C)

**Files:** `YAML_Node.hpp`, `YAML_Array.hpp`, `YAML_Sequence.hpp`, `YAML_Dictionary.hpp`, `YAML_String.hpp`

**Current behaviour**
All containers (`Array`, `Document`, `Dictionary`) use `std::vector` and
`std::unordered_map` with the default `std::allocator` (system heap). Strings
use `std::string` (heap-backed). On embedded systems with small or
fragmented heaps this causes failures or unacceptable fragmentation.

**Proposed change**

1. Template the major container types on an `Allocator` parameter:

   ```cpp
   template <typename Allocator = std::allocator<Node>>
   struct BasicArray final : SequenceBase<BasicArray<Allocator>, Allocator> { ... };
   using Array = BasicArray<>;  // default stays unchanged
   ```

2. Thread a `std::pmr::memory_resource *` (C++17 polymorphic allocators) through
   construction:

   ```cpp
   // Create a parse arena backed by a static 64 KB buffer
   std::array<std::byte, 65536> parseBuffer;
   std::pmr::monotonic_buffer_resource arena{parseBuffer.data(), parseBuffer.size()};
   std::pmr::unsynchronized_pool_resource pool{&arena};

   YAML yaml{&pool};   // YAML uses pool for all node allocations
   yaml.parse(src);
   // arena freed at end of scope — one deallocation for all nodes
   ```

3. Use `std::pmr::string`, `std::pmr::vector`, `std::pmr::unordered_map` in the
   node/container variants when PMR allocator is active.

4. Provide a `MonotonicArena<N>` helper wrapping `std::pmr::monotonic_buffer_resource`
   over a `std::array<std::byte, N>` for callers without dynamic heap.

**Benefit**
- Zero heap fragmentation: monotonic arena allocates linearly, frees in bulk.
- Known worst-case memory: size the arena from max YAML file size + overhead.
- No change to the default `std::allocator` path — existing code unaffected.

**Risk / effort:** Medium-high. PMR is well-specified in C++17; the major risk is
propagating the allocator through every construction site.

---

### E4 — Static-capacity containers (Profile B, C)

**Files:** `YAML_Array.hpp`, `YAML_Sequence.hpp`, `YAML_Dictionary.hpp`

**Current behaviour**
`Array` and `Document` use `std::vector<Node>` (heap, dynamic capacity).
`Dictionary` uses `std::vector<DictionaryEntry>` + `std::unordered_map` (heap).

**Proposed change**

Provide fixed-capacity alternatives that store elements in an inlined array:

```cpp
// Static-capacity sequence — N is maximum number of elements
template <std::size_t N, typename Derived>
struct StaticSequenceBase {
    using Entry   = Node;
    using Entries = std::array<Entry, N>;

    void add(Entry e) {
        if (count_ >= N) YAML_THROW(Overflow, "Static array capacity exceeded.");
        entries_[count_++] = std::move(e);
    }
    std::size_t size() const { return count_; }
    // ...
private:
    Entries     entries_{};
    std::size_t count_{0};
};

template <std::size_t N>
struct StaticArray final : StaticSequenceBase<N, StaticArray<N>> {
    [[nodiscard]] std::string toKey() const;
};
```

Dictionary analogue uses `std::array<DictionaryEntry, N>` with linear scan
(acceptable for small N; O(n) lookup is fine up to ~32 entries, beyond which
the array fits in cache anyway).

The capacity `N` is supplied by the user as a compile-time constant reflecting
their YAML schema's known upper bounds:

```cpp
// Config for a device with max 16 keys per dictionary, max 32 array elements
using MyYAML = BasicYAML<StaticAllocPolicy<16, 32>>;
```

**Benefit**
- Zero dynamic allocation for the YAML tree on Profile C (bare-metal).
- Hard compile-time guarantee on worst-case RAM usage.
- No `std::vector` or `std::unordered_map` instantiation → smaller binary.

**Risk / effort:** Medium. Requires a policy template parameter to propagate
capacity to the YAML instance, or explicit use of the static variant types.

---

### E5 — `ISource` / `IDestination` without `std::string` (Profile B, C)

**Files:** `ISource.hpp`, `IDestination.hpp`, `YAML_BufferSource.hpp`, `YAML_BufferDestination.hpp`

**Current behaviour**
`IDestination::add(const std::string &)` requires `std::string`.
`BufferDestination` accumulates into `std::string` (heap, unbounded).
`ISource` returns `char` and uses `std::string_view`, which is lightweight.

**Proposed change**

1. Add a `SpanSource` backed by `std::span<const char>` or `const char *` + length:

   ```cpp
   class SpanSource final : public ISource {
   public:
       constexpr SpanSource(const char *data, std::size_t len) noexcept
           : data_{data}, len_{len} {}
       // current(), next(), more(), save(), restore() via index arithmetic
   private:
       const char *data_;
       std::size_t len_;
       std::size_t pos_{0};
       std::size_t saved_{0};
   };
   ```

   `SpanSource` points directly into ROM/flash — zero copy, zero allocation.

2. Add a `FixedDestination<N>` that writes into a caller-supplied `char` array:

   ```cpp
   template <std::size_t N>
   class FixedDestination final : public IDestination {
   public:
       explicit FixedDestination(char (&buf)[N]) : buf_{buf} {}
       void add(char ch) override {
           if (pos_ < N) buf_[pos_++] = ch;
           else overflow_ = true;
       }
       bool overflow() const { return overflow_; }
       std::size_t size() const { return pos_; }
   private:
       char       *buf_;
       std::size_t pos_{0};
       bool        overflow_{false};
   };
   ```

3. Add a `CallbackDestination` that invokes a user function per character or
   per line — enables streaming directly to UART/serial without any buffer:

   ```cpp
   class CallbackDestination final : public IDestination {
   public:
       using Sink = void(*)(char, void *);
       CallbackDestination(Sink sink, void *ctx) : sink_{sink}, ctx_{ctx} {}
       void add(char ch) override { sink_(ch, ctx_); }
       // add(string_view) loops add(char) — or user overrides for bulk
   };
   ```

**Benefit**
- `SpanSource` enables parsing YAML constants from ROM with zero heap.
- `FixedDestination` bounds stringify output to a known-size stack/static buffer.
- `CallbackDestination` enables streaming stringify to UART with no intermediate buffer.

**Risk / effort:** Low — purely additive new source/destination classes. Existing classes unaffected.

---

### E6 — Remove file I/O from core library (Profile B, C)

**Files:** `YAML_FileSource.hpp`, `YAML_File.cpp`, `YAML.hpp`

**Current behaviour**
`YAML::fromFile()`, `YAML::toFile()`, `YAML::getFileFormat()`, and
`FileSource`/`FileDestination` include `<fstream>` unconditionally. On bare-metal
targets that don't have a filesystem, including these headers may fail or bring
in unnecessary symbol references.

**Proposed change**

1. Move all file-I/O helpers into a separate `YAML_File.hpp` / `YAML_File.cpp`
   that is conditionally compiled:

   ```cmake
   option(YAML_LIB_FILE_IO "Include file I/O support" ON)
   if(YAML_LIB_FILE_IO)
       target_sources(YAML_Lib PRIVATE classes/source/YAML_File.cpp)
       target_compile_definitions(YAML_Lib PUBLIC YAML_LIB_FILE_IO)
   endif()
   ```

2. Guard the `fromFile`/`toFile` declarations in `YAML.hpp` behind
   `#ifdef YAML_LIB_FILE_IO`.

3. Remove `#include <fstream>` from all core headers — restrict it to
   `YAML_FileSource.hpp` and `YAML_FileDestination.hpp`.

**Benefit**
- Core library compiles cleanly on targets with no filesystem support.
- Reduces binary size on embedded when file I/O is not needed.
- Allows selective linking: embedded binaries omit the file I/O object file.

**Risk / effort:** Low — purely structural reorganisation; no logic changes.

---

### E7 — Eliminate `std::function` from the parser routing table (Profile B, C)

**Files:** `Default_Parser.hpp`

**Current behaviour**
The 16-entry parser routing table uses `std::function<bool(ISource&)>` and
`std::function<Node(ISource&, const Delimiters&, unsigned long)>`. `std::function`
has a non-trivial destructor and may allocate for type-erased callables.

**Proposed change**

Replace `std::function` with plain function pointers. All routing table entries
are already free functions (static member functions), so their addresses are
plain function pointers — no capture, no closure, no allocation:

```cpp
using IsAFunc   = bool (*)(ISource &);
using ParseFunc = Node (*)(ISource &, const Delimiters &, unsigned long);
using ParserEntry = std::pair<IsAFunc, ParseFunc>;
inline static constexpr std::array<ParserEntry, 16> parsers{{
    {isArray,    parseArray},
    {isDictionary, parseDictionary},
    // ...
}};
```

Mark `parsers` as `constexpr` — the entire dispatch table lives in ROM.

**Benefit**
- Eliminates 16 `std::function` destructor calls and any small-buffer-optimisation overhead.
- `constexpr` table placed in `.rodata` — saves RAM on Harvard-architecture MCUs.
- Removes the last STL type-erasure mechanism from the hot parse path.

**Risk / effort:** Very low — mechanical substitution; all entries are already function pointers.

---

### E8 — Configurable `Timestamp` parsing (Profile B, C)

**Files:** `YAML_Timestamp.hpp`, parser timestamp handling

**Current behaviour**
`Timestamp` stores the raw string value without conversion. Parsing runs a
regex-style character-by-character validation. Users who need actual
`time_t` or `struct tm` fields must re-parse the string themselves.

**Proposed change**

1. Add a compile-time policy for Timestamp handling:

   ```cmake
   option(YAML_LIB_TIMESTAMP_PARSE "Parse timestamps to broken-down time" ON)
   ```

2. Under `YAML_LIB_TIMESTAMP_PARSE=OFF`, `Timestamp` stores only the raw
   `std::string_view` (pointing into the source buffer — zero allocation for
   read-only parses) rather than copying to a `std::string`.

3. Under `YAML_LIB_TIMESTAMP_PARSE=ON` (default), add `toTimeT()` /
   `toTm()` conversion accessors using `<ctime>` (available even on embedded).

**Benefit**
- Eliminates `std::string` allocation per Timestamp node on bare-metal
  (store as `string_view` into ROM).
- Provides ready-to-use `time_t` for embedded RTC integration.

**Risk / effort:** Low — additive. The `string_view` lifetime must be bounded to the source buffer.

---

### E9 — YAML schema validation / whitelisting (Profile B, C)

**Current behaviour**
The library is a general-purpose YAML parser. On embedded systems the YAML
schema is typically fixed (a configuration file with known keys and value
ranges). There is no built-in way to reject unexpected keys or validate value
ranges without application code doing so after parsing.

**Proposed change**

Add a lightweight schema descriptor that can be compiled into the firmware:

```cpp
struct FieldSchema {
    const char  *key;
    NodeType     expectedType;    // String, Number, Boolean, Array, Dictionary
    bool         required;
};
struct Schema {
    const FieldSchema *fields;
    std::size_t        count;
};

// Usage
constexpr FieldSchema kConfigFields[] = {
    {"device_id",  NodeType::Number,  true},
    {"enabled",    NodeType::Boolean, true},
    {"name",       NodeType::String,  false},
};
constexpr Schema kConfigSchema{kConfigFields, 3};

Result<void> validateAgainst(const Node &document, const Schema &schema);
```

`validateAgainst` performs a single pass: checks that all required keys are
present and that each key's value is the expected type. The schema array lives
in ROM.

**Benefit**
- Detects misconfigured embedded devices at startup rather than crashing later.
- Schema arrays are `constexpr`, stored in ROM — zero RAM cost.
- Replaces ad-hoc `isA<>` checks scattered through application code.

**Risk / effort:** Medium — new feature. Implementation is straightforward; the
schema struct is purely declarative.

---

### E10 — Stream-based (SAX-style) parse API (Profile C)

**Current behaviour**
The only parse API builds a complete in-memory tree (`yamlTree`). For a
10 KB YAML configuration file this means the entire parsed tree lives in RAM
simultaneously, even if the application only needs 3 of the 50 keys.

**Proposed change**

Add a SAX-style event callback API that processes YAML without building a tree:

```cpp
struct IYAMLEvents {
    virtual void onDocumentStart()                              = 0;
    virtual void onDocumentEnd()                                = 0;
    virtual void onMappingStart()                               = 0;
    virtual void onMappingEnd()                                 = 0;
    virtual void onSequenceStart()                              = 0;
    virtual void onSequenceEnd()                                = 0;
    virtual void onKey   (std::string_view key)                 = 0;
    virtual void onScalar(NodeType type, std::string_view value)= 0;
};

// Usage (embedded — filter only the keys you care about)
class ConfigReader final : public IYAMLEvents {
    void onKey   (std::string_view k) override { currentKey_ = k; }
    void onScalar(NodeType, std::string_view v) override {
        if (currentKey_ == "device_id") config_.deviceId = parseU32(v);
        else if (currentKey_ == "enabled") config_.enabled = (v == "true");
    }
    // ...
};

YAML::parseEvents(src, configReader);  // No tree built, no heap required
```

**Benefit**
- **Zero heap:** parser emits events, application reads into fixed structs.
- Peak RAM is proportional to YAML nesting depth, not total file size.
- Enables parsing 100 KB YAML files on a 32 KB RAM Cortex-M0.

**Risk / effort:** High — requires a separate event-emitting pass in the parser.
The existing recursive descent parser can be adapted by inserting event calls at
each node construction site. The existing tree-building API remains unchanged.

---

## Implementation Order

| Priority | Phase | Profile | Effort | Benefit |
|----------|-------|---------|--------|---------|
| 1 | E6 — separate file I/O | A, B, C | Low | Cleaner build system; required for E3+ |
| 2 | E7 — function pointers for routing table | A, B, C | Very low | ROM-able table; smaller binary |
| 3 | E5 — SpanSource + FixedDestination | B, C | Low | Zero-copy ROM parse; bounded output |
| 4 | E2 — remove global parser state | A, B, C | High | Thread safety; RTOS compatibility |
| 5 | E1 — exception-free error reporting | B, C | High | `-fno-exceptions` build support |
| 6 | E8 — Timestamp string_view option | B, C | Low | Eliminate per-Timestamp allocation |
| 7 | E3 — PMR / pluggable allocator | B, C | Medium-high | Arena allocation; heap control |
| 8 | E4 — static-capacity containers | C | Medium | Zero-heap tree on bare-metal |
| 9 | E9 — schema validation | B, C | Medium | Config validation at startup |
| 10 | E10 — SAX-style event API | C | High | Streaming parse with zero heap |

---

## CMake Feature Flags (Summary)

```cmake
# Add to CMakeLists.txt
option(YAML_LIB_NO_EXCEPTIONS  "Disable exception throwing; use error callbacks" OFF)
option(YAML_LIB_FILE_IO        "Include file I/O helpers (FileSource, fromFile)" ON)
option(YAML_LIB_TIMESTAMP_PARSE "Enable timestamp to struct tm conversion"       ON)
option(YAML_LIB_STATIC_CONTAINERS "Use fixed-capacity containers (requires N)"  OFF)
option(YAML_LIB_SAX_API        "Build SAX event-based parser API"               OFF)
```

All flags default to the current behaviour so existing users are unaffected.

---

## Minimum Viable Embedded Port

For **Profile B** (RTOS, e.g. FreeRTOS on STM32):
Implement E6 + E7 + E5 + E1.  This gives:
- No `<fstream>` in core.
- Function-pointer dispatch table in ROM.
- `SpanSource` for ROM-backed YAML.
- `-fno-exceptions` safe build.
- Remaining heap use (strings, vectors) acceptable with FreeRTOS heap4.

For **Profile C** (bare-metal Cortex-M0):
All of the above plus E2 + E3 + E4 + E10.  Achieves a fully stack/static
allocation path with a SAX-style read-only parse of fixed YAML schemas from
flash.

---

## Known Constraints That Cannot Be Eliminated

| Constraint | Reason |
|------------|--------|
| C++17 minimum | `std::variant`, `std::string_view`, `std::from_chars`, structured bindings are load-bearing |
| `std::string` in mutable String nodes | YAML values must be stored somewhere; only removable with E4 (static) or E10 (SAX, no storage) |
| Per-document `unique_ptr<Array/Document>` | Recursive `std::variant` type requires indirection to avoid infinite sizeof; eliminable only with E4 |
| `std::unordered_map` in Dictionary | Required for O(1) key lookup; removable with E4 (linear scan on static array) |
