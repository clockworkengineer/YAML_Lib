#pragma once

namespace YAML_Lib {

// =======================
// What is Node variant ?
// =======================
template <typename T> bool isA(const Node &yNode) {
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return std::holds_alternative<std::unique_ptr<T>>(yNode.getVariant());
  } else {
    return std::holds_alternative<T>(yNode.getVariant());
  }
}

// =========================
// Node reference converter
// =========================
template <typename T> void checkNode(const Node &yNode) {
  if constexpr (std::is_same_v<T, String>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a string.");
  } else if constexpr (std::is_same_v<T, Number>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a number.");
  } else if constexpr (std::is_same_v<T, Array>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not an array.");
  } else if constexpr (std::is_same_v<T, Dictionary>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not an dictionary.");
  } else if constexpr (std::is_same_v<T, Boolean>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a boolean.");
  } else if constexpr (std::is_same_v<T, Null>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a null.");
  } else if constexpr (std::is_same_v<T, Hole>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a hole.");
  } else if constexpr (std::is_same_v<T, Comment>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a comment.");
  } else if constexpr (std::is_same_v<T, Document>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a document.");
  } else if constexpr (std::is_same_v<T, Timestamp>) {
    if (!isA<T>(yNode))
      throw Node::Error("Node not a timestamp.");
  }
}

template <typename T> T &NRef(Node &yNode) {
  checkNode<T>(yNode);
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return *std::get<std::unique_ptr<T>>(yNode.getVariant());
  } else {
    return std::get<T>(yNode.getVariant());
  }
}
template <typename T> const T &NRef(const Node &yNode) {
  checkNode<T>(yNode);
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return *std::get<std::unique_ptr<T>>(yNode.getVariant());
  } else {
    return std::get<T>(yNode.getVariant());
  }
}
template <typename T> T &NRef(Dictionary::Entry &yNodeEntry) {
  checkNode<T>(yNodeEntry.getNode());
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return *std::get<std::unique_ptr<T>>(yNodeEntry.getNode().getVariant());
  } else {
    return std::get<T>(yNodeEntry.getNode().getVariant());
  }
}
template <typename T> const T &NRef(const Dictionary::Entry &yNodeEntry) {
  checkNode<T>(yNodeEntry.getNode());
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return *std::get<std::unique_ptr<T>>(yNodeEntry.getNode().getVariant());
  } else {
    return std::get<T>(yNodeEntry.getNode().getVariant());
  }
}

// ======================================================
// Post-definitions requiring all variant types complete
// ======================================================

// Helper: visit a NodeVariant and call toString()/toKey() on the active type.
namespace detail {
struct ToStringVisitor {
  std::string operator()(const std::monostate &) const { return ""; }
  std::string operator()(const std::unique_ptr<Array> &p) const {
    return p->toKey();
  }
  std::string operator()(const std::unique_ptr<Dictionary> &p) const {
    return p->toKey();
  }
  std::string operator()(const std::unique_ptr<Document> &) const {
    return "";
  }
  template <typename T> std::string operator()(const T &v) const {
    return v.toString();
  }
};
struct ToKeyVisitor {
  std::string operator()(const std::monostate &) const { return ""; }
  std::string operator()(const std::unique_ptr<Array> &p) const {
    return p->toKey();
  }
  std::string operator()(const std::unique_ptr<Dictionary> &p) const {
    return p->toKey();
  }
  std::string operator()(const std::unique_ptr<Document> &) const {
    return "";
  }
  template <typename T> std::string operator()(const T &v) const {
    return v.toKey();
  }
};
} // namespace detail

inline std::string Node::toString() const {
  return std::visit(detail::ToStringVisitor{}, yNodeVariant);
}
inline std::string Node::toKey() const {
  return std::visit(detail::ToKeyVisitor{}, yNodeVariant);
}

// Array::toKey() — build "[a, b, c]" key string
inline std::string Array::toKey() const {
  std::string result{kLeftSquareBracket};
  if (!yNodeArray.empty()) {
    std::size_t commaCount = yNodeArray.size() - 1;
    for (const auto &entryNode : yNodeArray) {
      // Matches old behavior: toString() dispatches to toKey() for containers
      result += entryNode.toString();
      if (commaCount-- > 0) {
        result += ", ";
      }
    }
  }
  result += "]";
  return result;
}

// Array::resize() — grow array and fill new slots with Hole nodes
inline void Array::resize(const std::size_t index) {
  yNodeArray.resize(index + 1);
  for (auto &entry : yNodeArray) {
    if (entry.isEmpty()) {
      entry = Node::make<Hole>();
    }
  }
}

// Dictionary::toKey() — build "{k: v, ...}" key string
inline std::string Dictionary::toKey() const {
  std::string result{kLeftCurlyBrace};
  std::size_t commaCount = yNodeDictionary.size() > 0
                               ? yNodeDictionary.size() - 1
                               : 0;
  for (const auto &entryNode : yNodeDictionary) {
    result += std::string(entryNode.getKey());
    result += ": ";
    // toString() dispatches to toKey() for containers, toString() for scalars
    result += entryNode.getNode().toString();
    if (commaCount-- > 0) {
      result += ", ";
    }
  }
  result += "}";
  return result;
}

// Document::resize() — grow document and fill new slots with Hole nodes
inline void Document::resize(const std::size_t index) {
  yNodeDocument.resize(index + 1);
  for (auto &entry : yNodeDocument) {
    if (entry.isEmpty()) {
      entry = Node::make<Hole>();
    }
  }
}

} // namespace YAML_Lib