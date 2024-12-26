
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
    long numberOfDocuments = static_cast<long>(index);
    for (auto &yNode : yamlTree) {
      if (isA<Document>(yNode)) {
        if (--numberOfDocuments < 0) {
          return yNode;
        }
      }
    }
    throw Error("Document does not exist.");
  }
  [[nodiscard]] const YNode &document(const unsigned long index) const {
    long numberOfDocuments = static_cast<long>(index);
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
  // YAML translator
  YAML_Translator yamlTranslator;
  // Pointer to YAML parser interface
  inline static std::unique_ptr<IParser> yamlParser;
  // Pointer to YAML stringify interface
  inline static std::unique_ptr<IStringify> yamlStringify;
  // YAML tree
  std::vector<YNode> yamlTree;
};
} // namespace YAML_Lib
