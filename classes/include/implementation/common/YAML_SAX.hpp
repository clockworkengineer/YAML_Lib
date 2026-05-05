#pragma once

#ifdef YAML_LIB_SAX_API

// =============================================================================
// E10 — SAX-style (streaming) event API for YAML_Lib
//
// IYAMLEvents defines the callback interface.  emitEvents() walks an
// already-built Node tree and fires the callbacks in document order — the same
// sequence a true streaming parser would produce.
//
// Usage:
//   class ConfigReader final : public YAML_Lib::IYAMLEvents {
//     void onKey(std::string_view k) override { lastKey_ = k; }
//     void onScalar(YAML_Lib::NodeType t, std::string_view v) override {
//       if (lastKey_ == "port") port_ = std::stoi(std::string(v));
//     }
//     std::string_view lastKey_;
//     int port_{0};
//   };
//
//   const YAML yaml;
//   yaml.parse(BufferSource{rawYaml});
//
//   ConfigReader reader;
//   yaml.traverseEvents(reader);   // fires SAX events from the built tree
//
// Note: traverseEvents() walks the existing tree — it does not skip heap
// allocation for the tree itself.  For a future fully-streaming zero-heap
// implementation, the parser would need to be adapted to call the callbacks
// during the parse pass (replacing Node construction sites with event fires).
// The interface here is intentionally identical to what that future
// implementation would expose.
// =============================================================================

namespace YAML_Lib {

// ---------------------------------------------------------------------------
// IYAMLEvents — SAX callback interface.
//
// Default implementations are no-ops so callers only override what they need.
// ---------------------------------------------------------------------------
class IYAMLEvents {
public:
  virtual ~IYAMLEvents() = default;

  // Document boundary
  virtual void onDocumentStart() {}
  virtual void onDocumentEnd()   {}

  // Mapping (Dictionary) boundary
  virtual void onMappingStart()  {}
  virtual void onMappingEnd()    {}

  // Sequence (Array) boundary
  virtual void onSequenceStart() {}
  virtual void onSequenceEnd()   {}

  // Dictionary key (fired immediately before the associated value event)
  virtual void onKey(std::string_view /*key*/) {}

  // Scalar leaf value — type comes from NodeType (YAML_Schema.hpp / E9)
  virtual void onScalar(NodeType /*type*/, std::string_view /*value*/) {}
};

// ---------------------------------------------------------------------------
// emitEvents — walk a Node tree and fire IYAMLEvents callbacks.
//
// Recursive; depth proportional to YAML nesting level only.
// ---------------------------------------------------------------------------
inline void emitEvents(const Node &node, IYAMLEvents &handler) {
  if (isA<String>(node)) {
    handler.onScalar(NodeType::String, NRef<String>(node).value());
  } else if (isA<Number>(node)) {
    handler.onScalar(NodeType::Number, NRef<Number>(node).toString());
  } else if (isA<Boolean>(node)) {
    handler.onScalar(NodeType::Boolean,
                     NRef<Boolean>(node).value() ? "true" : "false");
  } else if (isA<Null>(node)) {
    handler.onScalar(NodeType::Null, "null");
  } else if (isA<Timestamp>(node)) {
    handler.onScalar(NodeType::Timestamp, NRef<Timestamp>(node).value());
  } else if (isA<Dictionary>(node)) {
    handler.onMappingStart();
    for (const auto &entry : NRef<Dictionary>(node).value()) {
      handler.onKey(entry.getKey());
      emitEvents(entry.getNode(), handler);
    }
    handler.onMappingEnd();
  } else if (isA<Array>(node)) {
    handler.onSequenceStart();
    for (const auto &elem : NRef<Array>(node).value()) {
      emitEvents(elem, handler);
    }
    handler.onSequenceEnd();
  }
  // Hole / Anchor / Comment: silently skipped (internal-only nodes)
}

} // namespace YAML_Lib

#endif // YAML_LIB_SAX_API
