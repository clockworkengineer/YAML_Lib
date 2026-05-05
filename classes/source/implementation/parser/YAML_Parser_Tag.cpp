
//
// Class: YAML_Parser_Tag
//
// Description: YAML tag parsing support.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"
#include <unordered_set>

namespace YAML_Lib {

/// <summary>
/// Is a YAML tag (e.g. "!!str", "!!int", "!custom") on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a tag has been found.</returns>
bool Default_Parser::isTagged(ISource &source) {
  return source.current() == '!';
}

/// <summary>
/// Parse a YAML tag and the value it annotates.
/// Standard tags (!!str, !!int, !!bool, !!null, !!float) perform type
/// coercion. Custom tags (!tag) are stored on the parsed node.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse value.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Tagged Node.</returns>
Node Default_Parser::parseTagged(ISource &source, const Delimiters &delimiters,
                                 const unsigned long indentation) {
  // Consume first '!'
  source.next();
  bool isVerbatim = false;
  std::string tagHandle;
  std::string tagSuffix;

  if (!source.more()) {
    YAML_THROW_POS(source, "Incomplete tag.");
  }

  if (source.current() == '<') {
    // Verbatim tag: !<tag:yaml.org,2002:str>
    isVerbatim = true;
    source.next();
    tagHandle = extractToNext(source, {'>'});
    if (!source.more() || source.current() != '>') {
      YAML_THROW_POS(source, "Unclosed verbatim tag '<'.");
    }
    source.next(); // consume '>'
  } else if (source.current() == '!') {
    // Secondary tag handle: !! -> primary handle "tag:yaml.org,2002:"
    source.next();
    tagHandle = "!!";
    // Flow-aware extraction: ',' ']' '}' are additional stops inside a flow
    // collection; in block context only space/LF apply. See extractTagSuffix.
    tagSuffix = extractTagSuffix(source);
  } else {
    // Could be primary !suffix or named handle !ns!suffix.
    // Scan ahead: if we find a second '!' before space/LF it is a named handle.
    std::string preExcl;
    preExcl = extractToNext(source, {'!', kSpace, kLineFeed});
    if (source.more() && source.current() == '!') {
      source.next(); // consume second '!'
      tagHandle = "!" + preExcl + "!";
      tagSuffix = extractTagSuffix(source);
    } else {
      // Primary tag handle: !suffix
      tagHandle = "!";
      tagSuffix = preExcl;
    }
  }

  const bool valueStartsOnNextLine = source.current() == kLineFeed;
  source.ignoreWS();

  const auto isEmptyScalar = [&]() {
    if (!source.more()) {
      return true;
    }
    if (source.current() == kColon || source.current() == kComma ||
        source.current() == kRightSquareBracket ||
        source.current() == kRightCurlyBrace || source.current() == '#') {
      return true;
    }
    if (source.current() == kLineFeed) {
      SourceGuard guard(source);
      moveToNextIndent(source);
      return !source.more() || source.getPosition().second <= indentation;
    }
    return false;
  }();

  // YAML 1.2 §6.8.1: ns-tag-char excludes c-flow-indicator characters
  // (comma, square brackets, curly braces).  In block context these chars are
  // not flow separators; if they appear inside the extracted suffix the tag is
  // malformed; reject by throwing.
  static constexpr std::string_view kInvalidTagChars{",[]{}"};
  if (!tagSuffix.empty() &&
      tagSuffix.find_first_of(kInvalidTagChars) != std::string::npos) {
    YAML_THROW_POS(source, "Invalid character in tag suffix '" + tagSuffix + "'.");
  }

  // Build full tag name
  std::string fullTag;
  if (isVerbatim) {
    fullTag = "!<" + tagHandle + ">";
  } else if (tagHandle == "!!") {
    // Expand using registered prefix or default yaml.org
    const std::string defaultPrefix{"tag:yaml.org,2002:"};
    auto it = ctx_.yamlTagPrefixes.find("!!");
    fullTag =
        (it != ctx_.yamlTagPrefixes.end() ? it->second : defaultPrefix) + tagSuffix;
  } else {
    // Named handle (e.g. !ns!suffix) or local tag (e.g. !suffix)
    auto it = ctx_.yamlTagPrefixes.find(tagHandle);
    if (it != ctx_.yamlTagPrefixes.end()) {
      fullTag = it->second + tagSuffix;
    } else if (tagHandle == "!") {
      fullTag = "!" + tagSuffix;
    } else {
      YAML_THROW_POS(source, "Undefined tag handle '" + tagHandle + "'.");
    }
  }

  // Standard YAML core schema tags cause type coercion
  // Helper: extract the raw scalar value (unquoting if quoted) as a string.
  auto extractRawScalar = [&]() -> std::string {
    if (isQuotedString(source)) {
      return extractRawQuotedScalar(source);
    }
    return extractTrimmed(source, delimiters);
  };

  Node result;
  static const std::string kCoreTagPrefix{"tag:yaml.org,2002:"};
  static const std::unordered_set<std::string> passthroughTags{"seq", "map",
                                                               "omap", "pairs"};
  const auto valueRequiresNodeParse = [&]() {
    return valueStartsOnNextLine ||
           (source.more() &&
            (source.current() == '&' || source.current() == '*' ||
             source.current() == '!'));
  };
  const bool isCoreSecondaryTag =
      fullTag.rfind(kCoreTagPrefix, 0) == 0 &&
      fullTag.size() == kCoreTagPrefix.size() + tagSuffix.size();
  if (isCoreSecondaryTag && !tagSuffix.empty()) {
    if (tagSuffix == "str") {
      std::string value;
      const bool needsNodeParse = valueRequiresNodeParse();
      if (isEmptyScalar) {
        value = "";
      } else if (!needsNodeParse) {
        // Same-line scalar: preserve the raw token so !!str 007 stays "007".
        value = extractRawScalar();
      } else {
        // Later-line scalar: parse the full node so multiline plain scalars
        // and anchor/tag-leading values retain normal YAML parsing semantics
        // before coercion to string.
        Node parsed = parseDocument(source, delimiters, indentation);
        value = parsed.toString();
        if (value.empty() && !isA<String>(parsed)) {
          value = parsed.toKey();
        }
      }
      result = Node::make<String>(value, kNull);
    } else if (tagSuffix == "int" || tagSuffix == "float" ||
               tagSuffix == "bool" || tagSuffix == "null") {
      // Dispatch table for the four core type-coercion tags.
      using CoerceFunc =
          std::function<Node(ISource &, const Delimiters &, unsigned long)>;
      const std::unordered_map<std::string, std::pair<CoerceFunc, const char *>>
          coercions{
              {"int",
               {[this](ISource &s, const Delimiters &d, unsigned long i) {
                  return parseNumber(s, d, i);
                },
                "!!int"}},
              {"float",
               {[this](ISource &s, const Delimiters &d, unsigned long i) {
                  return parseNumber(s, d, i);
                },
                "!!float"}},
              {"bool",
               {[this](ISource &s, const Delimiters &d, unsigned long i) {
                  return parseBoolean(s, d, i);
                },
                "!!bool"}},
              {"null",
               {[this](ISource &s, const Delimiters &d, unsigned long i) {
                  return parseNone(s, d, i);
                },
                "!!null"}}};
      const auto &[fn, tagName] = coercions.at(tagSuffix);
      const bool needsNodeParse = valueRequiresNodeParse();
      if (isEmptyScalar && tagSuffix == "null") {
        result = Node::make<Null>();
      } else if (!needsNodeParse && isQuotedString(source)) {
        const std::string raw = extractRawScalar();
        BufferSource bs{raw + "\n"};
        result = fn(bs, {kLineFeed}, indentation);
      } else if (!needsNodeParse) {
        result = fn(source, delimiters, indentation);
      } else {
        Node parsed = parseDocument(source, delimiters, indentation);
        if (isA<Number>(parsed) &&
            (tagSuffix == "int" || tagSuffix == "float")) {
          result = std::move(parsed);
        } else if (isA<Boolean>(parsed) && tagSuffix == "bool") {
          result = std::move(parsed);
        } else if (isA<Null>(parsed) && tagSuffix == "null") {
          result = std::move(parsed);
        } else {
          const std::string raw = parsed.toString();
          BufferSource bs{raw + "\n"};
          result = fn(bs, {kLineFeed}, indentation);
        }
      }
      if (result.isEmpty()) {
        YAML_THROW_POS(source, std::string("Value cannot be parsed as ") + tagName +
                              ".");
      }
    } else if (passthroughTags.count(tagSuffix)) {
      result = parseDocument(source, delimiters, indentation);
    } else if (tagSuffix == "timestamp") {
      // Try to parse as a native timestamp; fall back to string
      result = parseTimestamp(source, delimiters, indentation);
      if (result.isEmpty()) {
        std::string value{extractTrimmed(source, delimiters)};
        result = Node::make<String>(value, kNull);
      }
    } else if (tagSuffix == "binary") {
      // base64 value — parse normally (handles double-quoted and block scalars)
      result = parseDocument(source, delimiters, indentation);
    } else {
      // Unknown standard tag — parse value normally and attach tag
      result = parseDocument(source, delimiters, indentation);
    }
  } else {
    // Custom/local tag — parse value normally, attach tag
    result = parseDocument(source, delimiters, indentation);
  }

  // Apply tag to the result node's variant
  if (!result.isEmpty()) {
    result.setTag(fullTag);
  }

  return result;
}

} // namespace YAML_Lib
