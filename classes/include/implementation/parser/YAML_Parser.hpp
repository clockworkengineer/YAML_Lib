#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Translator.hpp"

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
  static bool isValidKey(const std::string &key);

  static bool isKey(ISource &source);

  static bool isArray(ISource &source);

  static bool isBoolean(ISource &source);

  static bool isQuotedString(ISource &source);
  static bool isNumber(ISource &source);

  static bool isNone(ISource &source);

  static bool isBlockString(ISource &source);

  static bool isPipedBlockString(ISource &source);

  static bool isComment(ISource &source);

  static bool isAnchor(ISource &source);
  static bool isAlias(ISource &source);

  static bool isInlineArray(ISource &source);

  static bool isInlineDictionary(ISource &source);

  static bool isDictionary(ISource &source);

  static bool isDefault(ISource &source);

  static void foldCarriageReturns(ISource &source, std::string &yamlString);

  static BlockChomping parseBlockChomping(ISource &source);

  static std::string parseBlockString(ISource &source,
                                      const YAML_Parser::Delimeters &delimiters,
                                      char fillerDefault,
                                      BlockChomping &chomping);
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
  static YNode parseArray(ISource &source, const Delimeters &delimiters);
  static YNode parseInlineArray(ISource &source,
                                [[maybe_unused]] const Delimeters &delimiters);
  static DictionaryEntry parseKeyValue(ISource &source,

                                       const Delimeters &delimiters);
  static YNode parseDictionary(ISource &source, const Delimeters &delimiters);
  static YNode
  parseInlineDictionary(ISource &source,
                        [[maybe_unused]] const Delimeters &delimiters);
  static YNode parseDocument(ISource &source,
                             [[maybe_unused]] const Delimeters &delimiters);

  // inline static YAML_Translator translator {};

  using IsAFunc = std::function<bool(ISource &)>;
  using ParseFunc = std::function<YNode(ISource &, const Delimeters &)>;
  inline static std::vector<std::pair<IsAFunc, ParseFunc>> parsers{
      {isBoolean, parseBoolean},
      {isQuotedString, parseQuotedFlowString},
      {isNumber, parseNumber},
      {isNone, parseNone},
      {isBlockString, parseFoldedBlockString},
      {isPipedBlockString, parseLiteralBlockString},
      {isComment, parseComment},
      {isAnchor, parseAnchor},
      {isAlias, parseAlias},
      {isArray, parseArray},
      {isDictionary, parseDictionary},
      {isInlineArray, parseInlineArray},
      {isInlineDictionary, parseInlineDictionary},
      {isDefault, parsePlainFlowString}};
};

} // namespace YAML_Lib