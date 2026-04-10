
//
// Class: YAML_Parser_Tag
//
// Description: YAML tag parsing support.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Is a YAML tag (e.g. "!!str", "!!int", "!custom") on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a tag has been found.</returns>
bool Default_Parser::isTagged(const ISource &source) {
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
    throw SyntaxError(source.getPosition(), "Incomplete tag.");
  }

  if (source.current() == '<') {
    // Verbatim tag: !<tag:yaml.org,2002:str>
    isVerbatim = true;
    source.next();
    while (source.more() && source.current() != '>') {
      tagHandle += source.current();
      source.next();
    }
    if (!source.more() || source.current() != '>') {
      throw SyntaxError(source.getPosition(), "Unclosed verbatim tag '<'.");
    }
    source.next(); // consume '>'
  } else if (source.current() == '!') {
    // Secondary tag handle: !! -> primary handle "tag:yaml.org,2002:"
    source.next();
    tagHandle = "!!";
    while (source.more() && source.current() != kSpace &&
           source.current() != kLineFeed) {
      tagSuffix += source.current();
      source.next();
    }
  } else {
    // Could be primary !suffix or named handle !ns!suffix.
    // Scan ahead: if we find a second '!' before space/LF it is a named handle.
    std::string preExcl;
    while (source.more() && source.current() != '!' &&
           source.current() != kSpace && source.current() != kLineFeed) {
      preExcl += source.current();
      source.next();
    }
    if (source.more() && source.current() == '!') {
      source.next(); // consume second '!'
      tagHandle = "!" + preExcl + "!";
      while (source.more() && source.current() != kSpace &&
             source.current() != kLineFeed) {
        tagSuffix += source.current();
        source.next();
      }
    } else {
      // Primary tag handle: !suffix
      tagHandle = "!";
      tagSuffix = preExcl;
    }
  }

  source.ignoreWS();

  // Build full tag name
  std::string fullTag;
  if (isVerbatim) {
    fullTag = "!<" + tagHandle + ">";
  } else if (tagHandle == "!!") {
    // Expand using registered prefix or default yaml.org
    const std::string defaultPrefix{"tag:yaml.org,2002:"};
    auto it = yamlTagPrefixes.find("!!");
    fullTag =
        (it != yamlTagPrefixes.end() ? it->second : defaultPrefix) + tagSuffix;
  } else {
    // Named handle (e.g. !ns!suffix) or local tag (e.g. !suffix)
    auto it = yamlTagPrefixes.find(tagHandle);
    if (it != yamlTagPrefixes.end()) {
      fullTag = it->second + tagSuffix;
    } else if (tagHandle == "!") {
      fullTag = "!" + tagSuffix;
    } else {
      // Unknown named handle - keep verbatim
      fullTag = tagHandle + tagSuffix;
    }
  }

  // Standard YAML core schema tags cause type coercion
  // Helper: extract the raw scalar value (unquoting if quoted) as a string.
  auto extractRawScalar = [&]() -> std::string {
    if (isQuotedString(source)) {
      // extractString returns the content with surrounding quotes; strip them.
      std::string raw = extractString(source, source.current());
      if (raw.size() >= 2) {
        raw = raw.substr(1, raw.size() - 2);
      }
      rightTrim(raw);
      return raw;
    }
    return extractTrimmed(source, delimiters);
  };

  Node result;
  if (tagHandle == "!!" && !tagSuffix.empty()) {
    if (tagSuffix == "str") {
      // Force string interpretation — preserve quotes so the raw text is used.
      const std::string value = extractRawScalar();
      result = Node::make<String>(value, kNull);
    } else if (tagSuffix == "int" || tagSuffix == "float" ||
               tagSuffix == "bool" || tagSuffix == "null") {
      // Dispatch table for the four core type-coercion tags.
      using CoerceFunc = Node (*)(ISource &, const Delimiters &, unsigned long);
      static const std::unordered_map<std::string,
                                      std::pair<CoerceFunc, const char *>>
          coercions{{"int", {parseNumber, "!!int"}},
                    {"float", {parseNumber, "!!float"}},
                    {"bool", {parseBoolean, "!!bool"}},
                    {"null", {parseNone, "!!null"}}};
      const auto &[fn, tagName] = coercions.at(tagSuffix);
      if (isQuotedString(source)) {
        const std::string raw = extractRawScalar();
        BufferSource bs{raw + "\n"};
        result = fn(bs, {kLineFeed}, indentation);
      } else {
        result = fn(source, delimiters, indentation);
      }
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          std::string("Value cannot be parsed as ") + tagName +
                              ".");
      }
    } else if (tagSuffix == "seq") {
      result = parseDocument(source, delimiters, indentation);
    } else if (tagSuffix == "map") {
      result = parseDocument(source, delimiters, indentation);
    } else if (tagSuffix == "omap") {
      // !!omap — ordered map; Dictionary already preserves insertion order.
      // Parse as a normal mapping and attach the tag for semantic distinction.
      result = parseDocument(source, delimiters, indentation);
    } else if (tagSuffix == "pairs") {
      // !!pairs — sequence of key-value pairs; duplicate keys allowed.
      // Parse as a normal sequence and attach the tag.
      result = parseDocument(source, delimiters, indentation);
    } else if (tagSuffix == "timestamp") {
      // Try to parse as a native timestamp; fall back to string
      result = parseTimestamp(source, delimiters, indentation);
      if (result.isEmpty()) {
        std::string value{extractTrimmed(source, delimiters)};
        result = Node::make<String>(value, kNull);
      }
    } else if (tagSuffix == "binary") {
      // base64 value — keep raw string, tag carries the type signal
      std::string value{extractTrimmed(source, delimiters)};
      result = Node::make<String>(value, kNull);
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
    result.getVariant().setTag(fullTag);
  }

  return result;
}

} // namespace YAML_Lib
