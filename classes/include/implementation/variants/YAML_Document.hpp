#pragma once

namespace YAML_Lib {

struct Document {
  using Entry = Node;
  using Entries = std::vector<Entry>;
  // Constructors/Destructors
  Document() = default;
  Document(const Document &other) = delete;
  Document &operator=(const Document &other) = delete;
  Document(Document &&other) = default;
  Document &operator=(Document &&other) = default;
  ~Document() = default;
  // Add document element
  void add(Entry yNode) { yNodeDocument.emplace_back(std::move(yNode)); }
  // Return the size of document
  [[nodiscard]] std::size_t size() const { return yNodeDocument.size(); }
  // Return reference to document base
  Entries &value() { return yNodeDocument; }
  [[nodiscard]] const Entries &value() const { return yNodeDocument; }
  [[nodiscard]] std::string toString() const { return ""; }
  [[nodiscard]] std::string toKey() const { return ""; }
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
  // Resize Document (defined after all types in YAML_Node_Reference.hpp)
  void resize(const std::size_t index);

private:
  // Document entries list
  Entries yNodeDocument;
};

} // namespace YAML_Lib