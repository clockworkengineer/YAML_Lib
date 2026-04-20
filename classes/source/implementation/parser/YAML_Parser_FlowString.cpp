
//
// Class: YAML_Parser_FlowString
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

namespace {

void prepareQuotedContinuationLine(ISource &source,
                                   const unsigned long minIndent) {
  if (minIndent > 0 && source.more() && source.current() == '\t') {
    throw SyntaxError(source.getPosition(),
                      "Tab character not allowed in YAML block indentation.");
  }
  source.ignoreWS();
  if (minIndent > 0 && source.more() && source.current() != kLineFeed &&
      source.getPosition().second <= minIndent) {
    throw SyntaxError(
        source.getPosition(),
        "Multiline quoted scalar continuation must be indented beyond its "
        "parent context.");
  }
}

} // namespace

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
    prepareQuotedContinuationLine(source, minIndent);
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
    const bool commentTerminatesBlockPlainScalar = [&]() {
      if (isInsideFlowContext()) {
        return false;
      }
      SourceGuard guard(source);
      source.next();
      while (source.more() && source.current() == kSpace) {
        source.next();
      }
      return source.more() && source.current() == '#';
    }();
    if (commentTerminatesBlockPlainScalar) {
      if (!yamlString.empty() && yamlString.back() == kSpace) {
        yamlString.pop_back();
      }
      return Node::make<String>(yamlString, kNull);
    }
    bool commentOnlyContinuationInFlow = false;
    if (isInsideFlowContext()) {
      SourceGuard guard(source);
      while (source.more()) {
        if (source.current() == kLineFeed) {
          source.next();
          continue;
        }
        while (source.more() && source.isWS()) {
          source.next();
        }
        if (!source.more() || source.current() == kLineFeed) {
          continue;
        }
        if (source.current() == '#') {
          commentOnlyContinuationInFlow = true;
          skipLine(source);
          continue;
        }
        break;
      }
    }
    moveToNextIndent(source);
    if (commentOnlyContinuationInFlow) {
      if (!yamlString.empty() && yamlString.back() == kSpace) {
        yamlString.pop_back();
      }
      if (source.more() && source.current() != kComma &&
          source.current() != kRightSquareBracket &&
          source.current() != kRightCurlyBrace) {
        throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
      }
    }
    while (source.more() && indentation < source.getPosition().second) {
      // In flow context, a continuation line may legally reduce to a comment
      // followed by a separator on the next line. Re-check the current token
      // on each iteration so commas/closers revealed after skipping comments
      // remain collection syntax rather than scalar content.
      const bool stopAtFlowIndicator = [&]() {
        if (!isInsideFlowContext()) {
          return false;
        }
        if (source.current() == kRightSquareBracket ||
            source.current() == kRightCurlyBrace ||
            source.current() == kComma) {
          return true;
        }
        if (source.current() != kColon) {
          return false;
        }
        SourceGuard guard(source);
        source.next();
        return !source.more() || source.isWS() ||
               source.current() == kLineFeed || source.current() == kComma ||
               source.current() == kRightSquareBracket ||
               source.current() == kRightCurlyBrace;
      }();
      if (stopAtFlowIndicator) {
        break;
      }
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
        skipLine(source);
        moveToNextIndent(source);
        if (!yamlString.empty() && yamlString.back() == kSpace) {
          yamlString.pop_back();
        }
        if (isInsideFlowContext()) {
          if (!source.more() || source.current() == kComma ||
              source.current() == kRightSquareBracket ||
              source.current() == kRightCurlyBrace) {
            break;
          }
          throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
        }
        if (source.more() && indentation < source.getPosition().second &&
            !isDocumentBoundary(source)) {
          throw SyntaxError(
              source.getPosition(),
              "Block plain scalar cannot continue after an inline comment.");
        }
        break;
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
  bool closedQuote = false;
  if (quote == kDoubleQuote) {
    while (source.more() && source.current() != quote) {
      // YAML 1.2: document-start (---) and document-end (...) markers at
      // column 1 terminate a flow scalar.  Inside a double-quoted string that
      // is always a syntax error (test 5TRB / 9MQT).
      // '---' at column 1 is ALWAYS a document-start marker.
      // '...' at column 1 is a document-end marker only when followed by
      // whitespace or EOF ('...x' without space is valid literal content).
      if (source.getPosition().second == 1) {
        if (isDocumentStart(source)) {
          throw SyntaxError(source.getPosition(),
                            "Document start marker inside double-quoted "
                            "string.");
        }
        {
          SourceGuard guard(source);
          if (source.match("...") && (!source.more() || source.isWS() ||
                                      source.current() == kLineFeed)) {
            throw SyntaxError(source.getPosition(),
                              "Document end marker inside double-quoted "
                              "string.");
          }
        }
      }
      if (source.current() == '\\') {
        source.next(); // consume '\'
        if (source.more() && source.current() == kLineFeed) {
          // YAML 1.2 §7.3.1: \<newline> is a line continuation — discard
          // the backslash, the newline, and all leading white space on the
          // continuation line.
          source.next(); // consume LF
          prepareQuotedContinuationLine(source, indentation);
        } else {
          yamlString += '\\';
          if (source.more()) {
            yamlString += source.append();
          }
        }
      } else {
        appendCharacterToString(source, yamlString, true, indentation);
      }
    }
    if (!source.more() || source.current() != quote) {
      throw SyntaxError(source.getPosition(), "Missing closing quote.");
    }
    source.next(); // consume closing quote
    closedQuote = true;
    yamlString = yamlTranslator->from(yamlString);
  } else {
    while (source.more()) {
      // YAML 1.2: document-start (---) and document-end (...) markers at
      // column 1 terminate a flow scalar. Inside a single-quoted string that
      // is a syntax error (RXY3).
      if (source.getPosition().second == 1) {
        if (isDocumentStart(source)) {
          throw SyntaxError(source.getPosition(),
                            "Document start marker inside single-quoted "
                            "string.");
        }
        {
          SourceGuard guard(source);
          if (source.match("...") && (!source.more() || source.isWS() ||
                                      source.current() == kLineFeed)) {
            throw SyntaxError(source.getPosition(),
                              "Document end marker inside single-quoted "
                              "string.");
          }
        }
      }
      if (source.current() == quote) {
        source.next();
        if (source.current() == quote) {
          yamlString += source.append();
        } else {
          closedQuote = true;
          break;
        }
      } else {
        appendCharacterToString(source, yamlString);
      }
    }
  }
  if (!closedQuote) {
    throw SyntaxError(source.getPosition(), "Missing closing quote.");
  }
  bool sawTrailingWhitespace = false;
  while (source.more() &&
         (source.current() == kSpace || source.current() == '\t')) {
    sawTrailingWhitespace = true;
    source.next();
  }
  // YAML 1.2 §6.6: comments must be separated from scalars by whitespace.
  if (source.more() && source.current() == '#') {
    if (!sawTrailingWhitespace) {
      throw SyntaxError(source.getPosition(),
                        "Comment must be preceded by whitespace.");
    }
  } else if (source.more() && source.current() != kLineFeed &&
             !delimiters.contains(source.current())) {
    throw SyntaxError(source.getPosition(),
                      "Invalid trailing content after quoted scalar.");
  }
  moveToNext(source, delimiters);
  return Node::make<String>(yamlString, quote);
}

} // namespace YAML_Lib