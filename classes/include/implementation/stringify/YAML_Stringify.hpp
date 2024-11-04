#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class YAML_Stringify final : public IStringify {
public:
  YAML_Stringify() = default;
  YAML_Stringify(const YAML_Stringify &other) = delete;
  YAML_Stringify &operator=(const YAML_Stringify &other) = delete;
  YAML_Stringify(YAML_Stringify &&other) = delete;
  YAML_Stringify &operator=(YAML_Stringify &&other) = delete;
  ~YAML_Stringify() override = default;
  // Stringify YNode tree
  void stringify(const std::vector<YNode> &yamlTree,
                 IDestination &destination) const override;
  // Indentation increment
  void setIndentation(unsigned long indentation) {
    yamlIndentation = indentation;
  }

private:
  void stringifyYAML(IDestination &destination, const YNode &yNode,
                     unsigned long indent) const;
  unsigned long yamlIndentation{2};
};

} // namespace YAML_Lib