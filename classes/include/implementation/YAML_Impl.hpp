#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class YAML_Impl {
public:
  // Constructors/Destructors
  YAML_Impl(IStringify *stringify, IParser *parser);
  YAML_Impl(const YAML_Impl &other) = delete;
  YAML_Impl &operator=(const YAML_Impl &other) = delete;
  YAML_Impl(YAML_Impl &&other) = delete;
  YAML_Impl &operator=(YAML_Impl &&other) = delete;
  ~YAML_Impl() = default;
  // Get YAML_Lib version
  static std::string version();
  // Get number of documents
  [[nodiscard]] auto getNumberOfDocuments() const {
    return yamlTree.size();
  }
  // Parse YAML into Node tree
  void parse(ISource &source);
  // Create YAML text string from Node tree
  void stringify(IDestination &destination) const;
  // Get the document
  [[nodiscard]] Node &document(const unsigned long index) {
    if (index >= yamlTree.size()) {
      throw Error("Document does not exist.");
    }
    return yamlTree[index][0];
  }
  [[nodiscard]] const Node &document(const unsigned long index) const {
    if (index >= yamlTree.size()) {
      throw Error("Document does not exist.");
    }
    return yamlTree[index][0];
  }
  // Traverse YAML tree
  void traverse(IAction &action);
  void traverse(IAction &action) const;
  // Search for YAML object entry with a given key
  Node &operator[](const std::string_view &key);
  const Node &operator[](const std::string_view &key) const;
  // Get YAML array element at index
  Node &operator[](std::size_t index);
  const Node &operator[](std::size_t index) const;
  // Read/Write YAML from a file
  static std::string fromFile(const std::string_view &fileName);
  static void toFile(const std::string_view &fileName, const std::string_view &yamlString, YAML::Format format);
  // Get YAML file format
  static YAML::Format getFileFormat(const std::string_view &fileName);

private:
  // Traverse YAML tree
  template <typename T> static void traverseNodes(T &yNode, IAction &action);
  // Pointer to YAML parser interface
  inline static std::unique_ptr<IParser> yamlParser;
  // Pointer to YAML stringify interface
  inline static std::unique_ptr<IStringify> yamlStringify;
  // YAML tree
  std::vector<Node> yamlTree;
};

/// <summary>
/// Recursively traverse Node tree calling IAction methods and possibly
/// modifying the tree contents or even structure.
/// </summary>
/// <param name="yNode">Node tree to be traversed.</param>
/// <param name="action">Action methods to call during traversal.</param>
template <typename T>
void YAML_Impl::traverseNodes(T &yNode, IAction &action) {
  action.onNode(yNode);
  if (isA<Number>(yNode)) {
    action.onNumber(yNode);
  } else if (isA<String>(yNode)) {
    action.onString(yNode);
  } else if (isA<Boolean>(yNode)) {
    action.onBoolean(yNode);
  } else if (isA<Null>(yNode)) {
    action.onNull(yNode);
  } else if (isA<Dictionary>(yNode)) {
    action.onDictionary(yNode);
    for (auto &entry : NRef<Dictionary>(yNode).value()) {
      traverseNodes(entry.getNode(), action);
    }
  } else if (isA<Array>(yNode)) {
    action.onArray(yNode);
    for (auto &entry : NRef<Array>(yNode).value()) {
      traverseNodes(entry, action);
    }
  } else if (!isA<Hole>(yNode)) {
    throw Error("Unknown Node type encountered during tree traversal.");
  }
}
} // namespace YAML_Lib
