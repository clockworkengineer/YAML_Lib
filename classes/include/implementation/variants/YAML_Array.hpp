#pragma once

namespace YAML_Lib {

// Array error
struct ArrayError final : std::runtime_error
{
  explicit ArrayError(const std::string &message) : std::runtime_error("Array Error: " + message) {}
};

struct Array : Variant
{
  using Error = ArrayError;
  using Entry = YNode;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  Array() : Variant(Type::array) {}
  Array(const Array &other) = default;
  Array &operator=(const Array &other) = default;
  Array(Array &&other) = default;
  Array &operator=(Array &&other) = default;
  ~Array() = default;
  // Add array element
  void add(Entry yNode) { yNodeArray.emplace_back(std::move(yNode)); }
  // Return the size of array
  [[nodiscard]] std::size_t size() const { return yNodeArray.size(); }
  // Return reference to array base
  Entries &value() { return yNodeArray; }
  [[nodiscard]] const Entries &value() const { return yNodeArray; }
  // Array indexing operators
  YNode &operator[](const std::size_t index)
  {
    if (index < yNodeArray.size()) { return yNodeArray[index]; }
    throw YNode::Error("Invalid index used to access array.");
  }
  const YNode &operator[](const std::size_t index) const
  {
    if (index < yNodeArray.size()) { return yNodeArray[index]; }
    throw YNode::Error("Invalid index used to access array.");
  }
  // Resize Array
  void resize(const std::size_t index)
  {
    yNodeArray.resize(index + 1);
    for (auto &entry : yNodeArray) {
      if (entry.isEmpty()) { entry = YNode::make<Hole>(); }
    }
  }
  // Get/Set indentation
  unsigned long getIndentation() { return indentation; }
  void getIndentation(unsigned long indentation) { this->indentation = indentation; }
  
private:
  // Array entries list
  Entries yNodeArray;
  // Array indentation
  unsigned long indentation {};
};

}// namespace YAML_Lib