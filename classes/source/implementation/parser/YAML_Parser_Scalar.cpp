//
// Class: YAML_Parser_Scalar
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Parse a numeric value on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse number./param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Number Node.</returns>
Node Default_Parser::parseNumber(ISource &source, const Delimiters &delimiters,
                               [[maybe_unused]] unsigned long indentation) {
  Node numberNode;
  source.save();
  std::string numeric{extractToNext(source, delimiters)};
  rightTrim(numeric);
  if (Number number{numeric}; number.is<int>() || number.is<long>() ||
                              number.is<long long>() || number.is<float>() ||
                              number.is<double>() || number.is<long double>()) {
    moveToNext(source, delimiters);
    numberNode = Node::make<Number>(number);
  }
  if (numberNode.isEmpty()) {
    source.restore();
  }
  return numberNode;
}
/// <summary>
/// Parse None/Null on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse None.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>None Node.</returns>
Node Default_Parser::parseNone(ISource &source, const Delimiters &delimiters,
                             [[maybe_unused]] unsigned long indentation) {
  Node noneNode;
  source.save();
  std::string none{extractToNext(source, delimiters)};
  rightTrim(none);
  if (none == "null" || none == "~") {
    noneNode = Node::make<Null>();
  }
  if (noneNode.isEmpty()) {
    source.restore();
  }
  return noneNode;
}
/// <summary>
/// Parse boolean value on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse boolean.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Boolean Node.</returns>
Node Default_Parser::parseBoolean(ISource &source, const Delimiters &delimiters,
                                [[maybe_unused]] unsigned long indentation) {
  Node booleanNode;
  source.save();
  std::string boolean{extractToNext(source, delimiters)};
  rightTrim(boolean);
  if (Boolean::isTrue.contains(boolean)) {
    booleanNode = Node::make<Boolean>(true, boolean);
  } else if (Boolean::isFalse.contains(boolean)) {
    booleanNode = Node::make<Boolean>(false, boolean);
  }
  if (booleanNode.isEmpty()) {
    source.restore();
  }
  return booleanNode;
}

} // namespace YAML_Lib
