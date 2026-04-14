
//
// Class: YAML_Parser_FlowString
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

bool Default_Parser::isInlineComment(const ISource &source,
                                     const std::string &yamlString) {
  return source.current() == '#' && !yamlString.empty() &&
         (yamlString.back() == ' ' || yamlString.back() == '\t');
}

/// <summary>
/// Append character to YAML string performing any necessary newline folding.
/// YAML 1.2 §6.5 / §7.3.3: trailing white space (space or tab) on a source
/// line before a raw line break is excluded from the content.  Strip it before
/// appending a fold-space or an empty-line break character.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="yamlString">YAML string appended too.</param>
void Default_Parser::appendCharacterToString(ISource &source,
                                             std::string &yamlString,
                                             const bool escapeAware,
                                             const unsigned long minIndent) {
  if (source.current() == kLineFeed) {
    source.next();
    // YAML 1.2 §6.1: s-indent(n) consists of spaces only.  A tab at the
    // very first position of a continuation line (before any spaces) acts as
    // indentation, which is invalid when the context requires n>=1 spaces.
    // Only check when the caller supplies a non-zero minIndent — root-level
    // scalars (minIndent=0) allow a leading tab (s-indent(0) is empty).
    if (minIndent > 0 && source.more() && source.current() == '\t') {
      throw SyntaxError(source.getPosition(),
                        "Tab character not allowed in YAML block indentation.");
    }
    source.ignoreWS();
    // Strip trailing whitespace from the current line before folding.
    // In escape-aware mode (double-quoted strings) do not strip a space or tab
    // that is the second byte of an escape sequence (e.g. \<TAB> or \ ).
    while (!yamlString.empty() &&
           (yamlString.back() == kSpace || yamlString.back() == '\t')) {
      if (escapeAware && yamlString.size() >= 2 &&
          yamlString[yamlString.size() - 2] == '\\') {
        break; // stop — the trailing char is part of an escape sequence
      }
      yamlString.pop_back();
    }
    if (source.current() == kLineFeed) {
      yamlString += source.append();
      source.ignoreWS();
    } else {
      yamlString += kSpace;
    }
  } else {
    yamlString += source.append();
  }
}
/// <summary>
/// Parse plain flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>String Node.</returns>
Node Default_Parser::parsePlainFlowString(ISource &source,
                                          const Delimiters &delimiters,
                                          const unsigned long indentation) {
  // Extract the first-line content.  For single-line scalars rightTrim strips
  // all trailing whitespace + the sentinel space.  For multi-line scalars we
  // must strip trailing whitespace from the first line BEFORE adding the
  // fold-space, so that "hello   \nworld" → "hello world" (YAML 1.2 §6.5).
  std::string yamlString{extractToNext(source, delimiters)};
  // YAML 1.2 §6.8: '#' introduces a comment ONLY when preceded by whitespace.
  // If extraction stopped at '#' but the preceding character is NOT whitespace,
  // '#' is a literal — consume it and continue extracting to the next
  // delimiter.
  while (source.more() && source.current() == '#' && !yamlString.empty() &&
         !isInlineComment(source, yamlString)) {
    yamlString += source.append();                   // consume literal '#'
    yamlString += extractToNext(source, delimiters); // read to next delimiter
  }
  if (source.current() != kLineFeed) {
    rightTrim(yamlString);
  } else {
    rightTrim(yamlString); // strip trailing whitespace before fold
    yamlString += kSpace;  // fold first-line break to a single space
    moveToNextIndent(source);
    // In a flow context (inside [] or {}), a plain scalar must stop if the
    // continuation line begins with a flow indicator.  Leave the indicator
    // for the enclosing collection parser to consume.
    const bool stopAtFlowIndicator =
        isInsideFlowContext() &&
        (source.current() == kRightSquareBracket ||
         source.current() == kRightCurlyBrace || source.current() == kComma);
    while (!stopAtFlowIndicator && source.more() &&
           indentation < source.getPosition().second) {
      // Stop at document markers (--- or ...) at the start of a line.
      // This matters especially at indentation 0, where the column check
      // alone (0 < 1) would never exit the loop.
      if (source.getPosition().second == 1 && isDocumentBoundary(source)) {
        break;
      }
      // YAML 1.2 §6.8: inline comment on a continuation line — '#' after
      // whitespace in the accumulated string.  Consume the comment to the end
      // of the line; the '\n' is left for appendCharacterToString to fold.
      if (isInlineComment(source, yamlString)) {
        while (source.more() && source.current() != kLineFeed) {
          source.next();
        }
        continue;
      }
      appendCharacterToString(source, yamlString);
      if (source.match(": ")) {
        throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
      }
    }
    if (yamlString.back() == kSpace || yamlString.back() == kLineFeed) {
      yamlString.pop_back();
    }
  }
  // YAML 1.2 §7.3.3: in flow context '-', '?' and ':' may only start a plain
  // scalar when immediately followed by an ns-plain-safe character.  A scalar
  // that reduces to a single one of these characters means nothing safe
  // followed it — reject as invalid.
  if (isInsideFlowContext() && yamlString.size() == 1 &&
      (yamlString[0] == '-' || yamlString[0] == '?' || yamlString[0] == ':')) {
    throw SyntaxError(source.getPosition(),
                      "Bare '" + yamlString +
                          "' is not a valid plain scalar in flow context.");
  }
  return Node::make<String>(yamlString, kNull);
}
/// <summary>
/// Parse quoted flow string on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse string.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>String Node.</returns>
Node Default_Parser::parseQuotedFlowString(ISource &source,
                                           const Delimiters &delimiters,
                                           const unsigned long indentation) {
  const char quote = source.append();
  std::string yamlString;
  if (quote == kDoubleQuote) {
    while (source.more() && source.current() != quote) {
      if (source.current() == '\\') {
        yamlString += source.append();
        yamlString += source.append();
      } else {
        appendCharacterToString(source, yamlString, true, indentation);
      }
    }
    yamlString = yamlTranslator->from(yamlString);
  } else {
    while (source.more()) {
      if (source.current() == quote) {
        source.next();
        if (source.current() == quote) {
          yamlString += source.append();
        } else {
          break;
        }
      } else {
        appendCharacterToString(source, yamlString);
      }
    }
  }
  moveToNext(source, delimiters);
  return Node::make<String>(yamlString, quote);
}

} // namespace YAML_Lib