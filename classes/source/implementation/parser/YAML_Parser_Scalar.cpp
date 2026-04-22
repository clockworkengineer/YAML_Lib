//
// Class: YAML_Parser_Scalar
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

void Default_Parser::convertOctalToDecimal(std::string &numeric,
                                           const std::string &digits) {
  try {
    std::size_t end = 0;
    const long long val = std::stoll(digits, &end, 8);
    if (end == digits.size()) {
      numeric = std::to_string(val);
    } else {
      numeric.clear();
    }
  } catch (...) {
    numeric.clear();
  }
}

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
  return tryParseToken(
      source, delimiters,
      [](std::string numeric) -> Node {
        // YAML 1.2 special float literals (case-insensitive).
        {
          std::string lower = numeric;
          std::transform(lower.begin(), lower.end(), lower.begin(),
                         [](unsigned char c) {
                           return static_cast<char>(std::tolower(c));
                         });
          if (lower == ".inf" || lower == "+.inf") {
            return Node::make<Number>(std::numeric_limits<double>::infinity());
          } else if (lower == "-.inf") {
            return Node::make<Number>(-std::numeric_limits<double>::infinity());
          } else if (lower == ".nan") {
            return Node::make<Number>(std::numeric_limits<double>::quiet_NaN());
          }
        }
        // YAML 1.2 octal "0o<digits>" (or "0O<digits>"): convert the octal
        // digits to their decimal string equivalent so that Number parses them
        // as base 10. This avoids relying on C-style "0NNN" leading-zero octal
        // interpretation.
        if (numeric.size() >= 3 && numeric[0] == '0' &&
            (numeric[1] == 'o' || numeric[1] == 'O')) {
          const std::string octalDigits = numeric.substr(2);
          convertOctalToDecimal(numeric, octalDigits);
        } else if (yamlDirectiveMinor == 1 && numeric.size() >= 2 &&
                   numeric[0] == '0' &&
                   std::all_of(
                       numeric.begin() + 1, numeric.end(),
                       [](unsigned char c) { return c >= '0' && c <= '7'; })) {
          // YAML 1.1: C-style octal "0NNN" (leading zero, digits 0-7 only)
          convertOctalToDecimal(numeric, numeric);
        }
        if (!numeric.empty()) {
          if (Number number{numeric};
              number.is<int>() || number.is<long>() || number.is<long long>() ||
              number.is<float>() || number.is<double>() ||
              number.is<long double>()) {
            return Node::make<Number>(number);
          }
        }
        return {};
      },
      indentation);
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
  return tryParseToken(
      source, delimiters,
      [](const std::string &tok) -> Node {
        if (tok == "null" || tok == "~")
          return Node::make<Null>();
        return {};
      },
      indentation);
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
  return tryParseToken(
      source, delimiters,
      [&](const std::string &tok) -> Node {
        const bool strictMode = strictBooleans || yamlDirectiveMinor >= 2;
        const auto &trueSet = strictMode ? strict12True : Boolean::isTrue;
        const auto &falseSet = strictMode ? strict12False : Boolean::isFalse;
        if (trueSet.contains(tok))
          return Node::make<Boolean>(true, tok);
        if (falseSet.contains(tok))
          return Node::make<Boolean>(false, tok);
        return {};
      },
      indentation);
}

} // namespace YAML_Lib
