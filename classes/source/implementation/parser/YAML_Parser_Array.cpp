
//
// Class: YAML_Parser_Array
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Parse array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the array.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Array Node.</returns>
Node Default_Parser::parseArray(ISource &source, const Delimiters &delimiters, [[maybe_unused]] unsigned long indentation) {
  const unsigned long arrayIndent = source.getPosition().second;
  arrayIndentLevel++;
  auto arrayNode = Node::make<Array>();
  while (isArray(source) && arrayIndent == source.getPosition().second) {
    source.next();
    source.ignoreWS();
    Node yNode = Node::make<Null>();
    if (source.current() != kLineFeed) {
      yNode = parseDocument(source, delimiters, arrayIndent);
    } else {
      moveToNextIndent(source);
      if (arrayIndent < source.getPosition().second) {
        yNode = parseDocument(source, delimiters, arrayIndent);
      }
    }
    YRef<Array>(arrayNode).add(std::move(yNode));
    moveToNextIndent(source);
  }
  arrayIndentLevel--;
  if (isArray(source) && arrayIndentLevel == 0 &&
      arrayIndent > source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Invalid indentation for array element.");
  }
  return arrayNode;
}
/// <summary>
/// Parse inline array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the inline array.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Array Node.</returns>
Node Default_Parser::parseInlineArray(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters, const unsigned long indentation) {
  inlineArrayDepth++;
  Delimiters inLineArrayDelimiters = {delimiters};
  inLineArrayDelimiters.insert({kComma, kRightSquareBracket});
  auto arrayNode = Node::make<Array>();
  auto &yamlArray = YRef<Array>(arrayNode);
  do {
    source.next();
    yamlArray.add(parseDocument(source, inLineArrayDelimiters, indentation));
    if (auto &element = yamlArray.value().back(); isA<String>(element)) {
      if (YRef<String>(element).value().empty() &&
          YRef<String>(element).getQuote() == kNull) {
        if (source.current() != kRightSquareBracket) {
          throw SyntaxError("Unexpected ',' in in-line array.");
        }
        yamlArray.value().pop_back();
      }
    }
  } while (source.current() == kComma);
  inlineArrayDepth--;
  checkForEnd(source, kRightSquareBracket);
  source.ignoreWS();
  if (source.more() && inlineArrayDepth == 0) {
    if (!delimiters.contains(source.current())) {
      throw SyntaxError("Unexpected flow sequence token '" +
                        std::string(1, source.current()) + "'.");
    }
  }
  return arrayNode;
}
} // namespace YAML_Lib