
#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Parser.hpp"
#include "YAML_Stringify.hpp"

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
  [[nodiscard]] unsigned long getNumberOfDocuments() const {
    return yamlTree.size();
  }
  // Parse YAML into YNode tree
  void parse(ISource &source);
  // Create YAML text string from YNode tree
  void stringify(IDestination &destination) const;
  // Get the root of YAML tree
  [[nodiscard]] std::vector<YNode> &root() { return yamlTree; }
  [[nodiscard]] const std::vector<YNode> &root() const { return yamlTree; }
  // Get the document
  [[nodiscard]] YNode &document(const unsigned long index) {
    if (index >= yamlTree.size()) {
      throw Error("Document does not exist.");
    }
    return yamlTree[index][0];
  }
  [[nodiscard]] const YNode &document(const unsigned long index) const {
    if (index >= yamlTree.size()) {
      throw Error("Document does not exist.");
    }
    return yamlTree[index][0];
  }
  // Traverse YAML tree
  void traverse(IAction &action);
  void traverse(IAction &action) const;
  // Search for YAML object entry with a given key
  YNode &operator[](const std::string &key);
  const YNode &operator[](const std::string &key) const;
  // Get YAML array element at index
  YNode &operator[](std::size_t index);
  const YNode &operator[](std::size_t index) const;
  // Read/Write YAML from a file
  static std::string fromFile(const std::string &fileName);
  static void toFile(const std::string &fileName, const std::string &yamlString, YAML::Format format);
  // Get YAML file format
  static YAML::Format getFileFormat(const std::string &fileName);

private:
  // Traverse YAML tree
  template <typename T> static void traverseYNodes(T &yNode, IAction &action);
  // Pointer to YAML parser interface
  inline static std::unique_ptr<IParser> yamlParser;
  // Pointer to YAML stringify interface
  inline static std::unique_ptr<IStringify> yamlStringify;
  // YAML tree
  std::vector<YNode> yamlTree;
};

/// <summary>
/// Recursively traverse YNode tree calling IAction methods and possibly
/// modifying the tree contents or even structure.
/// </summary>
/// <param name="yNode">YNode tree to be traversed.</param>
/// <param name="action">Action methods to call during traversal.</param>
template <typename T>
void YAML_Impl::traverseYNodes(T &yNode, IAction &action) {
  action.onYNode(yNode);
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
    for (auto &entry : YRef<Dictionary>(yNode).value()) {
      traverseYNodes(entry.getYNode(), action);
    }
  } else if (isA<Array>(yNode)) {
    action.onArray(yNode);
    for (auto &entry : YRef<Array>(yNode).value()) {
      traverseYNodes(entry, action);
    }
  } else if (!isA<Hole>(yNode)) {
    throw Error("Unknown YNode type encountered during tree traversal.");
  }
}
} // namespace YAML_Lib
