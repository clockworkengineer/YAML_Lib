#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Translator.hpp"

namespace YAML_Lib {

class YAML_Parser final : public IParser {

public:
  using Delimiters = std::set<char>;
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
  // YAML parser
  static YNode convertYAMLToStringYNode(const std::string &yamlString);

  static bool isValidKey(const std::string &key);

  static bool isKey(ISource &source);

  static bool isArray(ISource &source);

  static bool isBoolean(ISource &source);

  static bool isQuotedString(ISource &source);
  static bool isNumber(ISource &source);

  static bool isNone(ISource &source);

  static bool isFoldedBlockString(ISource &source);

  static bool isPipedBlockString(ISource &source);

  static bool isComment(ISource &source);

  static bool isAnchor(ISource &source);
  static bool isAlias(ISource &source);

  static bool isInlineArray(ISource &source);

  static bool isInlineDictionary(ISource &source);

  static bool isDictionary(ISource &source);

  static bool isDefault(ISource &source);

  static bool isDocumentStart(ISource &source);

  static bool isDocumentEnd(ISource &source);

  static void appendCharacterToString(ISource &source, std::string &yamlString);

  static std::string extractKey(ISource &source);

  static BlockChomping parseBlockChomping(ISource &source);

  static std::string parseBlockString(ISource &source,
                                      const YAML_Parser::Delimiters &delimiters,
                                      char fillerDefault,
                                      BlockChomping &chomping);
  static YNode parseKey(ISource &source);
  static YNode parseFoldedBlockString(ISource &source,
                                      const Delimiters &delimiters);
  static YNode parseLiteralBlockString(ISource &source,
                                       const Delimiters &delimiters);
  static YNode parsePlainFlowString(ISource &source,
                                    const Delimiters &delimiters);
  static YNode parseQuotedFlowString(ISource &source,
                                     const Delimiters &delimiters);
  static YNode parseComment(ISource &source,
                            [[maybe_unused]] const Delimiters &delimiters);
  static YNode parseNumber(ISource &source, const Delimiters &delimiters);
  static YNode parseNone(ISource &source, const Delimiters &delimiters);
  static YNode parseBoolean(ISource &source, const Delimiters &delimiters);
  static YNode parseAnchor(ISource &source, const Delimiters &delimiters);
  static YNode parseAlias(ISource &source, const Delimiters &delimiters);
  static YNode parseArray(ISource &source, const Delimiters &delimiters);
  static YNode parseInlineArray(ISource &source,
                                [[maybe_unused]] const Delimiters &delimiters);
  static DictionaryEntry parseKeyValue(ISource &source,
                                       const Delimiters &delimiters,
                                       bool inlineDictionary);
  static YNode parseDictionary(ISource &source, const Delimiters &delimiters);
  static YNode
  parseInlineDictionary(ISource &source,
                        [[maybe_unused]] const Delimiters &delimiters);
  static YNode parseDocument(ISource &source,
                             [[maybe_unused]] const Delimiters &delimiters);
  // YAML parser routing table
  using IsAFunc = std::function<bool(ISource &)>;
  using ParseFunc = std::function<YNode(ISource &, const Delimiters &)>;
  inline static std::vector<std::pair<IsAFunc, ParseFunc>> parsers{
      // Mappings
      {isArray, parseArray},
      {isDictionary, parseDictionary},
      {isInlineDictionary, parseInlineDictionary},
      {isInlineArray, parseInlineArray},
      // Scalars
      {isBoolean, parseBoolean},
      {isQuotedString, parseQuotedFlowString},
      {isNumber, parseNumber},
      {isNone, parseNone},
      {isFoldedBlockString, parseFoldedBlockString},
      {isPipedBlockString, parseLiteralBlockString},
      {isComment, parseComment},
      {isAnchor, parseAnchor},
      {isAlias, parseAlias},
      {isDefault, parsePlainFlowString}};
};

} // namespace YAML_Lib