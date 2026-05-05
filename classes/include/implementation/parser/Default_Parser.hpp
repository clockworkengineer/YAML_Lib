#pragma once

#include <bitset>

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

// -----------------------------------------------------------------------
// E2: Per-parse mutable state — one ParseContext per Default_Parser
// instance.  Moved out of inline static members so that multiple
// Default_Parser objects (one per YAML instance) are fully independent and
// concurrent parses do not corrupt each other's state.
// -----------------------------------------------------------------------
struct ParseContext {
  std::unordered_map<std::string, std::string> yamlAliasMap;
  std::set<std::string>                        activeAliasExpansions;
  long          arrayIndentLevel{0};
  long          inlineArrayDepth{0};
  long          inlineDictionaryDepth{0};
  unsigned long blockFlowValueIndent{0};
  int           yamlDirectiveMinor{2};
  bool          yamlDirectiveSeen{false};
  std::map<std::string, std::string> yamlTagPrefixes;
};

class Default_Parser final : public IParser {

public:
  class Delimiters {
  public:
    Delimiters() = default;
    Delimiters(std::initializer_list<char> chars) { insert(chars); }
    Delimiters(const Delimiters &other) = default;
    Delimiters &operator=(const Delimiters &other) = default;

    bool empty() const noexcept { return none(); }
    bool contains(const char ch) const noexcept {
      return bitmask_.test(static_cast<unsigned char>(ch));
    }
    void insert(std::initializer_list<char> chars) {
      for (const char ch : chars) {
        bitmask_.set(static_cast<unsigned char>(ch));
      }
    }

  private:
    bool none() const noexcept { return bitmask_.none(); }

    std::bitset<256> bitmask_;
  };
  enum class BlockChomping : uint8_t { clip = 0, strip, keep };
  explicit Default_Parser(std::unique_ptr<ITranslator> translator)
      : yamlTranslator_(std::move(translator)) {
    // Build routing table with lambdas that capture `this`, so each
    // Default_Parser instance dispatches into its own non-static methods and
    // its own ParseContext — enabling safe concurrent use across instances.
    parsers_ = {{
        {[this](ISource &s) { return isArray(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseArray(s, d, i); }},
        {[this](ISource &s) { return isDictionary(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseDictionary(s, d, i); }},
        {[this](ISource &s) { return isInlineDictionary(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseInlineDictionary(s, d, i); }},
        {[this](ISource &s) { return isInlineArray(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseInlineArray(s, d, i); }},
        {[this](ISource &s) { return isBoolean(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseBoolean(s, d, i); }},
        {[this](ISource &s) { return isQuotedString(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseQuotedFlowString(s, d, i); }},
        {[this](ISource &s) { return isTimestamp(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseTimestamp(s, d, i); }},
        {[this](ISource &s) { return isNumber(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseNumber(s, d, i); }},
        {[this](ISource &s) { return isNone(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseNone(s, d, i); }},
        {[this](ISource &s) { return isFoldedBlockString(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseFoldedBlockString(s, d, i); }},
        {[this](ISource &s) { return isPipedBlockString(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseLiteralBlockString(s, d, i); }},
        {[this](ISource &s) { return isAnchor(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseAnchor(s, d, i); }},
        {[this](ISource &s) { return isAlias(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseAlias(s, d, i); }},
        {[this](ISource &s) { return isOverride(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseOverride(s, d, i); }},
        {[this](ISource &s) { return isTagged(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parseTagged(s, d, i); }},
        {[this](ISource &s) { return isDefault(s); },
         [this](ISource &s, const Delimiters &d, unsigned long i) { return parsePlainFlowString(s, d, i); }},
    }};
  }
  Default_Parser(const Default_Parser &other) = delete;
  Default_Parser &operator=(const Default_Parser &other) = delete;
  Default_Parser(Default_Parser &&other) = delete;
  Default_Parser &operator=(Default_Parser &&other) = delete;
  ~Default_Parser() override = default;

  std::vector<Node> parse(ISource &source) override;

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
    void release() {
      if (src_) {
        src_->discardSave();
      }
      src_ = nullptr;
    }
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
  Node tryParseToken(ISource &source, const Delimiters &delimiters,
                            unsigned long indentation, Predicate &&pred) {
    const unsigned long tokenIndent = source.getPosition().second;
    SourceGuard guard(source);
    std::string token{extractToNext(source, delimiters)};
    rightTrim(token);
    if (source.more() && source.current() == kLineFeed &&
        hasPlainScalarContinuation(source, tokenIndent)) {
      return {};
    }
    Node result = std::forward<Predicate>(pred)(token);
    if (!result.isEmpty()) {
      guard.release();
    }
    return result;
  }

  bool hasPlainScalarContinuation(ISource &source,
                                         unsigned long indentation);

  // YAML parser
  bool endsWith(const std::string_view &str,
                       const std::string_view &substr);
  void rightTrim(std::string &str);
  void moveToNext(ISource &source, const Delimiters &delimiters);
  void skipLine(ISource &source);
  bool skipIfComment(ISource &source);
  void moveToNextIndent(ISource &source);
  void addUniqueDictEntry(Node &dictionaryNode, DictionaryEntry entry,
                                 ISource &source);
  void addInlineDictEntry(Dictionary &dict, DictionaryEntry entry,
                                 ISource &source);
  const std::string &resolveAlias(const std::string &name,
                                         ISource &source);
  bool isNullStringNode(const Node &node);
  bool looksLikeIso8601Date(const std::string &s);
  std::string extractString(ISource &source, char quote);
  std::string extractString(ISource &source, char quote,
                                   unsigned long *quoteColumn);
  std::string extractString(ISource &source);
  std::string extractRawQuotedScalar(ISource &source);
  std::string extractTagSuffix(ISource &source);
  std::string extractToNext(ISource &source,
                                   const Delimiters &delimiters);
  std::string extractTrimmed(ISource &source,
                                    const Delimiters &delimiters);
  std::string extractInLine(ISource &source, char start, char end);
  std::string extractInlineCollectionAt(ISource &source);
  std::string extractMapping(ISource &source);
  void checkForEnd(ISource &source, char end);
  void checkFlowDelimiter(ISource &source, const Delimiters &delimiters);
  void checkAtFlowClose(ISource &source, const Delimiters &delimiters,
                               long depth);
  Node parseFromBuffer(const std::string &text,
                              const Delimiters &delimiters,
                              unsigned long indentation);
  std::string captureIndentedBlock(ISource &source,
                                          unsigned long minIndent);
  void upsertDictEntry(Dictionary &dict, const std::string &key,
                              Node value);
  Node mergeOverrides(Node &overrideRoot);
  Node convertYAMLToStringNode(const std::string_view &yamlString);
  Node convertYAMLToStringNode(const std::string_view &yamlString,
                                      unsigned long indentation);
  bool isValidKey(const std::string_view &key) noexcept;
  bool isOverride(ISource &source);
  bool isKey(ISource &source);
  bool isArray(ISource &source);
  bool isBoolean(const ISource &source);
  bool isQuotedString(const ISource &source);
  bool isNumber(const ISource &source);
  bool isNone(const ISource &source);
  bool isFoldedBlockString(const ISource &source);
  bool isPipedBlockString(const ISource &source);
  bool isComment(const ISource &source);
  bool isAnchor(const ISource &source);
  bool isAlias(const ISource &source);
  bool isInlineArray(const ISource &source);
  bool isInlineDictionary(const ISource &source);
  bool isInlineCollection(const ISource &source);
  bool isMapping(ISource &source);
  bool isDictionary(ISource &source);
  bool isDefault(ISource &source);
  bool matchesMarker(ISource &source, const char *marker);
  bool isDocumentStart(ISource &source);
  bool isDocumentEnd(ISource &source);
  bool isDocumentBoundary(ISource &source);
  bool isInlineComment(const ISource &source,
                              const std::string &yamlString);
  void convertOctalToDecimal(std::string &numeric,
                                    const std::string &digits);
  bool isDirective(ISource &source);
  bool isTagged(const ISource &source);
  bool isTimestamp(ISource &source);
  void appendCharacterToString(ISource &source, std::string &yamlString,
                                      bool escapeAware = false,
                                      unsigned long minIndent = 0);
  std::string extractKey(ISource &source,
                                unsigned long *quoteIndent = nullptr);
  std::pair<BlockChomping, int> parseBlockChomping(ISource &source);
  std::string parseBlockString(ISource &source,
                                      const Delimiters &delimiters,
                                      unsigned long indentation,
                                      char fillerDefault);
  Node parseKey(ISource &source);
  Node parseFoldedBlockString(ISource &source,
                                     const Delimiters &delimiters,
                                     unsigned long indentation);
  Node parseLiteralBlockString(ISource &source,
                                      const Delimiters &delimiters,
                                      unsigned long indentation);
  Node parsePlainFlowString(ISource &source,
                                   const Delimiters &delimiters,
                                   unsigned long indentation);
  Node parseQuotedFlowString(ISource &source,
                                    const Delimiters &delimiters,
                                    unsigned long indentation);
  Node parseComment(ISource &source,
                           [[maybe_unused]] const Delimiters &delimiters);
  Node parseNumber(ISource &source, const Delimiters &delimiters,
                          unsigned long indentation);
  Node parseNone(ISource &source, const Delimiters &delimiters,
                        unsigned long indentation);
  Node parseBoolean(ISource &source, const Delimiters &delimiters,
                           unsigned long indentation);
  Node parseTimestamp(ISource &source, const Delimiters &delimiters,
                             unsigned long indentation);
  Node parseAnchor(ISource &source, const Delimiters &delimiters,
                          unsigned long indentation);
  Node parseAlias(ISource &source, const Delimiters &delimiters,
                         unsigned long indentation);
  Node parseOverride(ISource &source, const Delimiters &delimiters,
                            unsigned long indentation);
  Node parseArray(ISource &source, const Delimiters &delimiters,
                         unsigned long indentation);
  Node parseInlineArray(ISource &source,
                               [[maybe_unused]] const Delimiters &delimiters,
                               unsigned long indentation);
  DictionaryEntry parseKeyValue(ISource &source,
                                       const Delimiters &delimiters,
                                       unsigned long indentation);
  DictionaryEntry parseInlineKeyValue(ISource &source,
                                             const Delimiters &delimiters,
                                             unsigned long indentation);
  Node parseDictionary(ISource &source, const Delimiters &delimiters,
                              unsigned long indentation);
  Node
  parseInlineDictionary(ISource &source,
                        [[maybe_unused]] const Delimiters &delimiters,
                        unsigned long indentation);
  Node parseDocument(ISource &source,
                            [[maybe_unused]] const Delimiters &delimiters,
                            unsigned long indentation);
  void parseDirective(ISource &source, bool inDocument);
  unsigned long scanToFirstBlockContent(ISource &source);
  Delimiters withExtras(const Delimiters &base,
                               std::initializer_list<char> extras);
  Delimiters keyStopDelimiters();
  [[nodiscard]] bool isInsideFlowContext() noexcept;
  Node parseTagged(ISource &source, const Delimiters &delimiters,
                          unsigned long indentation);
  // YAML parser routing table — per-instance (not static) so that each
  // Default_Parser owns its own routing table with lambdas bound to `this`.
  using IsAFunc = std::function<bool(ISource &)>;
  using ParseFunc =
      std::function<Node(ISource &, const Delimiters &, unsigned long)>;
  using ParserEntry = std::pair<IsAFunc, ParseFunc>;
  std::array<ParserEntry, 16> parsers_{};
  // Per-parse mutable state (replaces the former inline static members).
  ParseContext ctx_;
  // Translator (per-instance, not shared across Default_Parser instances).
  std::unique_ptr<ITranslator> yamlTranslator_;
  // Strict YAML 1.2 boolean mode — process-global setting (not per-parse).
  inline static bool strictBooleans{false};
};

} // namespace YAML_Lib