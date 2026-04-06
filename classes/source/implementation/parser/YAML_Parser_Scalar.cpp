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
/// Supports standard integers/floats, YAML 1.2 hex (0x), octal (0o),
/// and special float values .inf, -.inf, .nan (case-insensitive).
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
  // YAML 1.2 special float literals (case-insensitive).
  {
    std::string lower = numeric;
    std::transform(
        lower.begin(), lower.end(), lower.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (lower == ".inf" || lower == "+.inf") {
      moveToNext(source, delimiters);
      numberNode = Node::make<Number>(std::numeric_limits<double>::infinity());
    } else if (lower == "-.inf") {
      moveToNext(source, delimiters);
      numberNode = Node::make<Number>(-std::numeric_limits<double>::infinity());
    } else if (lower == ".nan") {
      moveToNext(source, delimiters);
      numberNode = Node::make<Number>(std::numeric_limits<double>::quiet_NaN());
    }
  }
  if (numberNode.isEmpty()) {
    // YAML 1.2 octal "0o<digits>" (or "0O<digits>"): convert the octal digits
    // to their decimal string equivalent so that Number parses them as base 10.
    // This avoids relying on C-style "0NNN" leading-zero octal interpretation.
    if (numeric.size() >= 3 && numeric[0] == '0' &&
        (numeric[1] == 'o' || numeric[1] == 'O')) {
      const std::string octalDigits = numeric.substr(2);
      try {
        std::size_t end = 0;
        const long long val = std::stoll(octalDigits, &end, 8);
        if (end == octalDigits.size()) {
          numeric = std::to_string(val); // e.g. "0o17" -> "15"
        } else {
          numeric.clear(); // malformed octal literal; don't try as a number
        }
      } catch (...) {
        numeric.clear();
      }
    } else if (yamlDirectiveMinor == 1 && numeric.size() >= 2 &&
               numeric[0] == '0' &&
               std::all_of(
                   numeric.begin() + 1, numeric.end(),
                   [](unsigned char c) { return c >= '0' && c <= '7'; })) {
      // YAML 1.1: C-style octal "0NNN" (leading zero, digits 0-7 only)
      try {
        std::size_t end = 0;
        const long long val = std::stoll(numeric, &end, 8);
        if (end == numeric.size()) {
          numeric = std::to_string(val); // e.g. "0777" -> "511"
        } else {
          numeric.clear();
        }
      } catch (...) {
        numeric.clear();
      }
    }
    if (!numeric.empty()) {
      if (Number number{numeric}; number.is<int>() || number.is<long>() ||
                                  number.is<long long>() ||
                                  number.is<float>() || number.is<double>() ||
                                  number.is<long double>()) {
        moveToNext(source, delimiters);
        numberNode = Node::make<Number>(number);
      }
    }
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
  static const std::set<std::string_view> strict12True{"true"};
  static const std::set<std::string_view> strict12False{"false"};
  Node booleanNode;
  source.save();
  std::string boolean{extractToNext(source, delimiters)};
  rightTrim(boolean);
  const bool strictMode = strictBooleans || yamlDirectiveMinor >= 2;
  const auto &trueSet = strictMode ? strict12True : Boolean::isTrue;
  const auto &falseSet = strictMode ? strict12False : Boolean::isFalse;
  if (trueSet.contains(boolean)) {
    booleanNode = Node::make<Boolean>(true, boolean);
  } else if (falseSet.contains(boolean)) {
    booleanNode = Node::make<Boolean>(false, boolean);
  }
  if (booleanNode.isEmpty()) {
    source.restore();
  }
  return booleanNode;
}

} // namespace YAML_Lib
