#pragma once

namespace YAML_Lib {

// Dictionary entry
struct DictionaryEntry {
  DictionaryEntry(const std::string_view &key, Node &yNode)
      : key(Node::make<String>(key, kNull)), yNode(std::move(yNode)) {}
  DictionaryEntry(const std::string_view &key, Node &&yNode)
      : key(Node::make<String>(key, kNull)), yNode(std::move(yNode)) {}
  DictionaryEntry(Node &keyNode, Node &yNode)
      : key(std::move(keyNode)), yNode(std::move(yNode)) {}
  DictionaryEntry(Node &keyNode, Node &&yNode)
      : key(std::move(keyNode)), yNode(std::move(yNode)) {}
  [[nodiscard]] std::string_view getKey() {
    return dynamic_cast<String &>(key.getVariant()).value();
  }
  [[nodiscard]] std::string_view getKey() const {
    return dynamic_cast<const String &>(key.getVariant()).value();
  }
  [[nodiscard]] Node &getKeyNode() { return key; }
  [[nodiscard]] const Node &getKeyNode() const { return key; }
  [[nodiscard]] Node &getNode() { return yNode; }
  [[nodiscard]] const Node &getNode() const { return yNode; }

private:
  Node key;
  Node yNode;
};

struct Dictionary final : Variant {
  using Entry = DictionaryEntry;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  explicit Dictionary() : Variant(Type::dictionary) {}
  Dictionary(const Dictionary &other) = default;
  Dictionary &operator=(const Dictionary &other) = default;
  Dictionary(Dictionary &&other) = default;
  Dictionary &operator=(Dictionary &&other) = default;
  ~Dictionary() override = default;
  // Add Entry to Dictionary
  template <typename T> void add(T &&entry) {
    yNodeDictionary.emplace_back(std::forward<T>(entry));
  }
  // Return true if a dictionary contains a given key
  [[nodiscard]] bool contains(const std::string_view &key) const {
    try {
      [[maybe_unused]] auto _ = findKey(yNodeDictionary, key);
    } catch ([[maybe_unused]] const Node::Error &e) {
      return false;
    }
    return true;
  }
  // Return number of entries in a dictionary
  [[nodiscard]] int size() const {
    return static_cast<int>(yNodeDictionary.size());
  }
  // Return dictionary entry for a given key
  Node &operator[](const std::string_view &key) {
    return findKey(yNodeDictionary, key)->getNode();
  }
  const Node &operator[](const std::string_view &key) const {
    return findKey(yNodeDictionary, key)->getNode();
  }
  // Return reference to base of dictionary entries
  Entries &value() { return yNodeDictionary; }
  [[nodiscard]] const Entries &value() const { return yNodeDictionary; }
  // Convert variant to a key
  [[nodiscard]]  std::string toKey() const override {
    std::string dictionary{kLeftCurlyBrace};
    size_t commaCount = yNodeDictionary.size() - 1;
    for (auto &entryNode : yNodeDictionary) {
      dictionary += entryNode.getKeyNode().getVariant().toString();
      dictionary += ": ";
      if (const auto type = entryNode.getNode().getVariant().getNodeType(); type == Type::dictionary || type == Type::array) {
        dictionary += entryNode.getNode().getVariant().toKey();
      } else {
        dictionary += entryNode.getNode().getVariant().toString();
      }
      if (commaCount-- > 0) {
        dictionary += ", ";
      }
    }
    dictionary += "}";
    return dictionary;
  }

private:
  // Search for a given entry given a key and dictionary list
  [[nodiscard]] static Entries::iterator findKey(Entries &dictionary,
                                                 const std::string_view &key);
  [[nodiscard]] static Entries::const_iterator
  findKey(const Entries &dictionary, const std::string_view &key);

  // Dictionary entries list
  Entries yNodeDictionary;
};

inline Dictionary::Entries::iterator
Dictionary::findKey(Entries &dictionary, const std::string_view &key) {
  const auto it =
      std::ranges::find_if(dictionary, [&key](Entry &entry) -> bool {
        return entry.getKey() == key;
      });
  if (it == dictionary.end()) {
    throw Node::Error("Invalid key used to access dictionary.");
  }
  return it;
}
inline Dictionary::Entries::const_iterator
Dictionary::findKey(const Entries &dictionary, const std::string_view &key) {
  const auto it =
      std::ranges::find_if(dictionary, [&key](const Entry &entry) -> bool {
        return entry.getKey() == key;
      });
  if (it == dictionary.end()) {
    throw Node::Error("Invalid key used to access dictionary.");
  }
  return it;
}
} // namespace YAML_Lib