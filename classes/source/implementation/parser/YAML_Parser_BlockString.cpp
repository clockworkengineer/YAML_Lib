
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
/// Parse any block string chomping.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Specified block chomping.</returns>
Default_Parser::BlockChomping Default_Parser::parseBlockChomping(ISource &source) {
  source.next();
  if (const auto ch = source.current(); ch == '-') {
    return BlockChomping::strip;
  } else {
    if (ch == '+') {
      return BlockChomping::keep;
    }
    return BlockChomping::clip;
  }
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
                                          [[maybe_unused]] unsigned long indentation,
                                          const char fillerDefault) {
  const BlockChomping chomping{parseBlockChomping(source)};
  moveToNext(source, delimiters);
  moveToNextIndent(source);
  const unsigned long blockIndent = source.getPosition().second;
  std::string yamlString{};
  do {
    char filler{fillerDefault};
    if (blockIndent < source.getPosition().second) {
      if (yamlString.back() != kLineFeed) {
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
/// <returns>String YNode.</returns>
YNode Default_Parser::parseFoldedBlockString(ISource &source,
                                          const Delimiters &delimiters,
                                          const unsigned long indentation) {
  return YNode::make<String>(parseBlockString(source, delimiters, indentation, kSpace), '>');
}
/// <summary>
/// Parse literal block string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>String YNode.</returns>
YNode Default_Parser::parseLiteralBlockString(ISource &source,
                                           const Delimiters &delimiters,
                                           const unsigned long indentation) {
  return YNode::make<String>(parseBlockString(source, delimiters, indentation, kLineFeed),
                             '|');
}
} // namespace YAML_Lib