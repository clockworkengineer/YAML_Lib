
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
/// Convert YAML key to a string YNode
/// </summary>
/// <param name="yamlString">YAML string.</param>
YNode Default_Parser::convertYAMLToStringYNode(const std::string &yamlString) {
  BufferSource yamlKey{yamlString + kLineFeed};
  auto keyYNode = parseDocument(yamlKey, {kLineFeed}, 0);
  std::string keyString{YRef<Variant>(keyYNode).toKey()};
  char quote = '\"';
  if (isA<String>(keyYNode)) {
    quote = YRef<String>(keyYNode).getQuote();
    if (keyString.empty()) {
      quote = kDoubleQuote;
    }
  }
  return YNode::make<String>(keyString, quote);
}
/// <summary>
/// Does YAML that is  passed in constitute a valid dictionary key?
/// </summary>
/// <param name="key">YAML sequence to be converted to be used as the key.</param>
/// <returns> If true value is a valid key.</returns>
bool Default_Parser::isValidKey(const std::string &key) {
  try {
    BufferSource yamlKey{key + kLineFeed};
    const YNode keyYNode = parseDocument(yamlKey, {kLineFeed}, 0);
    return !keyYNode.isEmpty() && !isA<Comment>(keyYNode);
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
  return extractToNext(source, {kColon, kComma, kRightCurlyBrace, kLineFeed});
}
/// <summary>
/// Parse dictionary key on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Dictionary entry key.</returns>
YNode Default_Parser::parseKey(ISource &source) {
  std::string key{extractKey(source)};
  if (key.back() == kColon) {
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
  return convertYAMLToStringYNode(key);
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
  YNode keyYNode = parseKey(source);
  source.ignoreWS();
  if (isKey(source) && !isMapping(source)) {
    throw SyntaxError(source.getPosition(),
                      "Only an inline/compact dictionary is allowed.");
  }
  moveToNextIndent(source);
  YNode dictionaryYNode = YNode::make<Null>();
  if (source.more() &&
      (source.getPosition().second > keyIndent || isInlineArray(source) ||
       isInlineDictionary(source))) {
    dictionaryYNode = parseDocument(source, delimiters, indentation);
  }
  return {keyYNode, dictionaryYNode};
}
/// <summary>
/// Parse inline dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry Default_Parser::parseInlineKeyValue(ISource &source,
                                                 const Delimiters &delimiters,
                                                 const unsigned long indentation) {
  YNode keyYNode = parseKey(source);
  YNode dictionaryYNode = YNode::make<Null>();
  if (source.current() != kComma) {
    dictionaryYNode = parseDocument(source, delimiters, indentation);
  }
  if (isA<String>(dictionaryYNode)) {
    if (YRef<String>(dictionaryYNode).value().empty() &&
        YRef<String>(dictionaryYNode).getQuote() == kNull) {
      dictionaryYNode = YNode::make<Null>();
    }
  }
  return {keyYNode, dictionaryYNode};
}
/// <summary>
/// Parse a dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse dictionary.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary YNode.</returns>
YNode Default_Parser::parseDictionary(ISource &source,
                                   const Delimiters &delimiters,
                                   [[maybe_unused]] unsigned long indentation) {
  const unsigned long dictionaryIndent = source.getPosition().second;
  YNode dictionaryYNode = YNode::make<Dictionary>();
  while (source.more() && dictionaryIndent == source.getPosition().second) {
    if (isKey(source)) {
      auto entry = parseKeyValue(source, delimiters, dictionaryIndent);
      if (YRef<Dictionary>(dictionaryYNode).contains(entry.getKey())) {
        throw SyntaxError(source.getPosition(),
                          "Dictionary already contains key '" + entry.getKey() +
                              "'.");
      }
      YRef<Dictionary>(dictionaryYNode).add(std::move(entry));
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
  return mergeOverrides(dictionaryYNode);
}
/// <summary>
/// Parse inline dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse inline dictionary.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary YNode.</returns>
YNode Default_Parser::parseInlineDictionary(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters,
    const unsigned long indentation) {
  Delimiters inLineDictionaryDelimiters = {delimiters};
  inLineDictionaryDelimiters.insert({kComma, kRightCurlyBrace});
  YNode dictionaryYNode = YNode::make<Dictionary>();
  inlineDictionaryDepth++;
  do {
    source.next();
    moveToNextIndent(source);
    if (source.current() == kComma) {
      throw SyntaxError("Unexpected ',' in in-line dictionary.");
    }
    if (source.current() != kRightCurlyBrace) {
      auto entry = parseInlineKeyValue(source, inLineDictionaryDelimiters, indentation);
      if (YRef<Dictionary>(dictionaryYNode).contains(entry.getKey())) {
        throw SyntaxError(source.getPosition(),
                          "Dictionary already contains key '" + entry.getKey() +
                          "'.");
      }
      YRef<Dictionary>(dictionaryYNode).add(std::move(entry));
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
  return dictionaryYNode;
}

} // namespace YAML_Lib