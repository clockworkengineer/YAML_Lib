#pragma once

#include <memory_resource>
#include <unordered_map>

namespace YAML_Lib {

struct StringViewHash {
  using is_transparent = void;
  [[nodiscard]] std::size_t operator()(const std::string_view value) const noexcept {
    return std::hash<std::string_view>{}(value);
  }
  [[nodiscard]] std::size_t operator()(const std::string &value) const noexcept {
    return operator()(std::string_view(value));
  }
};

struct StringViewEqual {
  using is_transparent = void;
  [[nodiscard]] bool operator()(const std::string_view lhs,
                                const std::string_view rhs) const noexcept {
    return lhs == rhs;
  }
  [[nodiscard]] bool operator()(const std::string &lhs,
                                const std::string &rhs) const noexcept {
    return lhs == rhs;
  }
  [[nodiscard]] bool operator()(const std::string &lhs,
                                const std::string_view rhs) const noexcept {
    return lhs == rhs;
  }
  [[nodiscard]] bool operator()(const std::string_view lhs,
                                const std::string &rhs) const noexcept {
    return lhs == rhs;
  }
};

// Dictionary entry
struct DictionaryEntry {
  DictionaryEntry(const std::string_view &key, Node yNode, char quote = kNull)
      : yNodeKey(key), yNodeKeyQuote(quote), yNode(std::move(yNode)) {}
  DictionaryEntry(Node &keyNode, Node yNode)
      : yNodeKey(std::get<String>(keyNode.getVariant()).value()),
        yNodeKeyQuote(std::get<String>(keyNode.getVariant()).getQuote()),
        yNode(std::move(yNode)) {}
  [[nodiscard]] std::string_view getKey() const { return yNodeKey; }
  [[nodiscard]] char getKeyQuote() const { return yNodeKeyQuote; }
  [[nodiscard]] Node &getNode() { return yNode; }
  [[nodiscard]] const Node &getNode() const { return yNode; }

private:
  std::pmr::string yNodeKey;
  char yNodeKeyQuote{kNull};
  Node yNode;
};

struct Dictionary {
  using Entry = DictionaryEntry;
  using Entries = std::pmr::vector<Entry>;
  // Constructors/Destructors
  explicit Dictionary() = default;
  Dictionary(const Dictionary &other) = delete;
  Dictionary &operator=(const Dictionary &other) = delete;
  Dictionary(Dictionary &&other) = default;
  Dictionary &operator=(Dictionary &&other) = default;
  ~Dictionary() = default;
  // Add Entry to Dictionary; also registers the key in the O(1) lookup index
  template <typename T> void add(T &&entry) {
    yNodeDictionary.emplace_back(std::forward<T>(entry));
    const auto key = std::string(yNodeDictionary.back().getKey());
    yNodeDictionaryIndex.emplace(std::move(key), yNodeDictionary.size() - 1);
  }
  // Return true if a dictionary contains a given key (O(1), no exception)
  [[nodiscard]] bool contains(const std::string_view &key) const noexcept {
    return yNodeDictionaryIndex.contains(key);
  }
  // Return number of entries in a dictionary
  [[nodiscard]] int size() const {
    return static_cast<int>(yNodeDictionary.size());
  }
  // Return dictionary entry for a given key
  Node &operator[](const std::string_view &key) {
    return findKey(key)->getNode();
  }
  const Node &operator[](const std::string_view &key) const {
    return findKey(key)->getNode();
  }
  // Return reference to base of dictionary entries
  Entries &value() { return yNodeDictionary; }
  [[nodiscard]] const Entries &value() const { return yNodeDictionary; }
  // Convert variant to a key (body defined in YAML_Node_Reference.hpp)
  [[nodiscard]] std::string toKey() const;
  [[nodiscard]] std::string toString() const { return ""; }

private:
  // Search for a given entry by key using the O(1) hash-map index
  [[nodiscard]] Entries::iterator findKey(const std::string_view &key);
  [[nodiscard]] Entries::const_iterator findKey(const std::string_view &key) const;

  // Dictionary entries list (preserves insertion order for stringify)
  Entries yNodeDictionary;
  // Hash-map index: key → position in yNodeDictionary (O(1) lookup)
  std::pmr::unordered_map<std::string, std::size_t,
                          StringViewHash, StringViewEqual>
      yNodeDictionaryIndex;
};

inline Dictionary::Entries::iterator
Dictionary::findKey(const std::string_view &key) {
  const auto indexIt = yNodeDictionaryIndex.find(key);
  if (indexIt == yNodeDictionaryIndex.end()) {
    YAML_THROW(Node::Error, "Invalid key used to access dictionary.");
  }
  return yNodeDictionary.begin() + static_cast<std::ptrdiff_t>(indexIt->second);
}
inline Dictionary::Entries::const_iterator
Dictionary::findKey(const std::string_view &key) const {
  const auto indexIt = yNodeDictionaryIndex.find(key);
  if (indexIt == yNodeDictionaryIndex.end()) {
    YAML_THROW(Node::Error, "Invalid key used to access dictionary.");
  }
  return yNodeDictionary.cbegin() + static_cast<std::ptrdiff_t>(indexIt->second);
}
} // namespace YAML_Lib