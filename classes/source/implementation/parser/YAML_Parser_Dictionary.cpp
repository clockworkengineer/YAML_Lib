
//
// Class: YAML_Parser_Dictionary
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Add a DictionaryEntry to a dictionary Node, throwing if the key already
/// exists.
/// </summary>
/// <param name="dictionaryNode">Target dictionary Node.</param>
/// <param name="entry">Entry to add.</param>
/// <param name="source">Source stream (used for error position).</param>
void Default_Parser::addUniqueDictEntry(Node &dictionaryNode,
                                        DictionaryEntry entry,
                                        ISource &source) {
  if (NRef<Dictionary>(dictionaryNode).contains(entry.getKey())) {
    throw SyntaxError(source.getPosition(),
                      "Dictionary already contains key '" +
                          std::string(entry.getKey()) + "'.");
  }
  NRef<Dictionary>(dictionaryNode).add(std::move(entry));
}
/// <summary>
/// Convert YAML key to a string Node
/// </summary>
/// <param name="yamlString">YAML string.</param>
Node Default_Parser::convertYAMLToStringNode(
    const std::string_view &yamlString) {
  BufferSource yamlKey{std::string(yamlString) + kLineFeed};
  auto keyNode = parseDocument(yamlKey, {kLineFeed}, 0);
  std::string keyString{NRef<Variant>(keyNode).toKey()};
  char quote = '\"';
  if (isA<String>(keyNode)) {
    quote = NRef<String>(keyNode).getQuote();
    if (keyString.empty()) {
      quote = kDoubleQuote;
    }
  }
  return Node::make<String>(keyString, quote);
}
/// <summary>
/// Does YAML that is  passed in constitute a valid dictionary key?
/// </summary>
/// <param name="key">YAML sequence to be converted to be used as the
/// key.</param> <returns> If true value is a valid key.</returns>
bool Default_Parser::isValidKey(const std::string_view &key) {
  try {
    BufferSource yamlKey{std::string(key) + kLineFeed};
    const Node keyNode = parseDocument(yamlKey, {kLineFeed}, 0);
    return !keyNode.isEmpty() && !isA<Comment>(keyNode);
  } catch ([[maybe_unused]] const std::exception &e) {
    return false;
  }
}
/// <summary>
/// Extract mapping on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Extracted mapping/.</returns>
std::string Default_Parser::extractMapping(ISource &source) {
  std::string key;
  source.next();
  while (source.more() && source.current() == kSpace) {
    key += kSpace;
    source.next();
  }
  if (isComment(source)) {
    moveToNext(source, {kLineFeed});
    return extractMapping(source);
  }
  if (isInlineDictionary(source)) {
    key += extractInLine(source, kLeftCurlyBrace, kRightCurlyBrace);
    moveToNext(source, {kColon});
  } else if (isInlineArray(source)) {
    key += extractInLine(source, kLeftSquareBracket, kRightSquareBracket);
    moveToNext(source, {kColon});
  } else if (isArray(source)) {
    key += extractToNext(source, {kColon});
  } else {
    key += extractToNext(source, {kLineFeed});
    key += kColon;
  }
  return key;
}
/// <summary>
/// Extract YAML used in possible key value from source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>YAML for key value.</returns>
std::string Default_Parser::extractKey(ISource &source) {
  if (isInlineDictionary(source)) {
    return extractInLine(source, kLeftCurlyBrace, kRightCurlyBrace);
  }
  if (isInlineArray(source)) {
    return extractInLine(source, kLeftSquareBracket, kRightSquareBracket);
  }
  if (isQuotedString(source)) {
    return extractString(source, source.current());
  }
  if (isMapping(source)) {
    return extractMapping(source);
  }
  // Plain scalar key: ':' is only a value separator when followed by
  // space/newline/EOF. Otherwise (e.g. ":foo") the colon is part of the key
  // text.
  // In block context (inlineDictionaryDepth == 0) '}' and ',' are ordinary
  // characters and must not terminate key extraction.  They are only special
  // inside flow collections (inlineDictionaryDepth > 0).
  const Delimiters plainKeyDelimiters =
      inlineDictionaryDepth > 0
          ? Delimiters{kColon, kComma, kRightCurlyBrace, kLineFeed}
          : Delimiters{kColon, kLineFeed};
  std::string key;
  while (source.more()) {
    key += extractToNext(source, plainKeyDelimiters);
    if (!source.more() || source.current() != kColon)
      break;
    const bool isSeparator = [&source]() -> bool {
      SourceGuard guard(source);
      source.next(); // peek past ':'
      return !source.more() || source.current() == kSpace ||
             source.current() == kLineFeed;
    }();
    if (isSeparator)
      break; // ':' is the key-value separator, stop here
    key += kColon;
    source.next(); // consume ':', it is part of the key
  }
  return key;
}
/// <summary>
/// Parse dictionary key on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Dictionary entry key.</returns>
Node Default_Parser::parseKey(ISource &source) {
  std::string key{extractKey(source)};
  if (!key.empty() && key.back() == kColon) {
    key.pop_back();
  }
  if (source.more() && source.current() != kRightCurlyBrace &&
      source.current() != kComma) {
    source.next();
  }
  rightTrim(key);
  if (!isValidKey(key)) {
    throw SyntaxError(source.getPosition(),
                      "Invalid key '" + key + "' specified.");
  }
  return convertYAMLToStringNode(key);
}
/// <summary>
/// Parse dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry Default_Parser::parseKeyValue(ISource &source,
                                              const Delimiters &delimiters,
                                              const unsigned long indentation) {
  const unsigned long keyIndent = source.getPosition().second;
  Node keyNode = parseKey(source);
  source.ignoreWS();
  // Explicit-key value separator: "? key\n: value" form — after parsing a '?'
  // key, source lands on the ': value' line.  Consume ':' (and optional space)
  // BEFORE calling isKey(), because isKey() treats ':' as a valid key start.
  if (source.more() && source.current() == kColon) {
    source.next(); // consume ':'
    if (source.more() && source.current() == kSpace) {
      source.next(); // consume optional space after ':'
    }
  } else if (isKey(source) && !isMapping(source) &&
             (inlineDictionaryDepth > 0 ||
              source.getPosition().second > keyIndent)) {
    // Only throw when inside a flow collection (inlineDictionaryDepth > 0)
    // or when an unexpected key appears deeper than keyIndent, indicating a
    // compact-nested / same-line key where a simple value was expected.
    // At block level with the next key at the same column, the explicit key
    // simply has a null value and the sibling entry follows immediately
    // (e.g. "? b\n&anchor c: 3" — ZWK4).
    throw SyntaxError(source.getPosition(),
                      "Only an inline/compact dictionary is allowed.");
  }
  moveToNextIndent(source);
  Node dictionaryNode = Node::make<Null>();
  if (source.more() && (source.getPosition().second > keyIndent ||
                        isInlineArray(source) || isInlineDictionary(source))) {
    dictionaryNode = parseDocument(source, delimiters, indentation);
  }
  return {keyNode, dictionaryNode};
}
/// <summary>
/// Parse inline dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry
Default_Parser::parseInlineKeyValue(ISource &source,
                                    const Delimiters &delimiters,
                                    const unsigned long indentation) {
  Node keyNode = parseKey(source);
  Node dictionaryNode = Node::make<Null>();
  if (source.current() != kComma) {
    dictionaryNode = parseDocument(source, delimiters, indentation);
  }
  if (isNullStringNode(dictionaryNode)) {
    dictionaryNode = Node::make<Null>();
  }
  return {keyNode, dictionaryNode};
}
/// <summary>
/// Parse a dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse dictionary.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary Node.</returns>
Node Default_Parser::parseDictionary(
    ISource &source, const Delimiters &delimiters,
    [[maybe_unused]] unsigned long indentation) {
  const unsigned long dictionaryIndent = source.getPosition().second;
  Node dictionaryNode = Node::make<Dictionary>();
  while (source.more() && dictionaryIndent == source.getPosition().second) {
    if (isKey(source)) {
      auto entry = parseKeyValue(source, delimiters, dictionaryIndent);
      addUniqueDictEntry(dictionaryNode, std::move(entry), source);
    } else if (isDocumentStart(source) || isDocumentEnd(source)) {
      break;
    } else {
      if (dictionaryIndent == source.getPosition().second) {
        throw SyntaxError(source.getPosition(),
                          "Missing key/value pair from indentation level.");
      }
    }
    moveToNextIndent(source);
  }
  if (isKey(source) && dictionaryIndent < source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Mapping key has the incorrect indentation.");
  }
  return mergeOverrides(dictionaryNode);
}
/// <summary>
/// Parse inline dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse inline dictionary.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary Node.</returns>
Node Default_Parser::parseInlineDictionary(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters,
    const unsigned long indentation) {
  Delimiters inLineDictionaryDelimiters = {delimiters};
  inLineDictionaryDelimiters.insert({kComma, kRightCurlyBrace});
  Node dictionaryNode = Node::make<Dictionary>();
  inlineDictionaryDepth++;
  do {
    source.next();
    moveToNextIndent(source);
    if (source.current() == kComma) {
      throw SyntaxError("Unexpected ',' in in-line dictionary.");
    }
    if (source.current() != kRightCurlyBrace) {
      auto entry =
          parseInlineKeyValue(source, inLineDictionaryDelimiters, indentation);
      addUniqueDictEntry(dictionaryNode, std::move(entry), source);
    }
  } while (source.current() == kComma);
  inlineDictionaryDepth--;
  checkForEnd(source, kRightCurlyBrace);
  if (source.current() == kColon) {
    throw SyntaxError(
        source.getPosition(),
        "Inline dictionary used as key is meant to be on one line.");
  }
  source.ignoreWS();

  if (source.more() && inlineDictionaryDepth == 0) {
    if (!delimiters.contains(source.current())) {
      throw SyntaxError("Unexpected flow sequence token '" +
                        std::string(1, source.current()) + "'.");
    }
  }
  return dictionaryNode;
}

} // namespace YAML_Lib