#pragma once

namespace YAML_Lib {

struct Array final : Variant {
  using Entry = Node;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  Array() : Variant(Type::array) {}
  Array(const Array &other) = default;
  Array &operator=(const Array &other) = default;
  Array(Array &&other) = default;
  Array &operator=(Array &&other) = default;
  ~Array() override = default;
  // Add array element
  void add(Entry yNode) { yNodeArray.emplace_back(std::move(yNode)); }
  // Return the size of an array
  [[nodiscard]] std::size_t size() const { return yNodeArray.size(); }
  // Return reference to array base
  Entries &value() { return yNodeArray; }
  [[nodiscard]] const Entries &value() const { return yNodeArray; }
  // Convert variant to a key
  [[nodiscard]]  std::string toKey() const override {
    std::string array{kLeftSquareBracket};
    if (!yNodeArray.empty()) {
      size_t commaCount = yNodeArray.size() - 1;
      for (auto &entryNode : yNodeArray) {
        if (const auto type = entryNode.getVariant().getNodeType(); type == Type::dictionary || type == Type::array) {
          array += entryNode.getVariant().toKey();
        } else {
          array += entryNode.getVariant().toString();
        }

        if (commaCount-- > 0) {
          array += ", ";
        }
      }
    }
    array += "]";
    return array;
  }
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
  void resize(const std::size_t index) {
    yNodeArray.resize(index + 1);
    for (auto &entry : yNodeArray) {
      if (entry.isEmpty()) {
        entry = Node::make<Hole>();
      }
    }
  }

private:
  // Array entries list
  Entries yNodeArray;
};

} // namespace YAML_Lib