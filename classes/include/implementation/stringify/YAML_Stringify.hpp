#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class YAML_Stringify final : public IStringify {
public:
  explicit YAML_Stringify(ITranslator &translator) : translator(translator) {}
  YAML_Stringify(const YAML_Stringify &other) = delete;
  YAML_Stringify &operator=(const YAML_Stringify &other) = delete;
  YAML_Stringify(YAML_Stringify &&other) = delete;
  YAML_Stringify &operator=(YAML_Stringify &&other) = delete;
  ~YAML_Stringify() override = default;
  // Stringify YNode tree
  void stringify(const std::vector<YNode> &yamlTree,
                 IDestination &destination) const override;
  // Indentation increment
  static void setIndentation(unsigned long indentation) {
    yamlIndentation = indentation;
  }
  // Array/dictionaries displayed as inline
  static void setInlineMode(bool mode) { inlineMode = mode; }
  static void stringifyToString(IDestination &destination, const YNode &yNode,
                                unsigned long indent)
  { stringifyYAML(destination, yNode, indent); }

private:
  // Stringify root
  static void stringifyYAML(IDestination &destination, const YNode &yNode,
                            unsigned long indent);
  // Current indentation level
  inline static unsigned long yamlIndentation{2};
  // Stringify inline array/dictionary
  inline static bool inlineMode{false};
  // Translator
  ITranslator &translator;
};

} // namespace YAML_Lib