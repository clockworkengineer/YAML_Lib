#pragma once

namespace YAML_Lib {

struct Array {
  using Entry = Node;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  Array() = default;
  Array(const Array &other) = delete;
  Array &operator=(const Array &other) = delete;
  Array(Array &&other) = default;
  Array &operator=(Array &&other) = default;
  ~Array() = default;
  // Add array element
  void add(Entry yNode) { yNodeArray.emplace_back(std::move(yNode)); }
  // Return the size of an array
  [[nodiscard]] std::size_t size() const { return yNodeArray.size(); }
  // Return reference to array base
  Entries &value() { return yNodeArray; }
  [[nodiscard]] const Entries &value() const { return yNodeArray; }
  // Convert variant to a key (defined in YAML_Node_Reference.hpp after all types are complete)
  [[nodiscard]] std::string toKey() const;
  // toString for compatibility
  [[nodiscard]] std::string toString() const { return ""; }
  // Array indexing operators
  Node &operator[](const std::size_t index) {
    if (index < yNodeArray.size()) {
      return yNodeArray[index];
    }
    throw Node::Error("Invalid index used to access array.");
  }
  const Node &operator[](const std::size_t index) const {
    if (index < yNodeArray.size()) {
      return yNodeArray[index];
    }
    throw Node::Error("Invalid index used to access array.");
  }
  // Resize Array
  void resize(const std::size_t index);

private:
  // Array entries list
  Entries yNodeArray;
};

} // namespace YAML_Lib