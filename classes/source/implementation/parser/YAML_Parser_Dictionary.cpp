
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
YNode YAML_Parser::convertYAMLToStringYNode(const std::string &yamlString) {
  BufferSource yamlKey{yamlString + kLineFeed};
  auto keyYNode = parseDocument(yamlKey, {kLineFeed});
  std::string keyString{YRef<Variant>(keyYNode).toKey()};
  char quote = '\"';
  if (isA<String>(keyYNode)) {
    quote = YRef<String>(keyYNode).getQuote();
    if (keyString.empty()) {
      quote = '"';
    }
  }
  return YNode::make<String>(keyString, quote, 0);
}
/// <summary>
/// Does YAML that is  passed in constitute a valid dictionary key?
/// </summary>
/// <param name="key">YAML sequence to be converted to be used as the
/// key.</param> <returns>==true value is a valid key.</returns>
bool YAML_Parser::isValidKey(const std::string &key) {
  try {
    BufferSource yamlKey{key + kLineFeed};
    const YNode keyYNode = parseDocument(yamlKey, {kLineFeed});
    return !keyYNode.isEmpty() && !isA<Comment>(keyYNode);
  } catch ([[maybe_unused]] const std::exception &e) {
    return false;
  }
}
/// <summary>
/// Extract YAML used in possible key value from source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>YAML for key value.</returns>
std::string YAML_Parser::extractKey(ISource &source) {
  Delimiters delimiters;
  if (isInlineDictionary(source)) {
    return extractInLine(source, '{', '}');
  } else if (source.current() == '?') {
    source.next();
    delimiters = {':'};
  } else if (source.current() == '[')
    return extractInLine(source, '[', ']');
  else {
    delimiters = {':', ',', '}', kLineFeed};
  }
  std::string key = extractToNext(source, delimiters);
  if (delimiters.contains('}') && source.current() == '}' && key[0] == '{') {
    key += source.append();
    source.ignoreWS();
  }
  return key;
}

YNode YAML_Parser::parseKey(ISource &source) {
  std::string key{extractKey(source)};
  if (source.more() && source.current() != '}' && source.current() != ',') {
    source.next();
  }
  rightTrim(key);
  if (!isValidKey(key)) {
    throw SyntaxError(source.getPosition(),
                      "Invalid key '" + key + "' specified.");
  }
  return convertYAMLToStringYNode(key);
}
// <summary>
/// Parse dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry YAML_Parser::parseKeyValue(ISource &source,
                                           const Delimiters &delimiters) {
  const unsigned long keyIndent = source.getPosition().second;
  YNode keyYNode = parseKey(source);
  source.ignoreWS();
  if (isKey(source)) {
    throw SyntaxError(source.getPosition(),
                      "Only an inline/compact dictionary is allowed.");
  }
  moveToNextIndent(source);
  YNode yNode{YNode::make<Null>()};
  if ((source.getPosition().second > keyIndent || isInlineArray(source) ||
       isInlineDictionary(source))) {
    yNode = parseDocument(source, delimiters);
  }
  return {keyYNode, yNode};
}
/// <summary>
/// Parse inline dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry YAML_Parser::parseInlineKeyValue(ISource &source,
                                                 const Delimiters &delimiters) {
  YNode keyYNode = parseKey(source);
  // source.ignoreWS();
  // moveToNextIndent(source);
  YNode yNode{YNode::make<Null>()};
  if (source.current() != ',') {
    yNode = parseDocument(source, delimiters);
  }
  if (isA<String>(yNode)) {
    if (YRef<String>(yNode).value().empty() &&
        YRef<String>(yNode).getQuote() == '\0') {
      yNode = YNode::make<Null>();
    }
  }
  return {keyYNode, yNode};
}
/// <summary>
/// Parse a dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse dictionary.</param>
/// <returns>Dictionary YNode.</returns>
YNode YAML_Parser::parseDictionary(ISource &source,
                                   const Delimiters &delimiters) {

  if (delimiters.contains('}')) {
    return {};
  }
  unsigned long dictionaryIndent = source.getPosition().second;
  YNode yNode = YNode::make<Dictionary>(dictionaryIndent);
  while (source.more() && dictionaryIndent == source.getPosition().second) {
    if (isKey(source)) {
      auto entry = parseKeyValue(source, delimiters);
      if (YRef<Dictionary>(yNode).contains(entry.getKey())) {
        throw SyntaxError(source.getPosition(),
                          "Dictionary already contains key '" + entry.getKey() +
                              "'.");
      }
      YRef<Dictionary>(yNode).add(std::move(entry));
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
  return mergeOverrides(yNode);
}
/// <summary>
/// Parse inline dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse inline
/// dictionary.</param> <returns>Dictionary YNode.</returns>
YNode YAML_Parser::parseInlineDictionary(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters) {
  Delimiters inLineDictionaryDelimiters = {delimiters};
  inLineDictionaryDelimiters.insert({',', '}'});
  unsigned long dictionaryIndent = source.getPosition().second;
  YNode yNode = YNode::make<Dictionary>(dictionaryIndent);
  do {
    source.next();
    moveToNextIndent(source);
    if (source.current() == ',') {
      throw SyntaxError("Unexpected ',' in in-line dictionary.");
    } else if (source.current() != '}') {
      auto entry = parseInlineKeyValue(source, inLineDictionaryDelimiters);
      if (YRef<Dictionary>(yNode).contains(entry.getKey())) {
        throw SyntaxError(source.getPosition(),
                          "Dictionary already contains key '" + entry.getKey() +
                              "'.");
      }
      YRef<Dictionary>(yNode).add(std::move(entry));
    }

  } while (source.current() == ',');
  checkForEnd(source, '}');
  if (source.current() == ':') {
    throw SyntaxError(
        source.getPosition(),
        "Inline dictionary used as key is meant to be on one line.");
  }
  return yNode;
}

} // namespace YAML_Lib