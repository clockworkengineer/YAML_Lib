#pragma once

namespace YAML_Lib {


struct Document : Variant
{
  using Entry = YNode;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  Document() : Variant(Type::array) {}
  Document(const Document &other) = default;
  Document &operator=(const Document &other) = default;
  Document(Document &&other) = default;
  Document &operator=(Document &&other) = default;
  ~Document() = default;
  // Add array element
  void add(Entry yNode) { yNodeDocument.emplace_back(std::move(yNode)); }
  // Return the size of array
  [[nodiscard]] std::size_t size() const { return yNodeDocument.size(); }
  // Return reference to array base
  Entries &value() { return yNodeDocument; }
  [[nodiscard]] const Entries &value() const { return yNodeDocument; }
  // Document indexing operators
  YNode &operator[](const std::size_t index)
  {
    if (index < yNodeDocument.size()) { return yNodeDocument[index]; }
    throw YNode::Error("Invalid index used to access array.");
  }
  const YNode &operator[](const std::size_t index) const
  {
    if (index < yNodeDocument.size()) { return yNodeDocument[index]; }
    throw YNode::Error("Invalid index used to access array.");
  }
  // Resize Document
  void resize(const std::size_t index)
  {
    yNodeDocument.resize(index + 1);
    for (auto &entry : yNodeDocument) {
      if (entry.isEmpty()) { entry = YNode::make<Hole>(); }
    }
  }

private:
  // Document entries list
  Entries yNodeDocument;
};

}// namespace YAML_Lib