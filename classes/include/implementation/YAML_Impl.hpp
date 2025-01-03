
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
    unsigned long numberOfDocuments = 0;
    for (auto &yNode : yamlTree) {
      if (isA<Document>(yNode)) {
        numberOfDocuments++;
      }
    }
    return numberOfDocuments;
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
    return yamlTree[index];
  }
  [[nodiscard]] const YNode &document(const unsigned long index) const {
    if (index >= yamlTree.size()) {
      throw Error("Document does not exist.");
    }
    return yamlTree[index];
  }
  // Search for YAML object entry with a given key
  YNode &operator[](const std::string &key);
  const YNode &operator[](const std::string &key) const;
  // Get YAML array element at index
  YNode &operator[](std::size_t index);
  const YNode &operator[](std::size_t index) const;
  
private:
  // Pointer to YAML parser interface
  inline static std::unique_ptr<IParser> yamlParser;
  // Pointer to YAML stringify interface
  inline static std::unique_ptr<IStringify> yamlStringify;
  // YAML tree
  std::vector<YNode> yamlTree;
};
} // namespace YAML_Lib
