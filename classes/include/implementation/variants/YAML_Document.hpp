#pragma once

namespace YAML_Lib {

struct Document final : Variant {
  using Entry = Node;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  Document()
      : Variant(Type::document) {
  } // document treated as an array so passes indexing checks
  Document(const Document &other) = default;
  Document &operator=(const Document &other) = default;
  Document(Document &&other) = default;
  Document &operator=(Document &&other) = default;
  ~Document() override = default;
  // Add document element
  void add(Entry yNode) { yNodeDocument.emplace_back(std::move(yNode)); }
  // Return the size of document
  [[nodiscard]] std::size_t size() const { return yNodeDocument.size(); }
  // Return reference to document base
  Entries &value() { return yNodeDocument; }
  [[nodiscard]] const Entries &value() const { return yNodeDocument; }
  Node &operator[](const std::size_t index) {
    if (index < yNodeDocument.size()) {
      return yNodeDocument[index];
    }
    throw Node::Error("Invalid index used to access document.");
  }
  const Node &operator[](const std::size_t index) const {
    if (index < yNodeDocument.size()) {
      return yNodeDocument[index];
    }
    throw Node::Error("Invalid index used to access document.");
  }
  // Resize Document
  void resize(const std::size_t index) {
    yNodeDocument.resize(index + 1);
    for (auto &entry : yNodeDocument) {
      if (entry.isEmpty()) {
        entry = Node::make<Hole>();
      }
    }
  }

private:
  // Document entries list
  Entries yNodeDocument;
};

} // namespace YAML_Lib