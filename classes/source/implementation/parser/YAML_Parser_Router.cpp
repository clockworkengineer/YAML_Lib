
//
// Class: YAML_Parser_Router
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Is YAML overrides on source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true value is an override.</returns>
bool Default_Parser::isOverride(ISource &source) {
  return matchesMarker(source, "<<:");
}
// <summary>
// Has a dictionary key been found in the source stream?
// </summary>
// <param name="source">Source stream.</param>
// <returns>== true if a dictionary key has been found.</returns>
bool Default_Parser::isKey(ISource &source) {
  SourceGuard guard(source);
  bool keyPresent{false};
  if (std::string key{extractKey(source)};
      source.current() == kColon || (!key.empty() && key.back() == kColon)) {
    if (key[0] == kLeftCurlyBrace || key[0] == kLeftSquareBracket) {
      if (key.find('\n') != std::string::npos) {
        if (key[0] == kLeftCurlyBrace) {
          throw SyntaxError(
              source.getPosition(),
              "Inline dictionary used as key is meant to be on one line.");
        }
        throw SyntaxError(
            source.getPosition(),
            "Inline array used as key is meant to be on one line.");
      }
    }
    if (source.more()) {
      source.next();
    }
    // YAML 1.2 §6.1/§6.3: a tab after ':' in block context is only a valid
    // separation space (§6.3) when at least one space immediately follows the
    // tab(s). A bare tab with no following space structurally determines the
    // block-value indentation → reject it (e.g. Y79Y/7: ":" + TAB + "-").
    // A tab followed by a space is a valid s-white separation sequence
    // (e.g. 6BCT: "foo:" + TAB + " bar").
    if (inlineDictionaryDepth == 0 && source.more() &&
        source.current() == '\t') {
      SourceGuard tabGuard(source);
      while (source.more() && source.current() == '\t') {
        source.next();
      }
      if (!source.more() || source.current() != kSpace) {
        throw SyntaxError(
            source.getPosition(),
            "Tab used as block value-separator after ':'; block indentation "
            "must use spaces, not tabs (YAML 1.2 \xc2\xa7"
            "6.1).");
      }
      tabGuard.release(); // space follows — consume the tab(s) and continue
    }
    if (source.current() == ' ' || source.current() == kLineFeed ||
        (!key.empty() && key.back() == kColon)) {
      if (!key.empty() && key.back() == kColon) {
        key.pop_back();
      }
      rightTrim(key);
      keyPresent = isValidKey(key);
    }
  }
  return keyPresent;
}
/// <summary>
/// Has an array element been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, an array element has been found.</returns>
bool Default_Parser::isArray(ISource &source) {
  SourceGuard guard(source);
  auto ch = source.current();
  auto arrayPresent{false};
  if (source.more() && ch == '-') {
    source.next();
    ch = source.current();
    // YAML 1.2: after '-' indicator, space, tab, or newline all satisfy
    // the '¬ns-char' lookahead (none is a printable non-space character).
    arrayPresent = ch == kSpace || ch == kLineFeed || ch == '\t';
  }
  return arrayPresent;
}
/// <summary>
/// Has a possible boolean value been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a boolean value has been found.</returns>
bool Default_Parser::isBoolean(const ISource &source) {
  const auto ch = source.current();
  if (strictBooleans || yamlDirectiveMinor >= 2) {
    // YAML 1.2 strict: only 'true' and 'false'
    return ch == 't' || ch == 'f';
  }
  // YAML 1.1 permissive: True/False/On/Off/Yes/No (all cases)
  return ch == 'T' || ch == 'F' || ch == 'O' || ch == 'Y' || ch == 'N' ||
         ch == 't' || ch == 'f' || ch == 'y' || ch == 'n' || ch == 'o';
}
/// <summary>
/// Has a quoted string been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, then a quoted string has been found.</returns>
bool Default_Parser::isQuotedString(const ISource &source) {
  const auto ch = source.current();
  return ch == kApostrophe || ch == kDoubleQuote;
}
/// <summary>
/// Has a possible number been found in the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, then a number has been found.</returns>
bool Default_Parser::isNumber(const ISource &source) {
  const auto ch = source.current();
  // Include '.' to catch YAML 1.2 special floats: .inf, .nan
  return (ch >= '0' && ch <= '9') || ch == '-' || ch == '+' || ch == '.';
}
/// <summary>
/// Has a possible null value been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a null (none) value has been found.</returns>
bool Default_Parser::isNone(const ISource &source) {
  const auto second = source.current();
  return second == 'n' || second == '~';
}
/// <summary>
/// Has a folded block string been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a founded block string has been found.</returns>
bool Default_Parser::isFoldedBlockString(const ISource &source) {
  return source.current() == '>';
}
/// <summary>
/// Has a piped block string been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a piped block string has been found.</returns>
bool Default_Parser::isPipedBlockString(const ISource &source) {
  return source.current() == '|';
}
/// <summary>
/// Has a comment been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a comment has been found.</returns>
bool Default_Parser::isComment(const ISource &source) {
  return source.current() == '#';
}
/// <summary>
/// Has an anchor been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, an anchor has been found.</returns>
bool Default_Parser::isAnchor(const ISource &source) {
  return source.current() == '&';
}
/// <summary>
/// Has an alias been found on the input stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, an alias has been found.</returns>
bool Default_Parser::isAlias(const ISource &source) {
  return source.current() == '*';
}
/// <summary>
/// Has an inline array been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, an inline array has been found.</returns>
bool Default_Parser::isInlineArray(const ISource &source) {
  return source.current() == kLeftSquareBracket;
}
/// <summary>
/// Has an inline dictionary been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, an inline dictionary has been found.</returns>
bool Default_Parser::isInlineDictionary(const ISource &source) {
  return source.current() == kLeftCurlyBrace;
}
/// <summary>
/// Has an inline collection (array or dictionary) been found on the source
/// stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, an inline array or dictionary has been found.</returns>
bool Default_Parser::isInlineCollection(const ISource &source) {
  return isInlineDictionary(source) || isInlineArray(source);
}
/// <summary>
/// Has a mapping been found on the source stream?
/// The explicit mapping key indicator '?' requires a space or linefeed
/// immediately after it.  '?foo' (no whitespace) is a plain scalar, not an
/// explicit key.  '?\t' (tab immediately after '?') is also invalid per
/// YAML 1.2 §6.1 — block structure separators must use spaces, not tabs.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a mapping has been found.</returns>
bool Default_Parser::isMapping(ISource &source) {
  if (source.current() != '?') {
    return false;
  }
  SourceGuard guard(source);
  source.next(); // peek at char after '?'
  // YAML 1.2 §6.1: block indentation must use spaces, not tabs.
  // A tab immediately after '?' uses a tab as the block-structure separator.
  if (source.more() && source.current() == '\t') {
    throw SyntaxError(
        source.getPosition(),
        "Tab used as block structure separator after '?' explicit mapping key "
        "indicator; block indentation must use spaces, not tabs "
        "(YAML 1.2 \xc2\xa7"
        "6.1).");
  }
  return !source.more() || source.current() == kSpace ||
         source.current() == kLineFeed;
}
/// <summary>
/// Has a dictionary been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a dictionary has been found.</returns>
bool Default_Parser::isDictionary(ISource &source) {
  // If the source starts with '*', extract the alias name using alias-name
  // rules (colon is valid in an anchor/alias name, it is NOT a delimiter).
  // When the extracted name ends with ':' and matches a known anchor, the
  // token is an alias reference where the colon is part of the name — not a
  // dict key followed by a ':' separator.
  // Example: "*a:\n" where anchor "a:" was defined → alias, not dict key.
  // Counter-example: "*alias1 : val\n" → name stops at space ("alias1"),
  // no trailing colon → falls through to isKey → treated as dict key.
  if (source.current() == '*') {
    SourceGuard guard(source);
    source.next();
    const Delimiters aliasStop{kLineFeed, kSpace, kComma, kRightSquareBracket,
                               kRightCurlyBrace};
    const std::string aliasName = extractToNext(source, aliasStop);
    if (!aliasName.empty() && aliasName.back() == kColon &&
        yamlAliasMap.count(aliasName)) {
      return false;
    }
  }
  return isKey(source);
}
/// <summary>
/// Has document start been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true,a start of document has been found.</returns>
bool Default_Parser::matchesMarker(ISource &source, const char *marker) {
  SourceGuard guard(source);
  return source.match(marker);
}
/// <summary>
/// Has document start been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true,a start of document has been found.</returns>
bool Default_Parser::isDocumentStart(ISource &source) {
  return matchesMarker(source, kStartDocument);
}
/// <summary>
/// Has the document end been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, an end document has been found.</returns>
bool Default_Parser::isDocumentEnd(ISource &source) {
  return matchesMarker(source, kEndDocument);
}
bool Default_Parser::isDocumentBoundary(ISource &source) {
  return isDocumentStart(source) || isDocumentEnd(source);
}
/// <summary>
/// Last parser router table entry so return true.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>True</returns>
bool Default_Parser::isDefault([[maybe_unused]] ISource &source) {
  return true;
}

/// <summary>
/// Has a YAML directive (%YAML or %TAG) been found on the source stream?
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>If true, a directive line has been found.</returns>
bool Default_Parser::isDirective(ISource &source) {
  return source.current() == '%';
}

} // namespace YAML_Lib