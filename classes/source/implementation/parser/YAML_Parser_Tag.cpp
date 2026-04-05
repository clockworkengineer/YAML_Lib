
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
    std::string raw = extractToNext(source, delimiters);
    rightTrim(raw);
    return raw;
  };

  Node result;
  if (tagHandle == "!!" && !tagSuffix.empty()) {
    if (tagSuffix == "str") {
      // Force string interpretation — preserve quotes so the raw text is used.
      const std::string value = extractRawScalar();
      result = Node::make<String>(value, kNull);
    } else if (tagSuffix == "int") {
      // Force integer interpretation; support quoted string values.
      if (isQuotedString(source)) {
        const std::string raw = extractRawScalar();
        BufferSource bs{raw + "\n"};
        result = parseNumber(bs, {kLineFeed}, indentation);
      } else {
        result = parseNumber(source, delimiters, indentation);
      }
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          "Value cannot be parsed as !!int.");
      }
    } else if (tagSuffix == "float") {
      // Force float interpretation; support quoted string values.
      if (isQuotedString(source)) {
        const std::string raw = extractRawScalar();
        BufferSource bs{raw + "\n"};
        result = parseNumber(bs, {kLineFeed}, indentation);
      } else {
        result = parseNumber(source, delimiters, indentation);
      }
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          "Value cannot be parsed as !!float.");
      }
    } else if (tagSuffix == "bool") {
      // Force boolean interpretation; support quoted string values.
      if (isQuotedString(source)) {
        const std::string raw = extractRawScalar();
        BufferSource bs{raw + "\n"};
        result = parseBoolean(bs, {kLineFeed}, indentation);
      } else {
        result = parseBoolean(source, delimiters, indentation);
      }
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          "Value cannot be parsed as !!bool.");
      }
    } else if (tagSuffix == "null") {
      // Force null interpretation; support quoted string values.
      if (isQuotedString(source)) {
        const std::string raw = extractRawScalar();
        BufferSource bs{raw + "\n"};
        result = parseNone(bs, {kLineFeed}, indentation);
      } else {
        result = parseNone(source, delimiters, indentation);
      }
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          "Value cannot be parsed as !!null.");
      }
    } else if (tagSuffix == "seq") {
      result = parseDocument(source, delimiters, indentation);
    } else if (tagSuffix == "map") {
      result = parseDocument(source, delimiters, indentation);
    } else if (tagSuffix == "timestamp") {
      // Try to parse as a native timestamp; fall back to string
      result = parseTimestamp(source, delimiters, indentation);
      if (result.isEmpty()) {
        std::string value{extractToNext(source, delimiters)};
        rightTrim(value);
        result = Node::make<String>(value, kNull);
      }
    } else if (tagSuffix == "binary") {
      // base64 value — keep raw string, tag carries the type signal
      std::string value{extractToNext(source, delimiters)};
      rightTrim(value);
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
