#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class Default_Parser final : public IParser {

public:
  using Delimiters = std::set<char>;
  enum class BlockChomping : uint8_t { clip = 0, strip, keep };
  explicit Default_Parser(std::unique_ptr<ITranslator> translator) {
    yamlTranslator = std::move(translator);
  }
  Default_Parser(const Default_Parser &other) = delete;
  Default_Parser &operator=(const Default_Parser &other) = delete;
  Default_Parser(Default_Parser &&other) = delete;
  Default_Parser &operator=(Default_Parser &&other) = delete;
  ~Default_Parser() override = default;

  std::vector<Node> parse(ISource &source) override;

  // Alias Map
  inline static std::map<std::string, std::string> yamlAliasMap{};
  // Enable/disable strict YAML 1.2 boolean mode (only 'true'/'false' valid)
  static void setStrictBooleans(const bool strict) { strictBooleans = strict; }

private:
  // RAII save/restore guard for ISource lookahead.
  // Calls source.restore() in the destructor unless release() is called first.
  class SourceGuard {
  public:
    explicit SourceGuard(ISource &src) : src_(&src) { src_->save(); }
    ~SourceGuard() {
      if (src_) {
        src_->restore();
      }
    }
    // Disarm the guard: the caller takes responsibility for the stream pos.
    void release() { src_ = nullptr; }
    SourceGuard(const SourceGuard &) = delete;
    SourceGuard &operator=(const SourceGuard &) = delete;
    SourceGuard(SourceGuard &&) = delete;
    SourceGuard &operator=(SourceGuard &&) = delete;

  private:
    ISource *src_;
  };

  // RAII guard that increments a depth counter on construction and
  // decrements it on destruction (including on exception).
  class DepthGuard {
  public:
    explicit DepthGuard(long &depth) : depth_(depth) { ++depth_; }
    ~DepthGuard() { --depth_; }
    DepthGuard(const DepthGuard &) = delete;
    DepthGuard &operator=(const DepthGuard &) = delete;
    DepthGuard(DepthGuard &&) = delete;
    DepthGuard &operator=(DepthGuard &&) = delete;

  private:
    long &depth_;
  };

  // Scaffold shared by simple scalar parsers (parseNone, parseBoolean,
  // parseNumber). Saves the source position, extracts the next token up to
  // the given delimiters, right-trims it, then calls pred(token).  If pred
  // returns a non-empty Node the guard is released (position stays consumed);
  // otherwise the guard restores the source to where it was.
  template <typename Predicate>
  static Node tryParseToken(ISource &source, const Delimiters &delimiters,
                            Predicate &&pred) {
    SourceGuard guard(source);
    std::string token{extractToNext(source, delimiters)};
    rightTrim(token);
    Node result = std::forward<Predicate>(pred)(token);
    if (!result.isEmpty()) {
      guard.release();
    }
    return result;
  }

  // YAML parser
  static bool endsWith(const std::string_view &str,
                       const std::string_view &substr);
  static void validateInputCharacters(ISource &source);
  static void rightTrim(std::string &str);
  static void moveToNext(ISource &source, const Delimiters &delimiters);
  static void skipLine(ISource &source);
  static void moveToNextIndent(ISource &source);
  static void addUniqueDictEntry(Node &dictionaryNode, DictionaryEntry entry,
                                 ISource &source);
  static void addInlineDictEntry(Dictionary &dict, DictionaryEntry entry,
                                 ISource &source);
  static const std::string &resolveAlias(const std::string &name,
                                         ISource &source);
  static bool isNullStringNode(const Node &node);
  static bool looksLikeIso8601Date(const std::string &s);
  static std::string extractString(ISource &source, char quote);
  static std::string extractString(ISource &source);
  static std::string extractToNext(ISource &source,
                                   const Delimiters &delimiters);
  static std::string extractTrimmed(ISource &source,
                                    const Delimiters &delimiters);
  static std::string extractInLine(ISource &source, char start, char end);
  static std::string extractInlineCollectionAt(ISource &source);
  static std::string extractMapping(ISource &source);
  static void checkForEnd(ISource &source, char end);
  static void checkFlowDelimiter(ISource &source, const Delimiters &delimiters);
  static Node parseFromBuffer(const std::string &text,
                              const Delimiters &delimiters,
                              unsigned long indentation);
  static std::string captureIndentedBlock(ISource &source,
                                          unsigned long minIndent);
  static void upsertDictEntry(Dictionary &dict, const std::string &key,
                              Node value);
  static Node mergeOverrides(Node &overrideRoot);
  static Node convertYAMLToStringNode(const std::string_view &yamlString);
  static bool isValidKey(const std::string_view &key);
  static bool isOverride(ISource &source);
  static bool isKey(ISource &source);
  static bool isArray(ISource &source);
  static bool isBoolean(const ISource &source);
  static bool isQuotedString(const ISource &source);
  static bool isNumber(const ISource &source);
  static bool isNone(const ISource &source);
  static bool isFoldedBlockString(const ISource &source);
  static bool isPipedBlockString(const ISource &source);
  static bool isComment(const ISource &source);
  static bool isAnchor(const ISource &source);
  static bool isAlias(const ISource &source);
  static bool isInlineArray(const ISource &source);
  static bool isInlineDictionary(const ISource &source);
  static bool isInlineCollection(const ISource &source);
  static bool isMapping(ISource &source);
  static bool isDictionary(ISource &source);
  static bool isDefault(ISource &source);
  static bool matchesMarker(ISource &source, const char *marker);
  static bool isDocumentStart(ISource &source);
  static bool isDocumentEnd(ISource &source);
  static bool isDocumentBoundary(ISource &source);
  static bool isInlineComment(const ISource &source,
                              const std::string &yamlString);
  static void convertOctalToDecimal(std::string &numeric,
                                    const std::string &digits);
  static bool isDirective(ISource &source);
  static bool isTagged(const ISource &source);
  static bool isTimestamp(ISource &source);
  static void appendCharacterToString(ISource &source, std::string &yamlString);
  static std::string extractKey(ISource &source);
  static std::pair<BlockChomping, int> parseBlockChomping(ISource &source);
  static std::string parseBlockString(ISource &source,
                                      const Delimiters &delimiters,
                                      unsigned long indentation,
                                      char fillerDefault);
  static Node parseKey(ISource &source);
  static Node parseFoldedBlockString(ISource &source,
                                     const Delimiters &delimiters,
                                     unsigned long indentation);
  static Node parseLiteralBlockString(ISource &source,
                                      const Delimiters &delimiters,
                                      unsigned long indentation);
  static Node parsePlainFlowString(ISource &source,
                                   const Delimiters &delimiters,
                                   unsigned long indentation);
  static Node parseQuotedFlowString(ISource &source,
                                    const Delimiters &delimiters,
                                    unsigned long indentation);
  static Node parseComment(ISource &source,
                           [[maybe_unused]] const Delimiters &delimiters);
  static Node parseNumber(ISource &source, const Delimiters &delimiters,
                          unsigned long indentation);
  static Node parseNone(ISource &source, const Delimiters &delimiters,
                        unsigned long indentation);
  static Node parseBoolean(ISource &source, const Delimiters &delimiters,
                           unsigned long indentation);
  static Node parseTimestamp(ISource &source, const Delimiters &delimiters,
                             unsigned long indentation);
  static Node parseAnchor(ISource &source, const Delimiters &delimiters,
                          unsigned long indentation);
  static Node parseAlias(ISource &source, const Delimiters &delimiters,
                         unsigned long indentation);
  static Node parseOverride(ISource &source, const Delimiters &delimiters,
                            unsigned long indentation);
  static Node parseArray(ISource &source, const Delimiters &delimiters,
                         unsigned long indentation);
  static Node parseInlineArray(ISource &source,
                               [[maybe_unused]] const Delimiters &delimiters,
                               unsigned long indentation);
  static DictionaryEntry parseKeyValue(ISource &source,
                                       const Delimiters &delimiters,
                                       unsigned long indentation);
  static DictionaryEntry parseInlineKeyValue(ISource &source,
                                             const Delimiters &delimiters,
                                             unsigned long indentation);
  static Node parseDictionary(ISource &source, const Delimiters &delimiters,
                              unsigned long indentation);
  static Node
  parseInlineDictionary(ISource &source,
                        [[maybe_unused]] const Delimiters &delimiters,
                        unsigned long indentation);
  static Node parseDocument(ISource &source,
                            [[maybe_unused]] const Delimiters &delimiters,
                            unsigned long indentation);
  static void parseDirective(ISource &source, bool inDocument);
  static unsigned long scanToFirstBlockContent(ISource &source);
  static Delimiters withExtras(const Delimiters &base,
                               std::initializer_list<char> extras);
  static Delimiters keyStopDelimiters();
  [[nodiscard]] static bool isInsideFlowContext() noexcept;
  static Node parseTagged(ISource &source, const Delimiters &delimiters,
                          unsigned long indentation);
  // YAML parser routing table
  using IsAFunc = std::function<bool(ISource &)>;
  using ParseFunc =
      std::function<Node(ISource &, const Delimiters &, unsigned long)>;
  inline static std::vector<std::pair<IsAFunc, ParseFunc>> parsers{
      // Mappings
      {isArray, parseArray},
      {isDictionary, parseDictionary},
      {isInlineDictionary, parseInlineDictionary},
      {isInlineArray, parseInlineArray},
      // Scalars
      {isBoolean, parseBoolean},
      {isQuotedString, parseQuotedFlowString},
      {isTimestamp, parseTimestamp},
      {isNumber, parseNumber},
      {isNone, parseNone},
      {isFoldedBlockString, parseFoldedBlockString},
      {isPipedBlockString, parseLiteralBlockString},
      {isAnchor, parseAnchor},
      {isAlias, parseAlias},
      {isOverride, parseOverride},
      {isTagged, parseTagged},
      {isDefault, parsePlainFlowString}};
  // Array Indent level
  inline static long arrayIndentLevel{0};
  // Inline Array depth
  inline static long inlineArrayDepth{0};
  // Inline Dictionary depth
  inline static long inlineDictionaryDepth{0};
  // YAML directive version (minor)
  inline static int yamlDirectiveMinor{2};
  // Strict YAML 1.2 boolean mode — only 'true'/'false' accepted (default: false
  // for backward compat)
  inline static bool strictBooleans{false};
  // TAG directive prefix map: handle -> prefix URI
  inline static std::map<std::string, std::string> yamlTagPrefixes{};
  // Set of anchor names currently being expanded (cycle detection)
  inline static std::set<std::string> activeAliasExpansions{};
  // Track whether a %YAML directive has been seen for the current document
  inline static bool yamlDirectiveSeen{false};
  // Translator
  inline static std::unique_ptr<ITranslator> yamlTranslator;
};

} // namespace YAML_Lib