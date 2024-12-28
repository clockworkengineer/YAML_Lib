#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Translator.hpp"

namespace YAML_Lib {

class YAML_Stringify final : public IStringify {
public:
  explicit YAML_Stringify(std::shared_ptr<ITranslator>translator)  { yamlTranslator = translator; }
  YAML_Stringify(const YAML_Stringify &other) = delete;
  YAML_Stringify &operator=(const YAML_Stringify &other) = delete;
  YAML_Stringify(YAML_Stringify &&other) = delete;
  YAML_Stringify &operator=(YAML_Stringify &&other) = delete;
  ~YAML_Stringify() override = default;
  // Stringify YNode tree
  void stringify(const std::vector<YNode> &yamlTree,
                 IDestination &destination) const override;
  // Indentation increment
  static void setIndentation(const unsigned long indentation) {
    yamlIndentation = indentation;
  }
private:
  // Stringify root
  static void stringifyYAML(IDestination &destination, const YNode &yNode,
                            unsigned long indent);
  // Current indentation level
  inline static unsigned long yamlIndentation{2};
  // Translator
  inline static std::shared_ptr<ITranslator>yamlTranslator;
};

} // namespace YAML_Lib