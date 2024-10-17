#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class YAML_Parser final : public IParser {

public:
  using Delimeters = std::set<char>;
  enum class BlockChomping : uint8_t { clip = 0, strip, keep };

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
  static BlockChomping parseBlockChomping(ISource &source);

  static std::string parseBlockString(ISource &source,
                                      const YAML_Parser::Delimeters &delimiters,
                                      char fillerDefault,
                                      unsigned long indentLevel);
  static std::string parseKey(ISource &source);
  static YNode parseFoldedBlockString(ISource &source,
                                      const Delimeters &delimiters);
  static YNode parseLiteralBlockString(ISource &source,
                                       const Delimeters &delimiters);
  static YNode parsePlainFlowString(ISource &source,
                                    const Delimeters &delimiters);
  static YNode parseQuotedFlowString(ISource &source,
                                     const Delimeters &delimiters);
  static YNode parseComment(ISource &source,
                            [[maybe_unused]] const Delimeters &delimiters);
  static YNode parseNumber(ISource &source, const Delimeters &delimiters);
  static YNode parseNone(ISource &source, const Delimeters &delimiters);
  static YNode parseBoolean(ISource &source, const Delimeters &delimiters);
  static YNode parseAnchor(ISource &source, const Delimeters &delimiters);
  static YNode parseAlias(ISource &source, const Delimeters &delimiters);
  static YNode parseArray(ISource &source, unsigned long indentLevel,
                          const Delimeters &delimiters);
  static YNode parseInlineArray(ISource &source, unsigned long indentLevel,
                                [[maybe_unused]] const Delimeters &delimiters);
  static DictionaryEntry parseKeyValue(ISource &source,
                                       unsigned long indentLevel,
                                       const Delimeters &delimiters);
  static YNode parseDictionary(ISource &source, unsigned long indentLevel,
                               const Delimeters &delimiters);
  static YNode
  parseInlineDictionary(ISource &source, unsigned long indentLevel,
                        [[maybe_unused]] const Delimeters &delimiters);
  static YNode parseDocument(ISource &source,
                             [[maybe_unused]] unsigned long indentLevel,
                             const Delimeters &delimiters);
};

} // namespace YAML_Lib