#pragma once

namespace YAML_Lib {

// Node special member function definitions and Node::make (defined here where container types are
// complete)
inline Node::Node() = default;
inline Node::Node(Node&&) noexcept = default;
inline Node& Node::operator=(Node&&) noexcept = default;
inline Node::~Node() = default;

template <typename T, typename... Args>
inline Node Node::make(Args&&... args) {
  Node n;
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    n.getVariant() = std::make_unique<T>(std::forward<Args>(args)...);
  } else {
    n.getVariant() = T(std::forward<Args>(args)...);
  }
  return n;
}

// =======================
// What is Node variant ?
// =======================
template <typename T>
bool isA(const Node& yNode) {
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
  if constexpr (std::is_same_v<T, String>)
    return "a string";
  else if constexpr (std::is_same_v<T, Number>)
    return "a number";
  else if constexpr (std::is_same_v<T, Array>)
    return "an array";
  else if constexpr (std::is_same_v<T, Dictionary>)
    return "an dictionary";
  else if constexpr (std::is_same_v<T, Boolean>)
    return "a boolean";
  else if constexpr (std::is_same_v<T, Null>)
    return "a null";
  else if constexpr (std::is_same_v<T, Hole>)
    return "a hole";
  else if constexpr (std::is_same_v<T, Comment>)
    return "a comment";
  else if constexpr (std::is_same_v<T, Document>)
    return "a document";
  else if constexpr (std::is_same_v<T, Timestamp>)
    return "a timestamp";
  else
    return "unknown";
}

template <typename T>
void checkNode(const Node& yNode) {
  if (!isA<T>(yNode))
    YAML_THROW(Node::Error, std::string("Node not ").append(nodeName<T>()).append("."));
}

template <typename T>
T& NRef(Node& yNode) {
  checkNode<T>(yNode);
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return *std::get<std::unique_ptr<T>>(yNode.getVariant());
  } else {
    return std::get<T>(yNode.getVariant());
  }
}
template <typename T>
const T& NRef(const Node& yNode) {
  checkNode<T>(yNode);
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return *std::get<std::unique_ptr<T>>(yNode.getVariant());
  } else {
    return std::get<T>(yNode.getVariant());
  }
}
template <typename T>
T& NRef(Dictionary::Entry& yNodeEntry) {
  checkNode<T>(yNodeEntry.getNode());
  if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                std::is_same_v<T, Document>) {
    return *std::get<std::unique_ptr<T>>(yNodeEntry.getNode().getVariant());
  } else {
    return std::get<T>(yNodeEntry.getNode().getVariant());
  }
}
template <typename T>
const T& NRef(const Dictionary::Entry& yNodeEntry) {
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
inline std::string pointerContainerToKey(const std::unique_ptr<T>& p) {
  if constexpr (std::is_same_v<T, Document>) {
    return "";
  } else {
    return p->toKey();
  }
}

template <TextMode Mode>
struct NodeTextVisitor {
  std::string operator()(const std::monostate&) const { return ""; }
  template <typename T>
  std::string operator()(const std::unique_ptr<T>& p) const {
    return pointerContainerToKey(p);
  }
  template <typename T>
  std::string operator()(const T& v) const {
    if constexpr (Mode == TextMode::ToString)
      return v.toString();
    else
      return v.toKey();
  }
};

template <typename ElementAccessor>
inline std::string sequenceToKey(const char leftBracket, const char rightBracket,
                                 const std::size_t count, ElementAccessor accessor) {
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
inline std::string dictionaryToKey(const std::size_t count, EntryAccessor accessor) {
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
}  // namespace detail

/// <summary>
/// Return a string representation of the node.
/// </summary>
/// <returns>String representation of the current node.</returns>
inline std::string Node::toString() const {
  return std::visit(detail::NodeTextVisitor<detail::TextMode::ToString>{}, yNodeVariant);
}
/// <summary>
/// Return a key string for the node.
/// </summary>
/// <returns>Key string representation of the current node.</returns>
inline std::string Node::toKey() const {
  return std::visit(detail::NodeTextVisitor<detail::TextMode::ToKey>{}, yNodeVariant);
}

// Array::toKey() — build "[a, b, c]" key string
inline std::string Array::toKey() const {
  return detail::sequenceToKey('[', ']', entries_.size(), [this](const std::size_t index) {
    return this->entries_[index].toString();
  });
}

// SequenceBase<Derived>::resize() — grow sequence and fill new slots with Hole nodes.
// Shared by Array and Document; defined here after Node::make<Hole>() is available.
template <typename Derived>
/// <summary>
/// Resize the sequence to the requested index and populate new elements with holes.
/// </summary>
/// <param name="index">Desired final index in the sequence.</param>
inline void SequenceBase<Derived>::resize(const std::size_t index) {
  entries_.resize(index + 1);
  for (auto& entry : entries_) {
    if (entry.isEmpty()) {
      entry = Node::make<Hole>();
    }
  }
}

// Dictionary::toKey() — build "{k: v, ...}" key string
inline std::string Dictionary::toKey() const {
  return detail::dictionaryToKey(yNodeDictionary.size(), [this](const std::size_t index) {
    const auto& entryNode = yNodeDictionary[index];
    return std::string(entryNode.getKey()) + ": " + entryNode.getNode().toString();
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
/// <summary>
/// Return a string representation for the static array key.
/// </summary>
/// <returns>String representation of the static array key.</returns>
inline std::string StaticArray<N>::toKey() const {
  return detail::sequenceToKey('[', ']', this->count_, [this](const std::size_t index) {
    return this->entries_[index].toString();
  });
}

// StaticDictionary<N>::toKey() — build "{k: v, ...}" key string
template <std::size_t N>
/// <summary>
/// Return a string representation for the static dictionary key.
/// </summary>
/// <returns>String representation of the static dictionary key.</returns>
inline std::string StaticDictionary<N>::toKey() const {
  return detail::dictionaryToKey(this->count_, [this](const std::size_t index) {
    return keys_[index] + std::string(": ") + values_[index].toString();
  });
}

// Node::clone() — deep copy scalar or container node
inline Node Node::clone() const {
  Node copy;
  copy.yamlTag = yamlTag;
  std::visit(
      [&copy](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          // empty monostate
        } else if constexpr (std::is_same_v<T, std::unique_ptr<Array>>) {
          if (val) {
            auto newArr = std::make_unique<Array>();
            for (const auto& elem : val->value()) {
              newArr->add(elem.clone());
            }
            copy.yNodeVariant = std::move(newArr);
          }
        } else if constexpr (std::is_same_v<T, std::unique_ptr<Dictionary>>) {
          if (val) {
            auto newDict = std::make_unique<Dictionary>();
            for (const auto& entry : val->value()) {
              newDict->add(
                  DictionaryEntry(entry.getKey(), entry.getNode().clone(), entry.getKeyQuote()));
            }
            copy.yNodeVariant = std::move(newDict);
          }
        } else if constexpr (std::is_same_v<T, std::unique_ptr<Document>>) {
          if (val) {
            auto newDoc = std::make_unique<Document>();
            for (const auto& elem : val->value()) {
              newDoc->add(elem.clone());
            }
            copy.yNodeVariant = std::move(newDoc);
          }
        } else {
          copy.yNodeVariant = val;
        }
      },
      yNodeVariant);
  return copy;
}

// Forward declaration of deserialization hook for user types
template <typename T>
void from_yaml(const Node& n, T& val);

// ============================================================================
// Modern C++23 type query and conversion accessors
// ============================================================================

template <typename T>
bool Node::is() const noexcept {
  using CleanT = std::remove_cvref_t<T>;
  if constexpr (std::is_same_v<CleanT, std::string> || std::is_same_v<CleanT, std::string_view>) {
    return isA<String>(*this);
  } else if constexpr (std::is_same_v<CleanT, bool>) {
    return isA<Boolean>(*this);
  } else if constexpr (std::is_integral_v<CleanT> || std::is_floating_point_v<CleanT>) {
    return isA<Number>(*this);
  } else if constexpr (std::is_same_v<CleanT, Array>) {
    return isA<Array>(*this);
  } else if constexpr (std::is_same_v<CleanT, Dictionary>) {
    return isA<Dictionary>(*this);
  } else if constexpr (std::is_same_v<CleanT, Timestamp>) {
    return isA<Timestamp>(*this);
  } else if constexpr (std::is_same_v<CleanT, Null> || std::is_null_pointer_v<CleanT>) {
    return isA<Null>(*this);
  } else if constexpr (std::is_same_v<CleanT, String> || std::is_same_v<CleanT, Number> ||
                       std::is_same_v<CleanT, Boolean> || std::is_same_v<CleanT, Hole> ||
                       std::is_same_v<CleanT, Comment> || std::is_same_v<CleanT, Document>) {
    return isA<CleanT>(*this);
  } else {
    return false;
  }
}

template <typename T>
decltype(auto) Node::as() {
  using CleanT = std::remove_cvref_t<T>;
  if constexpr (std::is_same_v<CleanT, std::string>) {
    if (isA<String>(*this)) {
      return std::string(NRef<String>(*this).value());
    } else if (isA<Number>(*this)) {
      return NRef<Number>(*this).toString();
    } else if (isA<Boolean>(*this)) {
      return NRef<Boolean>(*this).toKey();
    }
    return NRef<String>(*this).toString();
  } else if constexpr (std::is_same_v<CleanT, std::string_view>) {
    return NRef<String>(*this).value();
  } else if constexpr (std::is_same_v<CleanT, bool>) {
    return NRef<Boolean>(*this).value();
  } else if constexpr (std::is_integral_v<CleanT> || std::is_floating_point_v<CleanT>) {
    return NRef<Number>(*this).value<CleanT>();
  } else if constexpr (std::is_same_v<CleanT, Array>) {
    return NRef<Array>(*this);
  } else if constexpr (std::is_same_v<CleanT, Dictionary>) {
    return NRef<Dictionary>(*this);
  } else if constexpr (std::is_same_v<CleanT, Timestamp>) {
    return NRef<Timestamp>(*this);
  } else if constexpr (std::is_same_v<CleanT, String>) {
    return NRef<String>(*this);
  } else if constexpr (std::is_same_v<CleanT, Number>) {
    return NRef<Number>(*this);
  } else if constexpr (std::is_same_v<CleanT, Boolean>) {
    return NRef<Boolean>(*this);
  } else {
    CleanT val{};
    from_yaml(*this, val);
    return val;
  }
}

template <typename T>
decltype(auto) Node::as() const {
  using CleanT = std::remove_cvref_t<T>;
  if constexpr (std::is_same_v<CleanT, std::string>) {
    if (isA<String>(*this)) {
      return std::string(NRef<String>(*this).value());
    } else if (isA<Number>(*this)) {
      return NRef<Number>(*this).toString();
    } else if (isA<Boolean>(*this)) {
      return NRef<Boolean>(*this).toKey();
    }
    return NRef<String>(*this).toString();
  } else if constexpr (std::is_same_v<CleanT, std::string_view>) {
    return NRef<String>(*this).value();
  } else if constexpr (std::is_same_v<CleanT, bool>) {
    return NRef<Boolean>(*this).value();
  } else if constexpr (std::is_integral_v<CleanT> || std::is_floating_point_v<CleanT>) {
    return NRef<Number>(*this).value<CleanT>();
  } else if constexpr (std::is_same_v<CleanT, Array>) {
    return NRef<Array>(*this);
  } else if constexpr (std::is_same_v<CleanT, Dictionary>) {
    return NRef<Dictionary>(*this);
  } else if constexpr (std::is_same_v<CleanT, Timestamp>) {
    return NRef<Timestamp>(*this);
  } else if constexpr (std::is_same_v<CleanT, String>) {
    return NRef<String>(*this);
  } else if constexpr (std::is_same_v<CleanT, Number>) {
    return NRef<Number>(*this);
  } else if constexpr (std::is_same_v<CleanT, Boolean>) {
    return NRef<Boolean>(*this);
  } else {
    CleanT val{};
    from_yaml(*this, val);
    return val;
  }
}

template <typename T>
T Node::value_or(T&& fallback) const {
  if (isEmpty() || isA<Null>(*this)) {
    return std::forward<T>(fallback);
  }
#if !defined(YAML_LIB_NO_EXCEPTIONS) && defined(__cpp_exceptions)
  try {
    return as<std::remove_cvref_t<T>>();
  } catch (...) {
    return std::forward<T>(fallback);
  }
#else
  if (is<std::remove_cvref_t<T>>()) {
    return as<std::remove_cvref_t<T>>();
  }
  return std::forward<T>(fallback);
#endif
}

template <typename T>
T Node::value_or(const std::string_view& key, T&& fallback) const {
  if (isA<Dictionary>(*this) && NRef<Dictionary>(*this).contains(key)) {
    return (*this)[key].value_or(std::forward<T>(fallback));
  }
  return std::forward<T>(fallback);
}

template <typename T>
std::optional<T> Node::get_if() const {
  if (is<T>()) {
#if !defined(YAML_LIB_NO_EXCEPTIONS) && defined(__cpp_exceptions)
    try {
      return as<T>();
    } catch (...) {
      return std::nullopt;
    }
#else
    return as<T>();
#endif
  }
  return std::nullopt;
}

template <typename>
decltype(auto) Node::items() {
  return NRef<Dictionary>(*this).items();
}

template <typename>
decltype(auto) Node::items() const {
  return NRef<Dictionary>(*this).items();
}

}  // namespace YAML_Lib
