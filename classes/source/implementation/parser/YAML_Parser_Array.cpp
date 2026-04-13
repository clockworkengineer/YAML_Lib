
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
/// Return true if node is an unquoted empty string (represents a null element
/// produced by a trailing comma in an inline collection).
/// </summary>
bool Default_Parser::isNullStringNode(const Node &node) {
  return isA<String>(node) && NRef<String>(node).value().empty() &&
         NRef<String>(node).getQuote() == kNull;
}

/// <summary>
/// Parse array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the array.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Array Node.</returns>
Node Default_Parser::parseArray(ISource &source, const Delimiters &delimiters,
                                [[maybe_unused]] unsigned long indentation) {
  const unsigned long arrayIndent = source.getPosition().second;
  auto arrayNode = Node::make<Array>();
  {
    DepthGuard depthGuard(arrayIndentLevel);
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
      NRef<Array>(arrayNode).add(std::move(yNode));
      moveToNextIndent(source);
    }
  } // arrayIndentLevel decremented here (even on exception)
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
    ISource &source, [[maybe_unused]] const Delimiters &delimiters,
    const unsigned long indentation) {
  const auto inLineArrayDelimiters =
      withExtras(delimiters, {kComma, kRightSquareBracket});
  auto arrayNode = Node::make<Array>();
  auto &yamlArray = NRef<Array>(arrayNode);
  {
    DepthGuard depthGuard(inlineArrayDepth);
    do {
      source.next();
      yamlArray.add(parseDocument(source, inLineArrayDelimiters, indentation));
      if (auto &element = yamlArray.value().back(); isNullStringNode(element)) {
        if (source.current() != kRightSquareBracket) {
          throw SyntaxError("Unexpected ',' in in-line array.");
        }
        yamlArray.value().pop_back();
      }
    } while (source.current() == kComma);
  } // inlineArrayDepth decremented here
  checkForEnd(source, kRightSquareBracket);
  source.ignoreWS();
  if (inlineArrayDepth == 0) {
    checkFlowDelimiter(source, delimiters);
  }
  return arrayNode;
}
} // namespace YAML_Lib