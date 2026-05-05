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

// nodeName<T>() — compile-time article+noun for each node type.
// Returns the full "a string" / "an array" phrase so that checkNode<T>()
// can produce the same error text as the previous per-branch chain.
template <typename T>
consteval std::string_view nodeName() noexcept {
  if constexpr (std::is_same_v<T, String>)          return "a string";
  else if constexpr (std::is_same_v<T, Number>)     return "a number";
  else if constexpr (std::is_same_v<T, Array>)      return "an array";
  else if constexpr (std::is_same_v<T, Dictionary>) return "an dictionary";
  else if constexpr (std::is_same_v<T, Boolean>)    return "a boolean";
  else if constexpr (std::is_same_v<T, Null>)       return "a null";
  else if constexpr (std::is_same_v<T, Hole>)       return "a hole";
  else if constexpr (std::is_same_v<T, Comment>)    return "a comment";
  else if constexpr (std::is_same_v<T, Document>)   return "a document";
  else if constexpr (std::is_same_v<T, Timestamp>)  return "a timestamp";
  else                                               return "unknown";
}

template <typename T> void checkNode(const Node &yNode) {
  if (!isA<T>(yNode))
    YAML_THROW(Node::Error,
               std::string("Node not ").append(nodeName<T>()).append("."));
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

// Helper: visit a NodeVariant and call toString() or toKey() on the active type.
// The four concrete overloads (monostate, Array, Dictionary, Document) are
// identical for both operations; only the generic template differs.
namespace detail {
enum class TextMode { ToString, ToKey };

template <typename T>
inline std::string pointerContainerToKey(const std::unique_ptr<T> &p) {
  if constexpr (std::is_same_v<T, Document>) {
    return "";
  } else {
    return p->toKey();
  }
}

template <TextMode Mode>
struct NodeTextVisitor {
  std::string operator()(const std::monostate &) const { return ""; }
  template <typename T>
  std::string operator()(const std::unique_ptr<T> &p) const {
    return pointerContainerToKey(p);
  }
  template <typename T> std::string operator()(const T &v) const {
    if constexpr (Mode == TextMode::ToString) return v.toString();
    else                                      return v.toKey();
  }
};

template <typename ElementAccessor>
inline std::string sequenceToKey(const char leftBracket,
                                 const char rightBracket,
                                 const std::size_t count,
                                 ElementAccessor accessor) {
  std::string result;
  result += leftBracket;
  if (count > 0) {
    std::size_t commaCount = count - 1;
    for (std::size_t i = 0; i < count; ++i) {
      result += accessor(i);
      if (commaCount-- > 0) {
        result += ", ";
      }
    }
  }
  result += rightBracket;
  return result;
}

template <typename EntryAccessor>
inline std::string dictionaryToKey(const std::size_t count,
                                   EntryAccessor accessor) {
  std::string result{kLeftCurlyBrace};
  for (std::size_t i = 0; i < count; ++i) {
    result += accessor(i);
    if (i + 1 < count) {
      result += ", ";
    }
  }
  result += kRightCurlyBrace;
  return result;
}
} // namespace detail

inline std::string Node::toString() const {
  return std::visit(detail::NodeTextVisitor<detail::TextMode::ToString>{}, yNodeVariant);
}
inline std::string Node::toKey() const {
  return std::visit(detail::NodeTextVisitor<detail::TextMode::ToKey>{}, yNodeVariant);
}

// Array::toKey() — build "[a, b, c]" key string
inline std::string Array::toKey() const {
  return detail::sequenceToKey('[', ']', entries_.size(),
                              [this](const std::size_t index) {
                                return this->entries_[index].toString();
                              });
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
  return detail::dictionaryToKey(yNodeDictionary.size(),
                                 [this](const std::size_t index) {
                                   const auto &entryNode = yNodeDictionary[index];
                                   return std::string(entryNode.getKey()) + ": " +
                                          entryNode.getNode().toString();
                                 });
}

// -----------------------------------------------------------------------
// StaticSequenceBase<N, Derived>::resize() — defined here after Node::make<Hole()>
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
  return detail::sequenceToKey('[', ']', this->count_,
                              [this](const std::size_t index) {
                                return this->entries_[index].toString();
                              });
}

// StaticDictionary<N>::toKey() — build "{k: v, ...}" key string
template <std::size_t N>
inline std::string StaticDictionary<N>::toKey() const {
  return detail::dictionaryToKey(this->count_,
                                 [this](const std::size_t index) {
                                   return keys_[index] + std::string(": ") +
                                          values_[index].toString();
                                 });
}

} // namespace YAML_Lib