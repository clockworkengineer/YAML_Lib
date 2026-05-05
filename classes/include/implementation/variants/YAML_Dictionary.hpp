#pragma once

#include <unordered_map>

namespace YAML_Lib {

// Dictionary entry
struct DictionaryEntry {
  DictionaryEntry(const std::string_view &key, Node &yNode, char quote = kNull)
      : yNodeKey(key), yNodeKeyQuote(quote), yNode(std::move(yNode)) {}
  DictionaryEntry(const std::string_view &key, Node &&yNode,
                  char quote = kNull)
      : yNodeKey(key), yNodeKeyQuote(quote), yNode(std::move(yNode)) {}
  DictionaryEntry(Node &keyNode, Node &yNode)
      : yNodeKey(std::get<String>(keyNode.getVariant()).toString()),
        yNodeKeyQuote(std::get<String>(keyNode.getVariant()).getQuote()),
        yNode(std::move(yNode)) {}
  DictionaryEntry(Node &keyNode, Node &&yNode)
      : yNodeKey(std::get<String>(keyNode.getVariant()).toString()),
        yNodeKeyQuote(std::get<String>(keyNode.getVariant()).getQuote()),
        yNode(std::move(yNode)) {}
  [[nodiscard]] std::string_view getKey() const { return yNodeKey; }
  [[nodiscard]] char getKeyQuote() const { return yNodeKeyQuote; }
  [[nodiscard]] Node &getNode() { return yNode; }
  [[nodiscard]] const Node &getNode() const { return yNode; }

private:
  std::string yNodeKey;
  char yNodeKeyQuote{kNull};
  Node yNode;
};

struct Dictionary {
  using Entry = DictionaryEntry;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  explicit Dictionary() = default;
  Dictionary(const Dictionary &other) = delete;
  Dictionary &operator=(const Dictionary &other) = delete;
  Dictionary(Dictionary &&other) = default;
  Dictionary &operator=(Dictionary &&other) = default;
  ~Dictionary() = default;
  // Add Entry to Dictionary; also registers the key in the O(1) lookup index
  template <typename T> void add(T &&entry) {
    const std::string key{entry.getKey()};
    yNodeDictionaryIndex[key] = yNodeDictionary.size();
    yNodeDictionary.emplace_back(std::forward<T>(entry));
  }
  // Return true if a dictionary contains a given key (O(1), no exception)
  [[nodiscard]] bool contains(const std::string_view &key) const noexcept {
    return yNodeDictionaryIndex.count(std::string(key)) != 0;
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
  std::unordered_map<std::string, std::size_t> yNodeDictionaryIndex;
};

inline Dictionary::Entries::iterator
Dictionary::findKey(const std::string_view &key) {
  const auto indexIt = yNodeDictionaryIndex.find(std::string(key));
  if (indexIt == yNodeDictionaryIndex.end()) {
    YAML_THROW(Node::Error, "Invalid key used to access dictionary.");
  }
  return yNodeDictionary.begin() + static_cast<std::ptrdiff_t>(indexIt->second);
}
inline Dictionary::Entries::const_iterator
Dictionary::findKey(const std::string_view &key) const {
  const auto indexIt = yNodeDictionaryIndex.find(std::string(key));
  if (indexIt == yNodeDictionaryIndex.end()) {
    YAML_THROW(Node::Error, "Invalid key used to access dictionary.");
  }
  return yNodeDictionary.cbegin() + static_cast<std::ptrdiff_t>(indexIt->second);
}
} // namespace YAML_Lib