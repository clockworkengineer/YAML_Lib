
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
/// <returns>Array YNode.</returns>
YNode YAML_Parser::parseArray(ISource &source, const Delimiters &delimiters) {
  unsigned long arrayIndent = source.getPosition().second;
  arrayIndentLevel++;
  YNode arrayYNode = YNode::make<Array>();
  while (source.more() && isArray(source) &&
         arrayIndent == source.getPosition().second) {
    source.next();
    YRef<Array>(arrayYNode).add(parseDocument(source, delimiters));
    moveToNextIndent(source);
  }
  arrayIndentLevel--;
  if (isArray(source) && arrayIndentLevel == 0 &&
      arrayIndent > source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Invalid indentation for array element.");
  }
  return arrayYNode;
}
/// <summary>
/// Parse inline array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the inline array.</param>
/// <returns>Array YNode.</returns>
YNode YAML_Parser::parseInlineArray(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters) {
  inlineArrayDepth++;
  Delimiters inLineArrayDelimiters = {delimiters};
  inLineArrayDelimiters.insert({',', ']'});
  YNode arrayYNode = YNode::make<Array>();
  do {
    source.next();
    YRef<Array>(arrayYNode).add(parseDocument(source, inLineArrayDelimiters));
    auto &element = YRef<Array>(arrayYNode).value().back();
    if (isA<String>(element)) {
      if (YRef<String>(element).value().empty() &&
          YRef<String>(element).getQuote() == '\0') {
        if (source.current() != ']') {
          throw SyntaxError("Unexpected ',' in in-line array.");
        } else {
          YRef<Array>(arrayYNode).value().pop_back();
        }
      }
    }
  } while (source.current() == ',');
  inlineArrayDepth--;
  checkForEnd(source, ']');
  source.ignoreWS();
  if (source.more() && inlineArrayDepth == 0) {
    if (!delimiters.contains(source.current())) {
      throw SyntaxError("Unexpected flow sequence token '" +
                        std::string(1, source.current()) + "'.");
    }
  }
  return arrayYNode;
}
} // namespace YAML_Lib