# DRY Refactoring Plan 12 — Post-Plan 11 Library Sweep

## Goal

Finish the remaining library-wide DRY sweep by eliminating the highest-value repeated implementation patterns in the node/variant and stringify layers. The parser and IO layers already contain several shared abstractions (`BufferedSourceBase`, default `IDestination::add(...)`, `checkAtFlowClose`-style helpers) and will be left stable unless a low-risk duplication is discovered during this pass.

---

## Baseline

Current code already contains these DRY foundations:

- `BufferedSourceBase` shared `next()`, `save()`, `restore()`, `discardSave()`, `reset()`, and `backup()` for `BufferSource`, `SpanSource`, and `FileSource`.
- `IDestination` provides concrete default overloads for `add(std::string_view)`, `add(std::string)`, and `add(const char *)`.
- JSON, XML, and Bencode stringifiers all follow a parallel `stringifyNodes` / `stringifyDocument` design.
- Node variant helpers already centralize type checking and `NRef<T>` extraction.

This plan targets the next layer of shared behavior that remains duplicated in the current library.

---

## Section 1 — Node/variant text conversion

### Problem

`classes/include/implementation/node/YAML_Node_Reference.hpp` contains duplicate overload sets in `detail::NodeTextVisitor` for `std::unique_ptr<Array>`, `std::unique_ptr<Dictionary>`, and `std::unique_ptr<Document>`.

Also, `Array::toKey()` / `StaticArray<N>::toKey()` and `Dictionary::toKey()` / `StaticDictionary<N>::toKey()` are separate implementations with the same structure.

### Fix

1. Extract a shared container text visitor helper in `YAML_Node_Reference.hpp`.
   - Replace repeated `operator()(const std::unique_ptr<Array> &)`, `operator()(const std::unique_ptr<Dictionary> &)`, and `operator()(const std::unique_ptr<Document> &)` bodies with a single template helper that calls either `toKey()` or `toString()` depending on mode.
   - This should remove the duplicated visitor body while preserving compile-time dispatch.

2. Extract generic `toKey()` builders for sequence and dictionary containers.
   - Add one or two free functions in `YAML_Node_Reference.hpp` such as `joinSequenceToKey()` and `joinDictionaryToKey()`.
   - Reimplement `Array::toKey()`, `StaticArray<N>::toKey()`, `Dictionary::toKey()`, and `StaticDictionary<N>::toKey()` using these helpers.

### Files changed

- `classes/include/implementation/node/YAML_Node_Reference.hpp`
- `classes/include/implementation/variants/YAML_Array.hpp`
- `classes/include/implementation/variants/YAML_StaticArray.hpp`
- `classes/include/implementation/variants/YAML_Dictionary.hpp`
- `classes/include/implementation/variants/YAML_StaticDictionary.hpp`

### Expected result

- Removes duplicate `std::visit` handler code.
- Makes container key formatting reusable across array and dictionary implementations.
- Maintains existing public `Node::toString()` / `Node::toKey()` behavior.

### Commit message example

```
DRY Plan 12 Step 1: unify NodeTextVisitor container handlers and container toKey builders
```

---

## Section 2 — Stringify dispatch / document unwrapping

### Problem

The three structured stringifiers in `classes/include/implementation/stringify/JSON_Stringify.hpp`, `XML_Stringify.hpp`, and `Bencode_Stringify.hpp` each implement the same dispatch chain in `stringifyNodes()` and the same `stringifyDocument()` unwrap logic.

This is an instance of repeated control flow that can be centralized without altering per-format semantics.

### Fix

1. Introduce a shared helper family for `stringifyNodes()` dispatch.
   - Add a free helper or small base-class utility in a new header under `classes/include/implementation/stringify/` such as `YAML_Stringify_Helper.hpp`.
   - The helper should encode the common `isA<Document> / isA<Number> / isA<String> / ... / throwUnknownNodeType()` flow and defer format-specific scalar/collection emission to callable lambdas or protected overloads.

2. Extract the common `stringifyDocument()` unwrap into the same helper layer.
   - `stringifyDocument()` in JSON/XML/Bencode should become a one-line call to the shared helper.

3. Leave format-specific string building in the child stringifier files.
   - Only common node-type dispatch and document unwrapping should move to shared code.

### Files changed

- `classes/include/implementation/stringify/JSON_Stringify.hpp`
- `classes/include/implementation/stringify/XML_Stringify.hpp`
- `classes/include/implementation/stringify/Bencode_Stringify.hpp`
- `classes/include/implementation/stringify/YAML_Stringify_Helper.hpp` — new

### Expected result

- Reduces 3x duplicate node dispatch code.
- Makes any future new formatter easier to add by reusing the shared dispatch helper.

### Commit message example

```
DRY Plan 12 Step 2: factor shared stringifyNodes dispatch and document unwrap helper
```

---

## Section 3 — Low-risk helper consolidation

### Candidate cleanup

These are lower-priority but still concrete DRY opportunities if they appear during implementation:

- Consolidate repeated numeric serialization helpers such as `std::to_string(NRef<Number>(yNode).value<long long>())` across JSON and Bencode.
- Introduce a shared `formatBoolLiteral(bool, IDestination &)` helper if the boolean string output is repeated across stringifiers.
- Consolidate shared container iteration patterns used by `JSON_Stringify::stringifyDictionary`, `Bencode_Stringify::stringifyDictionary`, and `XML_Stringify::stringifyDictionary` where possible.

### Approach

Only apply when the shared helper preserves the unique format semantics and does not over-generalize the per-format logic.

### Files likely touched

- `classes/include/implementation/stringify/JSON_Stringify.hpp`
- `classes/include/implementation/stringify/Bencode_Stringify.hpp`
- `classes/include/implementation/stringify/XML_Stringify.hpp`

---

## Execution plan

1. Implement Section 1 refactors first. This is the smallest and most self-contained change set.
2. Run the unit test binary or `ctest` after Section 1.
3. Implement Section 2 next. Verify behavior with tests after the shared helper is in place.
4. Optionally apply Section 3 only if it remains a clear, low-complexity improvement.

> Always test after each step and commit stable refactors independently.

---

## Notes

- Avoid pulling the parser or IO layers into this pass unless an obvious duplicate emerges that is directly adjacent to the node/stringify work.
- Keep the stringifier refactor confined to common control flow; do not merge format-specific output strings.
- Preserve existing public interfaces and semantics for `Node::toString()`, `Node::toKey()`, and `IStringify::stringify(...)`.
