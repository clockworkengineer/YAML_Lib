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

  static std::string parseKey(ISource &source);
  static YNode parseBlockString(ISource &source, const std::set<char> &delimiters);
  static YNode parsePipedBlockString(ISource &source,
                              const std::set<char> &delimiters);
  static YNode parseString(ISource &source, const std::set<char> &delimiters);
  static YNode parseQuotedString(ISource &source, const std::set<char> &delimiters);
  static YNode parseComment(ISource &source, [[maybe_unused]] const std::set<char> &delimiters);
  static YNode parseNumber(ISource &source, const std::set<char> &delimiters);
  static YNode parseNone(ISource &source, const std::set<char> &delimiters);
  static YNode parseBoolean(ISource &source, const std::set<char> &delimiters);
  static YNode parseAnchor(ISource &source, const std::set<char> &delimiters);
  static YNode parseAlias(ISource &source, const std::set<char> &delimiters);
  static YNode parseArray(ISource &source, unsigned long indentLevel,
                   const std::set<char> &delimiters);
  static YNode parseInlineArray(ISource &source, unsigned long indentLevel,
                                [[maybe_unused]] const std::set<char> &delimiters);
  static DictionaryEntry parseKeyValue(ISource &source, unsigned long indentLevel,
                                const std::set<char> &delimiters);
  static YNode parseDictionary(ISource &source, unsigned long indentLevel,
                        const std::set<char> &delimiters);
  static YNode parseInlineDictionary(ISource &source, unsigned long indentLevel,
                                     [[maybe_unused]] const std::set<char> &delimiters);
  static YNode parseDocument(ISource &source, [[maybe_unused]] unsigned long indentLevel,
                      const std::set<char> &delimiters);
};

} // namespace YAML_Lib