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
      YAML_THROW(Node::Error, "Node not a string.");
  } else if constexpr (std::is_same_v<T, Number>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not a number.");
  } else if constexpr (std::is_same_v<T, Array>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not an array.");
  } else if constexpr (std::is_same_v<T, Dictionary>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not an dictionary.");
  } else if constexpr (std::is_same_v<T, Boolean>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not a boolean.");
  } else if constexpr (std::is_same_v<T, Null>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not a null.");
  } else if constexpr (std::is_same_v<T, Hole>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not a hole.");
  } else if constexpr (std::is_same_v<T, Comment>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not a comment.");
  } else if constexpr (std::is_same_v<T, Document>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not a document.");
  } else if constexpr (std::is_same_v<T, Timestamp>) {
    if (!isA<T>(yNode))
      YAML_THROW(Node::Error, "Node not a timestamp.");
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
  if (!entries_.empty()) {
    std::size_t commaCount = entries_.size() - 1;
    for (const auto &entryNode : entries_) {
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

// SequenceBase<Derived>::resize() — grow sequence and fill new slots with Hole nodes.
// Shared by Array and Document; defined here after Node::make<Hole>() is available.
template <typename Derived>
inline void SequenceBase<Derived>::resize(const std::size_t index) {
  entries_.resize(index + 1);
  for (auto &entry : entries_) {
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

// -----------------------------------------------------------------------
// StaticSequenceBase<N, Derived>::resize() — defined here after Node::make<Hole>()
template <std::size_t N, typename Derived>
inline void StaticSequenceBase<N, Derived>::resize(const std::size_t index) {
  if (index >= N) {
    YAML_THROW(Node::Error, "Static sequence capacity exceeded during resize.");
  }
  const std::size_t newSize = index + 1;
  if (newSize > this->count_) {
    this->count_ = newSize;
  }
  for (std::size_t i = 0; i < this->count_; ++i) {
    if (this->entries_[i].isEmpty()) {
      this->entries_[i] = Node::make<Hole>();
    }
  }
}

// StaticArray<N>::toKey() — same logic as Array::toKey()
template <std::size_t N>
inline std::string StaticArray<N>::toKey() const {
  std::string result{kLeftSquareBracket};
  if (this->count_ > 0) {
    std::size_t commaCount = this->count_ - 1;
    for (std::size_t i = 0; i < this->count_; ++i) {
      result += this->entries_[i].toString();
      if (commaCount-- > 0) {
        result += ", ";
      }
    }
  }
  result += "]";
  return result;
}

// StaticDictionary<N>::toKey() — build "{k: v, ...}" key string
template <std::size_t N>
inline std::string StaticDictionary<N>::toKey() const {
  std::string result{kLeftCurlyBrace};
  for (std::size_t i = 0; i < count_; ++i) {
    result += keys_[i];
    result += ": ";
    result += values_[i].toString();
    if (i + 1 < count_) {
      result += ", ";
    }
  }
  result += "}";
  return result;
}

} // namespace YAML_Lib