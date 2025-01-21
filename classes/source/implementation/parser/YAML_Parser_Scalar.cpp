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
/// <returns>Number YNode.</returns>
YNode YAML_Parser::parseNumber(ISource &source, const Delimiters &delimiters) {
  YNode yNode;
  source.save();
  std::string numeric{extractToNext(source, delimiters)};
  rightTrim(numeric);
  if (Number number{numeric}; number.is<int>() || number.is<long>() ||
                              number.is<long long>() || number.is<float>() ||
                              number.is<double>() || number.is<long double>()) {
    moveToNext(source, delimiters);
    moveToNextIndent(source);
    yNode = YNode::make<Number>(number);
  }
  if (yNode.isEmpty()) {
    source.restore();
  }
  return yNode;
}
/// <summary>
/// Parse None/Null on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse None.</param>
/// <returns>None YNode.</returns>
YNode YAML_Parser::parseNone(ISource &source, const Delimiters &delimiters) {
  YNode yNode;
  source.save();
  std::string none{extractToNext(source, delimiters)};
  rightTrim(none);
  if (none == "null" || none == "~") {
    yNode = YNode::make<Null>();
  }
  if (yNode.isEmpty()) {
    source.restore();
  }
  return yNode;
}
/// <summary>
/// Parse boolean value on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse boolean.</param>
/// <returns>Boolean YNode.</returns>
YNode YAML_Parser::parseBoolean(ISource &source, const Delimiters &delimiters) {
  YNode yNode;
  source.save();
  std::string boolean{extractToNext(source, delimiters)};
  rightTrim(boolean);
  if (Boolean::isTrue.contains(boolean)) {
    yNode = YNode::make<Boolean>(true, boolean);
  } else if (Boolean::isFalse.contains(boolean)) {
    yNode = YNode::make<Boolean>(false, boolean);
  }
  if (yNode.isEmpty()) {
    source.restore();
  }
  return yNode;
}

} // namespace YAML_Lib
