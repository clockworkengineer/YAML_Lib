# Size and Performance Reduction Plan

## Goal

Reduce the compiled library size and improve runtime performance while preserving the current YAML 1.2 feature set, API surface, and parser compatibility.

## Observations

- The public CMake target exposes many implementation headers through `target_include_directories(... PUBLIC ...)`, which increases compile-time coupling and may inflate dependency graphs.
- `Default_Parser` uses `std::set<char>` for delimiter sets and many `std::string` allocations during token extraction, which are expensive in a parser hot path.
- `Node` stores its payload as `std::unique_ptr<Variant>`, which means every value/node allocates separately. This creates heap churn and pointer indirection.
- `Default_Parser` holds static `std::map<std::string, std::string>` state for alias/tag prefixes; static containers can hurt binary size and memory usage, and make object lifetime harder.
- The library carries optional translators/stringifiers (JSON, XML, Bencode) inside the core build rather than as optional targets.
- Existing refactoring plans in `notes/DRY_REFACTORING_PLAN_*` already identify duplicate parser control-flow patterns. Those cleanup steps will support later performance work.

## Immediate concrete actions

1. Baseline measurement
   - Add or run a size baseline for the built static library (`YAML_Lib.lib`) and object files.
   - Add parser/stringify benchmark cases for common YAML workloads.
   - Track both binary size and runtime parse/stringify latency.

2. Public API and build cleanup
   - Restrict exported headers to the real public interface: `YAML.hpp`, `YAML_Core.hpp`, and minimal interface headers.
   - Hide implementation headers behind `classes/include/implementation` or move them into a private/internal include directory.
   - Add CMake options for optional output formats and translators, e.g. `BUILD_YAML_JSON`, `BUILD_YAML_XML`, `BUILD_YAML_BENCODE`, `BUILD_YAML_TESTS`.
   - Consider splitting `YAML_Lib` into a compact core target and optional extension targets for stringifiers/translators.

3. Parser hot-path optimization
   - Replace `Default_Parser::Delimiters = std::set<char>` with a faster, smaller representation:
     * `std::array<bool, 256>` or a compile-time `std::bitset<256>`
     * or a small bitmap encoded in `uint64_t`/`std::bitset` for ASCII delimiter tests
   - Reduce or eliminate temporary `std::string` allocations in:
     * `extractToNext`
     * `extractTrimmed`
     * `extractString`
     * `captureIndentedBlock`
     * `parseFromBuffer`
   - Prefer `std::string_view` over `std::string` for input slices when the lifetime can be tied to the source buffer.
   - Change predicate helpers and token extractors to operate on view/slice pairs instead of copy-then-parse.
   - Consolidate repeated scanner patterns and branch logic using helper functions. Complete the `DRY_REFACTORING_PLAN_9` cleanup and use those functions to reduce code size.

4. Node representation and memory reduction
   - Replace the `std::unique_ptr<Variant>` node payload model with a more compact representation:
     * inline `std::variant` if storage size is acceptable
     * or a custom tagged union with small object optimization
     * or a pooled arena allocator for node objects
   - Evaluate whether `String`, `Timestamp`, and `Comment` can store string views into a stable input buffer rather than owning copies immediately.
   - Remove or reduce dynamic allocation for empty/placeholder nodes (`Hole`, `Document`) wherever possible.

5. Reduce runtime polymorphism and optional overhead
   - Use direct `Default_Parser` / `Default_Stringify` code paths for the default library build and keep virtual interfaces for extension points only.
   - If the interface is required for tests/extensions, make the default `YAML` object use a static `Default_Parser` instance or internal type alias behind the scenes.
   - Move optional conversions and translator state out of the hot parse path.

6. Stringify and translator improvements
   - Optimize escaping/emission functions to append directly to the destination rather than building temporary `std::string` values.
   - Reduce allocations in `Default_Stringify`, `XML_Translator`, and `Default_Translator` by using `std::string_view` and destination appends.
   - Consider a specialized fast path for unquoted plain strings.

7. Static data and state reduction
   - Replace global static maps with per-parser instance structures or compact hash maps.
   - Use `std::unordered_map` only if needed; prefer `std::map` only when ordered traversal is required, otherwise use contiguous storage or custom small maps.
   - Remove global state from `Default_Parser` and keep parser state scoped to the active parse call.

8. Verification and regression tracking
   - Add tests for parser correctness after refactor.
   - Add compile-time and runtime regressions checks for size and performance.
   - Use the note file as a living record of which optimizations were completed and which remain.

## Short-term prioritized tasks

1. Convert delimiter handling from `std::set<char>` to a compact lookup and validate parser correctness.
2. Minimize string copies in parser token extraction.
3. Refactor node storage to reduce per-node heap allocations.
4. Hide internal headers and add CMake options for optional modules.
5. Complete the current parser DRY refactoring passes to reduce executable size and branch overhead.

## Expected outcomes

- Smaller compiled `YAML_Lib` static library footprint.
- Faster parsing and stringification for common YAML workloads.
- Lower memory usage during parse tree creation.
- Easier future size/performance work because the public interface is cleaner and the hot-parser code is less duplicated.

---

This plan is intended as an executable checklist for the next optimization cycle.
