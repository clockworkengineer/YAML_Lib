
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
    // Primary tag handle: ! -> local tag
    tagHandle = "!";
    while (source.more() && source.current() != kSpace &&
           source.current() != kLineFeed) {
      tagSuffix += source.current();
      source.next();
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
    // !handle or local tag
    auto it = yamlTagPrefixes.find("!");
    if (it != yamlTagPrefixes.end() && !tagSuffix.empty()) {
      fullTag = it->second + tagSuffix;
    } else {
      fullTag = "!" + tagSuffix;
    }
  }

  // Standard YAML core schema tags cause type coercion
  Node result;
  if (tagHandle == "!!" && !tagSuffix.empty()) {
    if (tagSuffix == "str") {
      // Force string interpretation
      std::string value{extractToNext(source, delimiters)};
      rightTrim(value);
      result = Node::make<String>(value, kNull);
    } else if (tagSuffix == "int") {
      // Force integer interpretation
      result = parseNumber(source, delimiters, indentation);
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          "Value cannot be parsed as !!int.");
      }
    } else if (tagSuffix == "float") {
      // Force float interpretation
      result = parseNumber(source, delimiters, indentation);
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          "Value cannot be parsed as !!float.");
      }
    } else if (tagSuffix == "bool") {
      result = parseBoolean(source, delimiters, indentation);
      if (result.isEmpty()) {
        throw SyntaxError(source.getPosition(),
                          "Value cannot be parsed as !!bool.");
      }
    } else if (tagSuffix == "null") {
      result = parseNone(source, delimiters, indentation);
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
