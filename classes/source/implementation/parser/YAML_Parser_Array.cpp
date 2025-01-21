
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
  indentLevel++;
  YNode yNode = YNode::make<Array>(arrayIndent);
  while (source.more() && isArray(source) &&
         arrayIndent == source.getPosition().second) {
    source.next();
    YRef<Array>(yNode).add(parseDocument(source, delimiters));
    moveToNextIndent(source);
  }
  if (isArray(source) && indentLevel == 1 &&
      arrayIndent > source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Invalid indentation for array element.");
  }
  indentLevel--;
  return yNode;
}
/// <summary>
/// Parse inline array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the inline array.</param>
/// <returns>Array YNode.</returns>
YNode YAML_Parser::parseInlineArray(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters) {
  unsigned long arrayIndent = source.getPosition().second;
  Delimiters inLineArrayDelimiters = {delimiters};
  inLineArrayDelimiters.insert({',', ']'});
  YNode yNode = YNode::make<Array>(arrayIndent);
  do {
    source.next();
    YRef<Array>(yNode).add(parseDocument(source, inLineArrayDelimiters));
    auto &element = YRef<Array>(yNode).value().back();
    if (isA<String>(element)) {
      if (YRef<String>(element).value().empty() &&
          YRef<String>(element).getQuote() == '\0') {
        if (source.current() != ']') {
          throw SyntaxError("Unexpected ',' in in-line array.");
        } else {
          YRef<Array>(yNode).value().pop_back();
        }
      }
    }
  } while (source.current() == ',');
  checkForEnd(source, ']');
  return yNode;
}
} // namespace YAML_Lib