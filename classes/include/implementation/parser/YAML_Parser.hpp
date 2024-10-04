#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class YAML_Parser final : public IParser {
public:
  YAML_Parser() = default;
  YAML_Parser(const YAML_Parser &other) = delete;
  YAML_Parser &operator=(const YAML_Parser &other) = delete;
  YAML_Parser(YAML_Parser &&other) = delete;
  YAML_Parser &operator=(YAML_Parser &&other) = delete;
  ~YAML_Parser() override = default;

  std::vector<YNode> parse(ISource &source) override;

  // Alias Map
  inline static std::map<std::string, std::string> yamlAliasMap{};

private:
  std::string parseKey(ISource &source);

  YNode parseBlockString(ISource &source, const std::set<char> &delimeters);

  YNode parsePipedBlockString(ISource &source,
                              const std::set<char> &delimeters);

  YNode parseString(ISource &source, const std::set<char> &delimeters);

  YNode parseQuotedString(ISource &source, const std::set<char> &delimeters);

  YNode parseComment(ISource &source, const std::set<char> &delimeters);

  YNode parseNumber(ISource &source, const std::set<char> &delimeters);

  YNode parseNone(ISource &source, const std::set<char> &delimeters);

  YNode parseBoolean(ISource &source, const std::set<char> &delimeters);

  YNode parseAnchor(ISource &source, const std::set<char> &delimeters);
  
  YNode parseAlias(ISource &source, const std::set<char> &delimeters);

  YNode parseArray(ISource &source, unsigned long indentLevel,
                   const std::set<char> &delimeters);

  YNode parseInlineArray(ISource &source, unsigned long indentLevel,
                         const std::set<char> &delimeters);

  DictionaryEntry parseKeyValue(ISource &source, unsigned long indentLevel,
                                const std::set<char> &delimeters);

  YNode parseDictionary(ISource &source, unsigned long indentLevel,
                        const std::set<char> &delimeters);

  YNode parseInlineDictionary(ISource &source, unsigned long indentLevel,
                              const std::set<char> &delimeters);

  YNode parseDocument(ISource &source, unsigned long indentLevel,
                      const std::set<char> &delimeters);
};

} // namespace YAML_Lib