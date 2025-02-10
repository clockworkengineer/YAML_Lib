#pragma once

namespace YAML_Lib {

// Dictionary error
struct DictionaryError final : std::runtime_error {
  explicit DictionaryError(const std::string &message)
      : std::runtime_error("Dictionary Error: " + message) {}
};

// Dictionary entry
struct DictionaryEntry {
  DictionaryEntry(const std::string &key, YNode &yNode)
      : key(YNode::make<String>(key, kNull)), yNode(std::move(yNode)) {}
  DictionaryEntry(const std::string &key, YNode &&yNode)
      : key(YNode::make<String>(key, kNull)), yNode(std::move(yNode)) {}
  DictionaryEntry(YNode &keyYNode, YNode &yNode)
      : key(std::move(keyYNode)), yNode(std::move(yNode)) {}
  DictionaryEntry(YNode &keyYNode, YNode &&yNode)
      : key(std::move(keyYNode)), yNode(std::move(yNode)) {}
  [[nodiscard]] std::string &getKey() {
    return dynamic_cast<String &>(key.getVariant()).value();
  }
  [[nodiscard]] const std::string &getKey() const {
    return dynamic_cast<const String &>(key.getVariant()).value();
  }
  [[nodiscard]] YNode &getKeyYNode() { return key; }
  [[nodiscard]] const YNode &getKeyYNode() const { return key; }
  [[nodiscard]] YNode &getYNode() { return yNode; }
  [[nodiscard]] const YNode &getYNode() const { return yNode; }

private:
  YNode key;
  YNode yNode;
};

struct Dictionary final : Variant {
  using Error = DictionaryError;
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
  [[nodiscard]] bool contains(const std::string &key) const {
    try {
      [[maybe_unused]] auto _ = findKey(yNodeDictionary, key);
    } catch ([[maybe_unused]] const Error &e) {
      return false;
    }
    return true;
  }
  // Return number of entries in a dictionary
  [[nodiscard]] int size() const {
    return static_cast<int>(yNodeDictionary.size());
  }
  // Return dictionary entry for a given key
  YNode &operator[](const std::string &key) {
    return findKey(yNodeDictionary, key)->getYNode();
  }
  const YNode &operator[](const std::string &key) const {
    return findKey(yNodeDictionary, key)->getYNode();
  }
  // Return reference to base of dictionary entries
  Entries &value() { return yNodeDictionary; }
  [[nodiscard]] const Entries &value() const { return yNodeDictionary; }
  // Convert variant to a key
  [[nodiscard]] const std::string toKey() const override {
    std::string dictionary{kLeftCurlyBrace};
    size_t commaCount = yNodeDictionary.size() - 1;
    for (auto &entryYNode : yNodeDictionary) {
      dictionary += entryYNode.getKeyYNode().getVariant().toString();
      dictionary += ": ";
      if (const auto type = entryYNode.getYNode().getVariant().getNodeType(); type == Type::dictionary || type == Type::array) {
        dictionary += entryYNode.getYNode().getVariant().toKey();
      } else {
        dictionary += entryYNode.getYNode().getVariant().toString();
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
                                                 const std::string &key);
  [[nodiscard]] static Entries::const_iterator
  findKey(const Entries &dictionary, const std::string &key);

  // Dictionary entries list
  Entries yNodeDictionary;
};

inline Dictionary::Entries::iterator
Dictionary::findKey(Entries &dictionary, const std::string &key) {
  const auto it =
      std::ranges::find_if(dictionary, [&key](Entry &entry) -> bool {
        return entry.getKey() == key;
      });
  if (it == dictionary.end()) {
    throw Error("Invalid key used to access dictionary.");
  }
  return it;
}
inline Dictionary::Entries::const_iterator
Dictionary::findKey(const Entries &dictionary, const std::string &key) {
  const auto it =
      std::ranges::find_if(dictionary, [&key](const Entry &entry) -> bool {
        return entry.getKey() == key;
      });
  if (it == dictionary.end()) {
    throw Error("Invalid key used to access dictionary.");
  }
  return it;
}
} // namespace YAML_Lib