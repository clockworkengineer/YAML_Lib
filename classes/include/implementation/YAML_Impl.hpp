
#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

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
    unsigned int numberOfDocuments = 0;
    for (auto &yNode : yamlYNodeTree) {
      if (isA<Document>(yNode)) {
        numberOfDocuments++;
      }
    }
    return numberOfDocuments;
  }
  // Parse YAML into YNode tree
  void parse(ISource &source);
  // Create YAML text string (no white space) from YNode tree
  void stringify(IDestination &destination) const;
  // Get root of JSON tree
  [[nodiscard]] std::vector<YNode> &root() { return yamlYNodeTree; }
  [[nodiscard]] const std::vector<YNode> &root() const { return yamlYNodeTree; }
  // Get document
  [[nodiscard]] YNode &document(unsigned long index) { return yamlYNodeTree[0]; }
  [[nodiscard]] const YNode &document(unsigned long index) const { return yamlYNodeTree[0]; }
private:
  // Yaml tree
  std::vector<YNode> yamlYNodeTree;
    // Yaml document
  // std::vector<YNode> yamlDocuments;
  // Root of YAML tree
  YNode yNodeRoot;
};
} // namespace YAML_Lib
