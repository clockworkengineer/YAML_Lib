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
/// <returns>Number YNode.</returns>
YNode YAML_Parser::parseNumber(ISource &source, const Delimiters &delimiters,
                               unsigned long indentation) {
  YNode numberYNode;
  source.save();
  std::string numeric{extractToNext(source, delimiters)};
  rightTrim(numeric);
  if (Number number{numeric}; number.is<int>() || number.is<long>() ||
                              number.is<long long>() || number.is<float>() ||
                              number.is<double>() || number.is<long double>()) {
    moveToNext(source, delimiters);
    numberYNode = YNode::make<Number>(number);
  }
  if (numberYNode.isEmpty()) {
    source.restore();
  }
  return numberYNode;
}
/// <summary>
/// Parse None/Null on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse None.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>None YNode.</returns>
YNode YAML_Parser::parseNone(ISource &source, const Delimiters &delimiters,
                             unsigned long indentation) {
  YNode noneYNode;
  source.save();
  std::string none{extractToNext(source, delimiters)};
  rightTrim(none);
  if (none == "null" || none == "~") {
    noneYNode = YNode::make<Null>();
  }
  if (noneYNode.isEmpty()) {
    source.restore();
  }
  return noneYNode;
}
/// <summary>
/// Parse boolean value on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse boolean.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Boolean YNode.</returns>
YNode YAML_Parser::parseBoolean(ISource &source, const Delimiters &delimiters,
                                unsigned long indentation) {
  YNode booleanYNode;
  source.save();
  std::string boolean{extractToNext(source, delimiters)};
  rightTrim(boolean);
  if (Boolean::isTrue.contains(boolean)) {
    booleanYNode = YNode::make<Boolean>(true, boolean);
  } else if (Boolean::isFalse.contains(boolean)) {
    booleanYNode = YNode::make<Boolean>(false, boolean);
  }
  if (booleanYNode.isEmpty()) {
    source.restore();
  }
  return booleanYNode;
}

} // namespace YAML_Lib
