#pragma once

namespace YAML_Lib {

// Object error
struct ObjectError final : std::runtime_error {
  explicit ObjectError(const std::string &message)
      : std::runtime_error("Object Error: " + message) {}
};

// Object entry
struct ObjectEntry {
  ObjectEntry(const std::string &key, YNode &yNode)
      : key(YNode::make<String>(key)), yNode(std::move(yNode)) {}
  ObjectEntry(const std::string &key, YNode &&yNode)
      : key(YNode::make<String>(key)), yNode(std::move(yNode)) {}
  [[nodiscard]] std::string &getKey() {
    return static_cast<String &>(key.getVariant()).value();
  }
  [[nodiscard]] const std::string &getKey() const {
    return static_cast<const String &>(key.getVariant()).value();
  }
  [[nodiscard]] YNode &getKeyYNode() { return key; }
  [[nodiscard]] const YNode &getKeyYNode() const { return key; }
  [[nodiscard]] YNode &getYNode() { return yNode; }
  [[nodiscard]] const YNode &getYNode() const { return yNode; }

private:
  YNode key;
  YNode yNode;
};

struct Object : Variant {
  using Error = ObjectError;
  using Entry = ObjectEntry;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  Object() : Variant(Type::object) {}
  Object(const Object &other) = default;
  Object &operator=(const Object &other) = default;
  Object(Object &&other) = default;
  Object &operator=(Object &&other) = default;
  ~Object() = default;
  // Add Entry to Object
  template <typename T> void add(T &&entry) {
    yNodeObject.emplace_back(std::forward<T>(entry));
  }
  // Return true if an object contains a given key
  [[nodiscard]] bool contains(const std::string &key) const {
    try {
      [[maybe_unused]] auto _ = findKey(yNodeObject, key);
    } catch ([[maybe_unused]] const Error &e) {
      return false;
    }
    return true;
  }
  // Return number of entries in an object
  [[nodiscard]] int size() const {
    return static_cast<int>(yNodeObject.size());
  }
  // Return object entry for a given key
  YNode &operator[](const std::string &key) {
    return findKey(yNodeObject, key)->getYNode();
  }
  const YNode &operator[](const std::string &key) const {
    return findKey(yNodeObject, key)->getYNode();
  }
  // Return reference to base of object entries
  Entries &value() { return yNodeObject; }
  [[nodiscard]] const Entries &value() const { return yNodeObject; }
  // Get/Set indentation
  unsigned long gegIndentation() { return indentation; }
  void setIndentation(unsigned long indentation) { this->indentation = indentation; }

private:
  // Search for a given entry given a key and object list
  [[nodiscard]] static Entries::iterator findKey(Entries &object,
                                                 const std::string &key);
  [[nodiscard]] static Entries::const_iterator findKey(const Entries &object,
                                                       const std::string &key);

  // Object entries list
  Entries yNodeObject;
  // Array indentation
  unsigned long indentation{};
};

inline Object::Entries::iterator Object::findKey(Entries &object,
                                                 const std::string &key) {
  auto it = std::ranges::find_if(
      object, [&key](Entry &entry) -> bool { return entry.getKey() == key; });
  if (it == object.end()) {
    throw Object::Error("Invalid key used to access object.");
  }
  return it;
}
inline Object::Entries::const_iterator Object::findKey(const Entries &object,
                                                       const std::string &key) {
  auto it = std::ranges::find_if(object, [&key](const Entry &entry) -> bool {
    return entry.getKey() == key;
  });
  if (it == object.end()) {
    throw Object::Error("Invalid key used to access object.");
  }
  return it;
}
} // namespace YAML_Lib