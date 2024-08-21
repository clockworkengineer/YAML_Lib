
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
  // Get number of documents
  [[nodiscard]] unsigned int getNumberOfDocuments() const {
    return (yamlDocuments.size());
  }
  // Parse YAML into YNode tree
  void parse(ISource &source);
  // Create YAML text string (no white space) from YNode tree
  void stringify(IDestination &destination) const;
  // Get root of JSON tree
  [[nodiscard]] std::vector<YNode> &root() { return yamlDocuments; }
  [[nodiscard]] const std::vector<YNode> &root() const { return yamlDocuments; }
  // Get documents root
  // [[nodiscard]] YNode &root() { return yNodeRoot; }
  // [[nodiscard]] const YNode &root() const { return yNodeRoot; }

private:
  void parseDocuments(ISource &source);
  // YNode parseDocument(ISource &source);
  // Number of documents
  std::vector<YNode> yamlDocuments;
  // Root of YAML tree
  YNode yNodeRoot;
};
} // namespace YAML_Lib
