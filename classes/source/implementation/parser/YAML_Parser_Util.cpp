
//
// Class: YAML_Parser_util
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Returns true if str ends with substr.
/// </summary>
/// <param name="str">Target string.</param>
/// <param name="substr">Ends with string.</param>
/// <returns>If true, str then ends with substr.</returns>
bool Default_Parser::endsWith(const std::string_view &str,
                              const std::string_view &substr) {
  const auto strLen = str.size();
  const auto substrLen = substr.size();
  if (strLen < substrLen)
    return false;

  return str.compare(strLen - substrLen, substrLen, substr) == 0;
}
/// <summary>
/// Remove any spaces at the end of str.
/// </summary>
/// <param name="str">Target string.</param>
void Default_Parser::rightTrim(std::string &str) {
  str.erase(
      std::find_if(str.rbegin(), str.rend(),
                   [](const unsigned char ch) { return !std::isspace(ch); })
          .base(),
      str.end());
}
/// <summary>
/// Move to the next delimiter on source stream in a set.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Set of possible delimiter characters.</param>
void Default_Parser::moveToNext(ISource &source, const Delimiters &delimiters) {
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      source.next();
    }
  }
}
/// <summary>
/// If the current character is the start of a comment ('#'), skip the rest
/// of the line and return true. Otherwise return false.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>True if a comment line was skipped.</returns>
bool Default_Parser::skipIfComment(ISource &source) {
  if (isComment(source)) {
    skipLine(source);
    return true;
  }
  return false;
}
/// <summary>
/// Move to the end of the current line and consume the newline character.
/// Equivalent to: moveToNext(source, {kLineFeed}); if (source.more())
/// source.next();
/// </summary>
/// <param name="source">Source stream.</param>
void Default_Parser::skipLine(ISource &source) {
  moveToNext(source, {kLineFeed});
  if (source.more()) {
    source.next();
  }
}
/// <summary>
/// Move to the next non-whitespace character in source stream; jumping over new
/// lines and stripping amy comments. Tabs used as block indentation (at the
/// start of a line) are rejected as per the YAML 1.2 specification.
/// </summary>
/// <param name="source">Source stream.</param>
void Default_Parser::moveToNextIndent(ISource &source) {
  bool indentFound{false};
  while (!indentFound) {
    bool afterNewline = (source.getPosition().second == 1);
    bool seenSpaceOnLine = false;
    while (source.more() && (source.isWS() || source.current() == kLineFeed)) {
      if (source.current() == kLineFeed) {
        afterNewline = true;
        seenSpaceOnLine = false;
      } else if (afterNewline && source.current() == '\t' && !seenSpaceOnLine) {
        // A leading tab (before any space) on this line. Reject it when it is
        // used as block indentation before block-style content, but allow it
        // for blank lines and top-level flow-style lines such as "\t[" / "\t]"
        // (yaml-test-suite 6CA3).
        const auto firstNonWsOnLine = [&]() -> char {
          SourceGuard guard(source);
          source.next(); // look past the first tab
          while (source.more() && source.current() != kLineFeed &&
                 source.isWS()) {
            source.next();
          }
          if (!source.more() || source.current() == kLineFeed) {
            return '\0';
          }
          return source.current();
        }();
        const bool blankLine = (firstNonWsOnLine == '\0');
        const bool flowLine = firstNonWsOnLine == kLeftSquareBracket ||
                              firstNonWsOnLine == kRightSquareBracket ||
                              firstNonWsOnLine == kLeftCurlyBrace ||
                              firstNonWsOnLine == kRightCurlyBrace ||
                              firstNonWsOnLine == kComma;
        if (!blankLine && !flowLine) {
          throw SyntaxError(
              source.getPosition(),
              "Tab character not allowed in YAML block indentation.");
        }
        // Blank/flow line — fall through; source.next() below consumes the tab.
      } else if (source.current() == kSpace) {
        seenSpaceOnLine = true;
      }
      source.next();
    }
    if (!skipIfComment(source)) {
      indentFound = true;
    }
  }
}
/// <summary>
/// Extract quoted from source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Extracted characters (uses source.current() as the quote
/// character).</returns>
std::string Default_Parser::extractString(ISource &source) {
  return extractString(source, source.current());
}
std::string Default_Parser::extractString(ISource &source, const char quote) {
  return extractString(source, quote, nullptr);
}
std::string Default_Parser::extractString(ISource &source, const char quote,
                                          unsigned long *quoteColumn) {
  if (quoteColumn) {
    *quoteColumn = source.getPosition().second;
  }
  std::string extracted{quote};
  source.next(); // skip opening quote
  bool foundClosing = false;
  while (source.more()) {
    if (source.current() == quote) {
      if (quote == kApostrophe) {
        // Advance past this quote and decide: '' or real closing quote?
        source.next();
        if (source.more() && source.current() == quote) {
          // '' escape: keep both raw chars so the downstream re-parser
          // (convertYAMLToStringNode → parseQuotedFlowString) decodes them.
          extracted += quote;
          extracted += quote;
          source.next(); // consume the second quote; continue scanning
          continue;
        }
        // Real closing quote: already consumed above.
        extracted += quote;
        source.ignoreWS();
        return extracted;
      }
      foundClosing = true;
      break; // double-quoted (or other): closing quote
    }
    extracted += source.current();
    source.next();
  }
  if (!foundClosing) {
    throw SyntaxError(source.getPosition(),
                      "Unterminated quoted string: missing closing quote");
  }
  extracted += quote;
  if (source.more()) {
    source.next(); // consume closing quote
  }
  source.ignoreWS();
  return extracted;
}
/// <summary>
/// Extract a quoted scalar from source, stripping the surrounding quote
/// characters and trimming trailing whitespace.  The source must be
/// positioned at the opening quote character.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Unquoted, right-trimmed scalar value.</returns>
std::string Default_Parser::extractRawQuotedScalar(ISource &source) {
  std::string raw = extractString(source);
  if (raw.size() >= 2) {
    raw = raw.substr(1, raw.size() - 2);
  }
  rightTrim(raw);
  return raw;
}
/// <summary>
/// Extract characters from source stream up to a delimiter.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters"></param>
/// <returns>Extracted characters.</returns>
/// <summary>
/// Extract the tag suffix from source, using a flow-context-aware delimiter
/// set.  Inside a flow collection ([] or {}) the stop characters include ',',
/// ']' and '}'; in block context only space and linefeed stop extraction.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Extracted tag suffix string.</returns>
std::string Default_Parser::extractTagSuffix(ISource &source) {
  if (isInsideFlowContext()) {
    return extractToNext(source, {kSpace, kLineFeed, ',', ']', '}'});
  }
  return extractToNext(source, {kSpace, kLineFeed});
}
std::string Default_Parser::extractToNext(ISource &source,
                                          const Delimiters &delimiters) {
  std::string extracted;
  if (!delimiters.empty()) {
    while (source.more() && !delimiters.contains(source.current())) {
      extracted += source.append();
    }
  }
  return extracted;
}
/// <summary>
/// Extract the next token up to a delimiter and right-trim whitespace.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiter set.</param>
/// <returns>Extracted and right-trimmed string.</returns>
std::string Default_Parser::extractTrimmed(ISource &source,
                                           const Delimiters &delimiters) {
  std::string s{extractToNext(source, delimiters)};
  rightTrim(s);
  return s;
}
/// <summary>
/// Extract characters from source stream up to a last end character.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="start">Start character.</param>
/// <param name="end">End character.</param>
/// <returns>Extracted characters.</returns>
std::string Default_Parser::extractInLine(ISource &source, const char start,
                                          const char end) {
  std::string extracted;
  unsigned long depth{1};
  extracted += start;
  source.next();
  while (depth > 0 && source.more()) {
    if (source.current() == start) {
      depth++;
    } else if (source.current() == end) {
      depth--;
    }
    extracted += source.current();
    source.next();
  }
  source.ignoreWS();
  return extracted;
}
/// <summary>
/// Check that end character has been found if not throw an exception.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="end">End character.</param>
void Default_Parser::checkForEnd(ISource &source, const char end) {
  if (source.current() != end) {
    throw SyntaxError(source.getPosition(),
                      std::string("Missing closing ") + end + ".");
  }
  source.next();
}
/// <summary>
/// Construct a BufferSource from text and parse it as a document.
/// </summary>
/// <param name="text">YAML text to parse.</param>
/// <param name="delimiters">Delimiters used to parse the document.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Parsed Node.</returns>
Node Default_Parser::parseFromBuffer(const std::string &text,
                                     const Delimiters &delimiters,
                                     const unsigned long indentation) {
  BufferSource src{text}; // string_view into text — no copy; text outlives src
  return parseDocument(src, delimiters, indentation);
}
/// <summary>
/// Collect all lines whose column is >= minIndent into a single string,
/// preserving synthetic indentation and separating lines with '\n'.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="minIndent">Minimum column for inclusion.</param>
/// <returns>Captured block text.</returns>
std::string
Default_Parser::captureIndentedBlock(ISource &source,
                                     const unsigned long minIndent) {
  std::string text;
  while (source.more() && source.getPosition().second >= minIndent) {
    if (isDocumentBoundary(source)) {
      break;
    }
    const std::string indent(source.getPosition().second, kSpace);
    text += indent + extractToNext(source, {kLineFeed}) + "\n";
    moveToNextIndent(source);
  }
  return text;
}
/// <summary>
/// Insert or overwrite a key in a Dictionary.
/// If the key already exists its value is replaced; otherwise a new entry
/// is appended.
/// </summary>
/// <param name="dict">Target dictionary.</param>
/// <param name="key">Key string.</param>
/// <param name="value">Value node (moved in).</param>
void Default_Parser::upsertDictEntry(Dictionary &dict, const std::string &key,
                                     Node value) {
  if (dict.contains(key)) {
    dict[key] = std::move(value);
  } else {
    dict.add(DictionaryEntry(key, std::move(value)));
  }
}
/// <summary>
/// After closing an outermost inline collection, verify that the next
/// character belongs to the caller's delimiter set; throw SyntaxError if not.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Expected delimiter set.</param>
void Default_Parser::checkFlowDelimiter(ISource &source,
                                        const Delimiters &delimiters) {
  if (source.more() && !delimiters.contains(source.current())) {
    throw SyntaxError("Unexpected flow sequence token '" +
                      std::string(1, source.current()) + "'.");
  }
}
/// <summary>
/// After closing an inline collection, skip whitespace and — when this was
/// the outermost inline collection (depth == 0) — verify the next character
/// belongs to the caller's delimiter set.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Expected delimiter set.</param>
/// <param name="depth">Post-decrement inline-collection depth.</param>
void Default_Parser::checkAtFlowClose(ISource &source,
                                      const Delimiters &delimiters,
                                      const long depth) {
  bool separatedFromClose = false;
  while (source.more() && source.isWS()) {
    separatedFromClose = true;
    source.next();
  }
  if (source.more() && source.current() == '#' && !separatedFromClose) {
    throw SyntaxError(
        source.getPosition(),
        "Comment after flow collection close requires separation whitespace.");
  }
  if (depth == 0) {
    checkFlowDelimiter(source, delimiters);
  }
}
/// <summary>
/// Return a copy of base with extra delimiter characters added.
/// </summary>
/// <param name="base">Base delimiter set to copy.</param>
/// <param name="extras">Additional characters to include.</param>
/// <returns>New Delimiters set.</returns>
Default_Parser::Delimiters
Default_Parser::withExtras(const Delimiters &base,
                           std::initializer_list<char> extras) {
  Delimiters result{base};
  result.insert(extras);
  return result;
}
/// <summary>
/// Return the delimiter set that terminates plain-scalar key extraction,
/// taking the current flow context into account.
/// In block context only ':' and '\n' stop extraction; inside a flow
/// collection '{', '}', and ',' are also stop characters.
/// </summary>
/// <returns>Appropriate Delimiters set for the current parser depth.</returns>
Default_Parser::Delimiters Default_Parser::keyStopDelimiters() {
  return inlineDictionaryDepth > 0
             ? Delimiters{kColon, kComma, kRightCurlyBrace, kLineFeed}
             : Delimiters{kColon, kLineFeed};
}
/// <summary>
/// Is the parser currently inside at least one flow collection ([] or {})?
/// </summary>
/// <returns>True when inlineArrayDepth or inlineDictionaryDepth is
/// non-zero.</returns>
bool Default_Parser::isInsideFlowContext() noexcept {
  return inlineArrayDepth > 0 || inlineDictionaryDepth > 0;
}

} // namespace YAML_Lib