
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
    if (ch == '0') {
      // YAML 1.2 §8.1.1: indentation indicator must be 1-9; 0 is invalid.
      YAML_THROW_POS(source, "Block scalar indentation indicator must be 1-9; "
                        "0 is not allowed.");
    }
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
/// Advance source to the first non-blank content line of a block scalar,
/// tracking the maximum number of leading spaces seen on blank lines.
/// A blank line is defined here as a line consisting entirely of U+0020 space
/// characters (any other character — including tab — terminates space-counting
/// and marks the line as content, not blank).
/// If source is positioned at '#' on entry (a comment on the block scalar
/// header line, positioned there by moveToNext), that comment line is skipped.
/// Does NOT throw on tabs — tabs are valid literal content in block scalars.
/// Does NOT skip '#'-started content lines — '#' inside a block is literal.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Maximum leading-space count seen on any blank line.</returns>
unsigned long Default_Parser::scanToFirstBlockContent(ISource &source) {
  // Header-line comment: moveToNext may have stopped at '#'.
  if (source.more()) {
    skipIfComment(source);
  }
  unsigned long maxBlankLeadingSpaces = 0;
  while (source.more()) {
    if (source.current() == kLineFeed) {
      source.next();
      continue;
    }
    // Count leading U+0020 spaces only (stop at tab or any other char).
    unsigned long lineSpaces = 0;
    while (source.more() && source.current() == kSpace) {
      ++lineSpaces;
      source.next();
    }
    // Blank line: nothing after the spaces before the next newline (or EOF).
    if (!source.more() || source.current() == kLineFeed) {
      maxBlankLeadingSpaces = std::max(maxBlankLeadingSpaces, lineSpaces);
      continue;
    }
    // First non-blank line found; source positioned at first non-space char.
    break;
  }
  return maxBlankLeadingSpaces;
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
  // YAML 1.2 §8.1.1: after '|' / '>' and optional chomping/indent indicators,
  // only separation whitespace and an optional comment are allowed on the
  // header line. Plain text (e.g. "> first") is invalid.
  if (source.more() && source.current() != kLineFeed) {
    if (source.current() == '#') {
      YAML_THROW_POS(source, "Block scalar comment must be preceded by whitespace.");
    }
    if (source.current() != kSpace && source.current() != '\t') {
      YAML_THROW_POS(source, "Invalid text after block scalar indicator.");
    }
    while (source.more() &&
           (source.current() == kSpace || source.current() == '\t')) {
      source.next();
    }
    if (source.more() && source.current() != kLineFeed) {
      if (source.current() == '#') {
        moveToNext(source, {kLineFeed});
      } else {
        YAML_THROW_POS(source, "Invalid text after block scalar indicator.");
      }
    }
  }
  // YAML 1.2 §8.1.1: a comment on the block scalar header line must be
  // preceded by at least one whitespace character.  '># comment' (no space)
  // is a syntax error.
  if (source.more() && isComment(source)) {
    YAML_THROW_POS(source, "Block scalar comment must be preceded by whitespace.");
  }
  moveToNext(source, delimiters);
  // Advance to first content, tracking blank-line leading spaces for W9L4.
  const unsigned long maxBlankLeadingSpaces = scanToFirstBlockContent(source);
  // Use the explicit indent indicator when present (YAML 1.2 §8.1.1);
  // otherwise auto-detect from the first content line's column.
  const unsigned long blockIndent =
      (explicitIndent > 0)
          ? indentation + static_cast<unsigned long>(explicitIndent)
          : source.getPosition().second;
  // YAML 1.2 §6.1: Tab characters are NOT valid block indentation — only
  // spaces count.  If the first content line starts with a TAB (meaning 0
  // leading spaces) at a column that cannot distinguish the block scalar from
  // its parent context (column <= parent indentation), the input is invalid.
  // Example: "foo: |\n\t\nbar: 1" — the \t line has 0 space-indentation and
  // sits at column 1 = parent indentation 1, so the block border is ambiguous.
  // Note: a leading space BEFORE the tab is fine (e.g. " \t" at column 2
  // when parent indent is 1) because the space establishes proper indentation.
  if (explicitIndent == 0 && source.more() && source.current() == '\t' &&
      source.getPosition().second <= indentation) {
    YAML_THROW_POS(source, "Tab character used as block scalar indentation; "
                      "block scalar content must be more indented than its "
                      "parent context with spaces (not tabs).");
  }
  // YAML 1.2 §8.1.1: blank lines before block content may not have more
  // leading spaces than the block indentation level (test case W9L4).
  if (source.more() && maxBlankLeadingSpaces >= blockIndent) {
    YAML_THROW_POS(source, "Block scalar blank line has more leading spaces than "
                      "block indentation level.");
  }
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