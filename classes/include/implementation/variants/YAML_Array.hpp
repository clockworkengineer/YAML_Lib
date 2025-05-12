#pragma once

namespace YAML_Lib {

// Array error
struct ArrayError final : std::runtime_error {
  explicit ArrayError(const std::string_view &message)
      : std::runtime_error(std::string("Array Error: ").append(message)) {}
};

struct Array final : Variant {
  using Error = ArrayError;
  using Entry = YNode;
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
  [[nodiscard]] const std::string toKey() const override {
    std::string array{kLeftSquareBracket};
    if (!yNodeArray.empty()) {
      size_t commaCount = yNodeArray.size() - 1;
      for (auto &entryYNode : yNodeArray) {
        if (const auto type = entryYNode.getVariant().getNodeType(); type == Type::dictionary || type == Type::array) {
          array += entryYNode.getVariant().toKey();
        } else {
          array += entryYNode.getVariant().toString();
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
  YNode &operator[](const std::size_t index) {
    if (index < yNodeArray.size()) {
      return yNodeArray[index];
    }
    throw YNode::Error("Invalid index used to access array.");
  }
  const YNode &operator[](const std::size_t index) const {
    if (index < yNodeArray.size()) {
      return yNodeArray[index];
    }
    throw YNode::Error("Invalid index used to access array.");
  }
  // Resize Array
  void resize(const std::size_t index) {
    yNodeArray.resize(index + 1);
    for (auto &entry : yNodeArray) {
      if (entry.isEmpty()) {
        entry = YNode::make<Hole>();
      }
    }
  }

private:
  // Array entries list
  Entries yNodeArray;
};

} // namespace YAML_Lib