
//
// Class: YAML_Parser_BlockString
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Parse any block string chomping and optional explicit indentation indicator.
/// Handles either order: digit-then-chomping (|2-) or chomping-then-digit
/// (|-2). Per YAML 1.2 §8.1.1.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Pair of chomping indicator and explicit indent (0 =
/// auto-detect).</returns>
std::pair<Default_Parser::BlockChomping, int>
Default_Parser::parseBlockChomping(ISource &source) {
  source.next(); // consume '|' or '>'
  BlockChomping chomping = BlockChomping::clip;
  int explicitIndent = 0;
  // Both orders are legal: digit-then-indicator (|2-) and indicator-then-digit
  // (|-2).
  for (int i = 0; i < 2; ++i) {
    const auto ch = source.current();
    if (ch >= '1' && ch <= '9' && explicitIndent == 0) {
      explicitIndent = ch - '0';
      source.next();
    } else if (ch == '-' && chomping == BlockChomping::clip) {
      chomping = BlockChomping::strip;
      source.next();
    } else if (ch == '+' && chomping == BlockChomping::clip) {
      chomping = BlockChomping::keep;
      source.next();
    } else {
      break;
    }
  }
  return {chomping, explicitIndent};
}
/// <summary>
/// Parse a block string.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used in parsing.</param>
/// <param name="indentation">Parent indentation.</param>
/// <param name="fillerDefault">Default filler.</param>
/// <returns>Block string parsed.</returns>
std::string Default_Parser::parseBlockString(ISource &source,
                                             const Delimiters &delimiters,
                                             unsigned long indentation,
                                             const char fillerDefault) {
  const auto [chomping, explicitIndent] = parseBlockChomping(source);
  moveToNext(source, delimiters);
  moveToNextIndent(source);
  // Use the explicit indent indicator when present (YAML 1.2 §8.1.1);
  // otherwise auto-detect from the first content line's column.
  const unsigned long blockIndent =
      (explicitIndent > 0)
          ? indentation + static_cast<unsigned long>(explicitIndent)
          : source.getPosition().second;
  std::string yamlString{};
  do {
    char filler{fillerDefault};
    if (blockIndent < source.getPosition().second) {
      if (!yamlString.empty() && yamlString.back() != kLineFeed) {
        yamlString += kLineFeed;
      }
      yamlString += std::string(source.getPosition().second - 1, kSpace);
      filler = kLineFeed;
    }
    yamlString += extractToNext(source, delimiters);
    yamlString += filler;
    if (source.more()) {
      source.next();
    }
    while (source.more() && (source.isWS() || source.current() == kLineFeed)) {
      if (source.current() == kLineFeed) {
        yamlString.pop_back();
        yamlString += "\n\n";
      }
      source.next();
    }
  } while (source.more() && blockIndent <= source.getPosition().second);
  if (chomping == BlockChomping::clip || chomping == BlockChomping::strip) {
    if (endsWith(yamlString, "\n\n\n")) {
      yamlString.pop_back();
    }
    yamlString.pop_back();
  }
  if (chomping == BlockChomping::strip && yamlString.back() == kLineFeed) {
    yamlString.pop_back();
  }
  if (chomping == BlockChomping::keep && source.more() &&
      source.current() == kLineFeed) {
    yamlString += kLineFeed;
  }
  return yamlString;
}

/// <summary>
/// Parse folded block string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>String Node.</returns>
Node Default_Parser::parseFoldedBlockString(ISource &source,
                                            const Delimiters &delimiters,
                                            const unsigned long indentation) {
  return Node::make<String>(
      parseBlockString(source, delimiters, indentation, kSpace), '>');
}
/// <summary>
/// Parse literal block string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>String Node.</returns>
Node Default_Parser::parseLiteralBlockString(ISource &source,
                                             const Delimiters &delimiters,
                                             const unsigned long indentation) {
  return Node::make<String>(
      parseBlockString(source, delimiters, indentation, kLineFeed), '|');
}
} // namespace YAML_Lib