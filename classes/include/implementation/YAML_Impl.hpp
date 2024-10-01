
#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Parser.hpp"
#include "YAML_Stringify.hpp"

namespace YAML_Lib {

class YAML_Impl {
public:
  // Constructors/Destructors
  YAML_Impl() = default;
  YAML_Impl(const YAML_Impl &other) = delete;
  YAML_Impl &operator=(const YAML_Impl &other) = delete;
  YAML_Impl(YAML_Impl &&other) = delete;
  YAML_Impl &operator=(YAML_Impl &&other) = delete;
  ~YAML_Impl() = default;
  // Get YAML_Lib version
  static std::string version();
  // Get number of document
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
  void parse(ISource &source) { yamlTree = parser.parse(source); }
  // Create YAML text string from YNode tree
  void stringify(IDestination &destination) const {
    stringifer.stringify(yamlTree, destination);
  }
  // Get root of JSON tree
  [[nodiscard]] std::vector<YNode> &root() { return yamlTree; }
  [[nodiscard]] const std::vector<YNode> &root() const { return yamlTree; }
  // Get document
  [[nodiscard]] YNode &document(unsigned long index) {
    long numberOfDocuments = index;
    for (auto &yNode : yamlTree) {
      if (isA<Document>(yNode)) {
        if (--numberOfDocuments < 0) {
          return yNode;
        }
      }
    }
    throw Error("Document does not exist.");
  }
  [[nodiscard]] const YNode &document(unsigned long index) const {
    long numberOfDocuments = index;
    for (auto &yNode : yamlTree) {
      if (isA<Document>(yNode)) {
        if (--numberOfDocuments < 0) {
          return yNode;
        }
      }
    }
    throw Error("Document does not exist.");
  }

private:
  // YAML Parser
  YAML_Parser parser;
  // YAML Stringifier
  YAML_Stringify stringifer;
  // YAML tree
  std::vector<YNode> yamlTree;
};
} // namespace YAML_Lib
