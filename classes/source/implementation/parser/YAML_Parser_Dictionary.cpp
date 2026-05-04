
//
// Class: YAML_Parser_Dictionary
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Add a DictionaryEntry to a dictionary Node, throwing if the key already
/// exists.
/// </summary>
/// <param name="dictionaryNode">Target dictionary Node.</param>
/// <param name="entry">Entry to add.</param>
/// <param name="source">Source stream (used for error position).</param>
void Default_Parser::addUniqueDictEntry(Node &dictionaryNode,
                                        DictionaryEntry entry,
                                        ISource &source) {
  if (NRef<Dictionary>(dictionaryNode).contains(entry.getKey())) {
    if (entry.getKey().empty()) {
      // Empty (null) key: YAML permits duplicate null keys; last-wins.
      NRef<Dictionary>(dictionaryNode)[entry.getKey()] =
          std::move(entry.getNode());
      return;
    }
    throw SyntaxError(source.getPosition(),
                      "Dictionary already contains key '" +
                          std::string(entry.getKey()) + "'.");
  }
  NRef<Dictionary>(dictionaryNode).add(std::move(entry));
}
/// <summary>
/// Add a DictionaryEntry to an inline dictionary, applying complex-key
/// (collection-key) last-wins semantics for duplicate '[...]' or '{...}' keys
/// while still throwing on duplicate plain-scalar keys.
/// </summary>
/// <param name="dict">Target Dictionary (unwrapped).</param>
/// <param name="entry">Entry to add.</param>
/// <param name="source">Source stream (used for error position).</param>
void Default_Parser::addInlineDictEntry(Dictionary &dict, DictionaryEntry entry,
                                        ISource &source) {
  const std::string keyStr{entry.getKey()};
  const bool isComplexKey =
      !keyStr.empty() && (keyStr.front() == kLeftSquareBracket ||
                          keyStr.front() == kLeftCurlyBrace);
  if (dict.contains(keyStr) && !isComplexKey) {
    throw SyntaxError(source.getPosition(),
                      "Dictionary already contains key '" + keyStr + "'.");
  } else if (dict.contains(keyStr)) {
    dict[keyStr] = std::move(entry.getNode());
  } else {
    dict.add(std::move(entry));
  }
}
/// <summary>
/// Extract a balanced inline collection ('{...}' or '[...]') starting at the
/// current source position. Precondition: isInlineCollection(source).
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>The raw collection text including its brackets.</returns>
std::string Default_Parser::extractInlineCollectionAt(ISource &source) {
  const char start = source.current();
  const char end =
      (start == kLeftCurlyBrace) ? kRightCurlyBrace : kRightSquareBracket;
  return extractInLine(source, start, end);
}
/// <summary>
/// Convert YAML key to a string Node
/// </summary>
/// <param name="yamlString">YAML string.</param>
Node Default_Parser::convertYAMLToStringNode(
    const std::string_view &yamlString) {
  return convertYAMLToStringNode(yamlString, 0);
}
Node Default_Parser::convertYAMLToStringNode(const std::string_view &yamlString,
                                             unsigned long indentation) {
  auto keyNode = parseFromBuffer(std::string(yamlString) + kLineFeed,
                                 {kLineFeed}, indentation);
  std::string keyString{keyNode.toKey()};
  char quote = '\"';
  if (isA<String>(keyNode)) {
    quote = NRef<String>(keyNode).getQuote();
    if (keyString.empty()) {
      quote = kDoubleQuote;
    }
  }
  return Node::make<String>(keyString, quote);
}
/// <summary>
/// Does the string constitute a valid dictionary key?
/// Empty / whitespace-only strings map to YAML null keys (valid).
/// A key whose first non-whitespace character is '#' is a comment (invalid).
/// Plain scalar keys are accepted directly (fast path, no allocation).
/// Quoted-string extractions are re-validated via parseFromBuffer because
/// extractString() does not process escape sequences: "foo\": bar" is
/// extracted raw as "foo\" (stopping at the wrong "), and only the
/// mini-parse can tell that the result is not a well-formed YAML value.
/// </summary>
/// <param name="key">Candidate key string.</param>
/// <returns>True if key is valid.</returns>
bool Default_Parser::isValidKey(const std::string_view &key) noexcept {
  const auto first = key.find_first_not_of(" \t");
  if (first == std::string_view::npos) return true; // null/empty key
  if (key[first] == '#') return false;              // comment
  if (key[first] == kDoubleQuote || key[first] == kApostrophe) {
    // Slow path: re-parse to catch truncated quoted-string extractions.
    try {
      const Node keyNode =
          parseFromBuffer(std::string(key) + kLineFeed, {kLineFeed}, 0);
      return !keyNode.isEmpty() && !isA<Comment>(keyNode);
    } catch ([[maybe_unused]] const std::exception &e) {
      return false;
    }
  }
  return true; // fast path: plain scalar, anchor, tag, inline collection, etc.
}
/// <summary>
/// Extract mapping on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Extracted mapping/.</returns>
std::string Default_Parser::extractMapping(ISource &source) {
  std::string key;
  // Save the column of the '?' indicator so we know how deep the block key
  // content must be indented when capturing continuation lines.
  const auto questionCol = source.getPosition().second;
  source.next(); // consume leading '?' (mapping indicator)
  while (true) {
    key.clear();
    while (source.more() && source.current() == kSpace) {
      key += kSpace;
      source.next();
    }
    if (!skipIfComment(source)) {
      break;
    }
  }
  // Explicit mapping key with node content on following lines, e.g.:
  // ?\n- a\n- b\n:\n- c\n- d
  // Capture lines up to a ':' at the same column as the '?' indicator.
  if (inlineDictionaryDepth == 0 && source.current() == kLineFeed) {
    source.next(); // first line of key node content (or ':' line)
    std::string multilineKey;
    while (source.more()) {
      if (source.getPosition().second == questionCol &&
          source.current() == kColon) {
        key += multilineKey;
        key += kColon;
        return key;
      }
      multilineKey += extractToNext(source, {kLineFeed});
      if (source.more() && source.current() == kLineFeed) {
        multilineKey += kLineFeed;
        source.next();
      } else {
        break;
      }
    }
    key += multilineKey;
    key += kColon;
    return key;
  }
  if (isInlineCollection(source)) {
    key += extractInlineCollectionAt(source);
    moveToNext(source, {kColon, kLineFeed});
    key += kColon;
  } else if (isArray(source)) {
    key += extractToNext(source, {kLineFeed});
    if (source.more() && source.current() == kLineFeed) {
      std::string multilineKey;
      multilineKey += kLineFeed;
      source.next();
      while (source.more()) {
        if (source.getPosition().second == questionCol &&
            source.current() == kColon) {
          key += multilineKey;
          key += kColon;
          return key;
        }
        if (source.getPosition().second < questionCol) {
          break;
        }
        multilineKey += extractToNext(source, {kLineFeed});
        if (source.more() && source.current() == kLineFeed) {
          multilineKey += kLineFeed;
          source.next();
        } else {
          break;
        }
      }
      key += multilineKey;
    }
    key += kColon;
    return key;
  } else if (isPipedBlockString(source) || isFoldedBlockString(source)) {
    // Block scalar as explicit mapping key (? | or ? >). Capture the
    // indicator line plus all continuation lines that are more indented
    // than the '?' column.  This allows "? |\n  content\n: value" to be
    // correctly parsed: the content lines become the key text rather than
    // being mis-parsed as the value of a bare-'|' key.
    key += extractToNext(source, {kLineFeed}); // reads '|...' or '>...'
    if (source.more() && source.current() == kLineFeed) {
      key += kLineFeed;
      source.next(); // consume LF after indicator
      // Capture continuation lines whose leading-space count is at least
      // questionCol (i.e. they are more indented than the '?' indicator).
      // IMPORTANT: SourceGuard is used only for peek, and release() is never
      // called: the guard always fires normally (restoring), then the line is
      // re-read without a guard.  This prevents orphaned context entries on
      // the save/restore stack that would corrupt outer SourceGuards.
      while (source.more()) {
        // Peek: check line indentation using a guard that always restores.
        bool lineIsContent = false;
        unsigned long spaces = 0;
        {
          SourceGuard peekGuard(source);
          while (source.more() && source.current() == kSpace) {
            spaces++;
            source.next();
          }
          const bool isBlankLine =
              !source.more() || source.current() == kLineFeed;
          lineIsContent = !isBlankLine && spaces >= questionCol;
        } // peekGuard always restores here — no release() used
        if (!lineIsContent) {
          break;
        }
        // Line is sufficiently indented: re-read and append to key.
        std::string lineContent;
        while (source.more() && source.current() == kSpace) {
          lineContent += source.current();
          source.next();
        }
        key += lineContent;
        key += extractToNext(source, {kLineFeed});
        key += kLineFeed;
        if (source.more() && source.current() == kLineFeed) {
          source.next(); // consume LF
        }
      }
    }
    key += kColon;
  } else {
    std::string text = extractToNext(source, {kLineFeed});
    std::string multilineKey;
    bool multiline = false;
    if (source.more() && source.current() == kLineFeed) {
      source.next();
      while (source.more()) {
        if (source.getPosition().second == questionCol &&
            source.current() == kColon) {
          // Found the explicit key terminator. Preserve the full key text.
          // Example: '? a\n  true\n: value' -> 'a\n  true'.
          // 'text' holds the first line, 'multilineKey' holds subsequent lines.
          key += text;
          key += kLineFeed;
          key += multilineKey;
          key += kColon;
          return key;
        }
        const bool lineTooShallow = [&]() {
          SourceGuard guard(source);
          while (source.more() &&
                 (source.current() == kSpace || source.current() == '\t')) {
            source.next();
          }
          return source.getPosition().second <= questionCol;
        }();
        if (lineTooShallow) {
          break;
        }
        std::size_t separatorPos = std::string::npos;
        {
          SourceGuard lineGuard(source);
          const std::string line = extractToNext(source, {kLineFeed});
          for (std::size_t pos = 0; pos < line.size(); ++pos) {
            if (line[pos] != kColon) {
              continue;
            }
            const char nextChar = (pos + 1 < line.size() ? line[pos + 1] : '\0');
            if (nextChar == kSpace || nextChar == '\t' || nextChar == '#' ||
                nextChar == '\0') {
              separatorPos = pos;
              break;
            }
          }
        }
        if (separatorPos != std::string::npos) {
          std::string linePrefix;
          for (std::size_t i = 0; i < separatorPos; ++i) {
            linePrefix += source.append();
          }
          key += text;
          key += kLineFeed;
          key += multilineKey;
          key += linePrefix;
          key += kColon;
          return key;
        }
        multilineKey += extractToNext(source, {kLineFeed});
        if (source.more() && source.current() == kLineFeed) {
          multilineKey += kLineFeed;
          source.next();
        } else {
          break;
        }
      }
      multiline = true;
    }
    // Strip inline comment: in YAML, '#' preceded by whitespace is a comment.
    for (std::size_t i = 1; i < text.size(); ++i) {
      if (text[i] == '#' && (text[i - 1] == ' ' || text[i - 1] == '\t')) {
        text.erase(i - 1); // erase from the whitespace before '#'
        break;
      }
    }
    key += text;
    if (multiline) {
      key += kLineFeed;
      key += multilineKey;
    }
    key += kColon;
  }
  return key;
}
/// <summary>
/// Extract YAML used in possible key value from source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>YAML for key value.</returns>
std::string Default_Parser::extractKey(ISource &source,
                                       unsigned long *quoteIndent) {
  const auto extractPlainKeyTail = [&source]() {
    const Delimiters plainKeyDelimiters = keyStopDelimiters();
    const Delimiters delimitersWithComment = withExtras(plainKeyDelimiters, {'#'});
    std::string keyTail;
    while (source.more()) {
      keyTail += extractToNext(source, delimitersWithComment);
      if (!source.more())
        break;
      if (source.current() == '#') {
        if (!keyTail.empty() &&
            (keyTail.back() == kSpace || keyTail.back() == '\t')) {
          break; // comment begins here; do not include the '#' or the rest
        }
        keyTail += source.append();
        continue;
      }
      if (source.current() != kColon)
        break;
      const bool isSeparator = [&source]() -> bool {
        SourceGuard guard(source);
        source.next(); // peek past ':'
        if (!source.more() || source.current() == kSpace ||
            source.current() == kLineFeed || source.current() == '#') {
          return true;
        }
        if (source.current() == '\t') {
          while (source.more() && source.current() == '\t') {
            source.next();
          }
          return !source.more() || source.current() == kSpace ||
                 source.current() == kLineFeed || source.current() == '#'
                 || source.current() == '|' || source.current() == '>';
        }
        return false;
      }();
      if (isSeparator)
        break; // ':' is the key-value separator, stop here
      keyTail += kColon;
      source.next(); // consume ':', it is part of the key
    }
    return keyTail;
  };

  // Handle "&anchor value" prefix (e.g. "&a [...]" or "&a scalar" as a key).
  // In flow context, a plain-scalar extraction would stop at the first ','
  // inside the inline collection; the anchor branch handles it bracket-aware
  // instead.
  if (source.current() == '&') {
    std::string result{"&"};
    source.next();
    result += extractToNext(source, {kSpace, '\t', kLineFeed});
    result += ' ';
    source.ignoreWS();
    if (isInlineCollection(source)) {
      result += extractInlineCollectionAt(source);
    } else if (isQuotedString(source)) {
      result += extractString(source);
    } else {
      result += extractPlainKeyTail();
      rightTrim(result);
    }
    return result;
  }
  // Handle tagged keys (e.g. "!!str key: value" or
  // "!<tag:yaml.org,2002:str> foo : ..."). The tag token may contain ':'
  // characters, so plain-scalar key extraction cannot start at the tag.
  if (source.current() == '!') {
    std::string result{"!"};
    source.next();
    if (!source.more()) {
      return result;
    }
    if (source.current() == '<') {
      result += source.append();
      result += extractToNext(source, {'>'});
      if (!source.more() || source.current() != '>') {
        return result;
      }
      result += source.append();
    } else if (source.current() == '!') {
      result += source.append();
      result += extractTagSuffix(source);
    } else {
      result += extractToNext(source, {'!', kSpace, '\t', kLineFeed});
      if (source.more() && source.current() == '!') {
        result += source.append();
        result += extractTagSuffix(source);
      }
    }
    if (source.more() &&
        (source.current() == kSpace || source.current() == '\t')) {
      result += ' ';
      source.ignoreWS();
      if (source.more() && source.current() == '&') {
        result += '&';
        source.next();
        result += extractToNext(source, {kSpace, '\t', kLineFeed});
        if (source.more() &&
            (source.current() == kSpace || source.current() == '\t')) {
          result += ' ';
          source.ignoreWS();
        }
        if (isInlineCollection(source)) {
          result += extractInlineCollectionAt(source);
        } else if (isQuotedString(source)) {
          if (quoteIndent) {
            *quoteIndent = source.getPosition().second;
          }
          result += extractString(source, source.current(), quoteIndent);
        } else {
          result += extractPlainKeyTail();
          rightTrim(result);
        }
      } else if (isInlineCollection(source)) {
        result += extractInlineCollectionAt(source);
      } else if (isQuotedString(source)) {
        if (quoteIndent) {
          *quoteIndent = source.getPosition().second;
        }
        result += extractString(source, source.current(), quoteIndent);
      } else {
        result += extractPlainKeyTail();
        rightTrim(result);
      }
    }
    return result;
  }
  if (isInlineCollection(source)) {
    return extractInlineCollectionAt(source);
  }
  if (isQuotedString(source)) {
    if (quoteIndent) {
      *quoteIndent = source.getPosition().second;
    }
    return extractString(source, source.current(), quoteIndent);
  }
  if (isMapping(source)) {
    return extractMapping(source);
  }
  // Plain scalar key: ':' is only a value separator when followed by
  // space/newline/EOF. Otherwise (e.g. ":foo") the colon is part of the key
  // text.
  // In block context (inlineDictionaryDepth == 0) '}' and ',' are ordinary
  // characters and must not terminate key extraction.  They are only special
  // inside flow collections (inlineDictionaryDepth > 0).
  return extractPlainKeyTail();
}
/// <summary>
/// Parse dictionary key on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Dictionary entry key.</returns>
Node Default_Parser::parseKey(ISource &source) {
  unsigned long keyQuoteIndent = 0;
  std::string key{extractKey(source, &keyQuoteIndent)};
  // Patch: In flow context, allow multi-line explicit keys (e.g., '? foo\n bar
  // : baz')
  if (isInsideFlowContext()) {
    // Accumulate lines until ':' is found at the correct indentation or a flow
    // delimiter
    while (source.more() && source.current() == kLineFeed) {
      // Peek ahead to see if next line is indented (continuation of key)
      SourceGuard guard(source);
      source.next();
      unsigned long spaces = 0;
      while (source.more() && source.current() == ' ') {
        ++spaces;
        source.next();
      }
      // If next non-space is ':' or a flow delimiter, stop
      if (source.more() &&
          (source.current() == ':' || source.current() == ',' ||
           source.current() == ']' || source.current() == '}')) {
        break;
      }
      // Otherwise, treat as continuation of key
      guard.release();
      key += ' ';
      key += extractToNext(source, {':', ',', ']', '}', '\n'});
    }
  }
  if (!key.empty() && key.back() == kColon) {
    key.pop_back();
  }
  if (source.more() &&
      (source.current() == kColon || source.current() == kLineFeed)) {
    source.next();
  }
  rightTrim(key);
  if (!isValidKey(key)) {
    throw SyntaxError(source.getPosition(),
                      "Invalid key '" + key + "' specified.");
  }
  if (isInsideFlowContext()) {
    return convertYAMLToStringNode(key, 0);
  }
  return convertYAMLToStringNode(key, keyQuoteIndent);
}
/// <summary>
/// Parse dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry Default_Parser::parseKeyValue(ISource &source,
                                              const Delimiters &delimiters,
                                              const unsigned long indentation) {
  const unsigned long keyIndent = source.getPosition().second;
  const auto keyLine = source.getPosition().first;
  Node keyNode = parseKey(source);
  source.ignoreWS();
  // Explicit-key value separator: "? key\n: value" form — after parsing a '?'
  // key, source lands on the ': value' line.  Consume ':' (and optional space)
  // BEFORE calling isKey(), because isKey() treats ':' as a valid key start.
  if (source.more() && source.current() == kColon) {
    source.next(); // consume ':'
    if (source.more() && source.current() == kSpace) {
      source.next(); // consume optional space after ':'
    } else if (inlineDictionaryDepth == 0 && source.more() &&
               source.current() == '\t') {
      SourceGuard tabGuard(source);
      while (source.more() && source.current() == '\t') {
        source.next();
      }
      if (!source.more() || source.current() == kSpace ||
          source.current() == kLineFeed || source.current() == '#') {
        if (source.current() == kSpace) {
          tabGuard.release();
        }
      } else {
        throw SyntaxError(
            source.getPosition(),
            "Tab used as block value-separator after ':'; block indentation "
            "must use spaces, not tabs (YAML 1.2 \xc2\xa7"
            "6.1).");
      }
    }
  } else if (isKey(source) && !isMapping(source) && !isAlias(source) &&
             (inlineDictionaryDepth > 0 ||
              source.getPosition().second > keyIndent)) {
    // Only throw when inside a flow collection (inlineDictionaryDepth > 0)
    // or when an unexpected key appears deeper than keyIndent, indicating a
    // compact-nested / same-line key where a simple value was expected.
    // At block level with the next key at the same column, the explicit key
    // simply has a null value and the sibling entry follows immediately
    // (e.g. "? b\n&anchor c: 3" — ZWK4).
    throw SyntaxError(source.getPosition(),
                      "Only an inline/compact dictionary is allowed.");
  }
  moveToNextIndent(source);
  // YAML 1.2 §8.2.1: a block sequence indicator '-' must start on its own
  // line, more indented than the surrounding block context.  If '-' appears
  // on the same line as the mapping key (implicit block mapping form where
  // the ':' was already consumed inline), it is a syntax error
  // (e.g. "key: - a" is invalid).
  if (inlineDictionaryDepth == 0 && isArray(source) &&
      source.getPosition().first == keyLine) {
    throw SyntaxError(
        source.getPosition(),
        "Block sequence indicator '-' cannot appear inline on the same line "
        "as a mapping key value (YAML 1.2 \xc2\xa7"
        "8.2.1).");
  }
  Node dictionaryNode = Node::make<Null>();
  if (source.more() && (source.getPosition().second > keyIndent ||
                        isInlineCollection(source) || isArray(source))) {
    const bool sameLineBlockFlowValue = inlineDictionaryDepth == 0 &&
                                        isInlineCollection(source) &&
                                        source.getPosition().first == keyLine;
    const auto previousBlockFlowValueIndent = blockFlowValueIndent;
    if (sameLineBlockFlowValue) {
      blockFlowValueIndent = keyIndent;
    }
    dictionaryNode = parseDocument(source, delimiters, indentation);
    blockFlowValueIndent = previousBlockFlowValueIndent;
  }
  return {keyNode, dictionaryNode};
}
/// <summary>
/// Parse inline dictionary key/value pair on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse a key/value pair.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary entry for key/value.</returns>
DictionaryEntry
Default_Parser::parseInlineKeyValue(ISource &source,
                                    const Delimiters &delimiters,
                                    const unsigned long indentation) {
  Node keyNode = parseKey(source);
  Node dictionaryNode = Node::make<Null>();
  // In a single-line flow mapping ({k: v}), parseKey already consumed ':'
  // when it was the next token. In multi-line flow mappings, comments or a
  // line break can separate the key from its ':' value separator.
  source.ignoreWS();
  if (source.more() && (source.current() == kLineFeed || isComment(source))) {
    moveToNextIndent(source);
  }
  if (source.more() && source.current() == kColon) {
    source.next(); // consume ':'
    source.ignoreWS();
  }
  if (source.more() && source.current() != kComma &&
      source.current() != kRightCurlyBrace) {
    dictionaryNode = parseDocument(source, delimiters, indentation);
  }
  if (isNullStringNode(dictionaryNode)) {
    dictionaryNode = Node::make<Null>();
  }
  return {keyNode, dictionaryNode};
}
/// <summary>
/// Parse a dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse dictionary.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary Node.</returns>
Node Default_Parser::parseDictionary(
    ISource &source, const Delimiters &delimiters,
    [[maybe_unused]] unsigned long indentation) {
  const unsigned long dictionaryIndent = source.getPosition().second;
  Node dictionaryNode = Node::make<Dictionary>();
  while (source.more() && dictionaryIndent == source.getPosition().second) {
    if (isKey(source)) {
      auto entry = parseKeyValue(source, delimiters, dictionaryIndent);
      addUniqueDictEntry(dictionaryNode, std::move(entry), source);
    } else if (isInsideFlowContext() &&
               (source.current() == kComma ||
                source.current() == kRightSquareBracket ||
                source.current() == kRightCurlyBrace)) {
      break;
    } else if (isDocumentBoundary(source)) {
      break;
    } else {
      if (dictionaryIndent == source.getPosition().second) {
        throw SyntaxError(source.getPosition(),
                          "Missing key/value pair from indentation level.");
      }
    }
    moveToNextIndent(source);
  }
  if (isKey(source) && dictionaryIndent < source.getPosition().second) {
    throw SyntaxError(source.getPosition(),
                      "Mapping key has the incorrect indentation.");
  }
  return mergeOverrides(dictionaryNode);
}
/// <summary>
/// Parse inline dictionary on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse inline dictionary.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Dictionary Node.</returns>
Node Default_Parser::parseInlineDictionary(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters,
    const unsigned long indentation) {
  const auto inLineDictionaryDelimiters =
      withExtras(delimiters, {kComma, kRightCurlyBrace});
  Node dictionaryNode = Node::make<Dictionary>();
  {
    DepthGuard depthGuard(inlineDictionaryDepth);
    do {
      // Save the line number of the '{' or ',' so we can detect if the next
      // key/value entry starts on a new line (multi-line flow mapping).
      const auto openLine = source.getPosition().first;
      source.next();
      moveToNextIndent(source);
      if (source.current() == kComma) {
        throw SyntaxError(source.getPosition(),
                          "Unexpected ',' in in-line dictionary.");
      }
      if (source.current() != kRightCurlyBrace) {
        // YAML 1.2 §7.4.2 / c-flow-mapping: when flow-mapping content starts
        // on a new line, it must be indented strictly deeper than the
        // surrounding block context (indentation).  Content at column
        // <= indentation violates the block-indentation rule.
        // The newline check avoids false positives in parseFromBuffer() where
        // positions restart at column 1 for re-parsed sub-strings.
        const bool crossedNewline = source.getPosition().first > openLine;
        if (crossedNewline && source.getPosition().second <= indentation) {
          throw SyntaxError(
              source.getPosition(),
              "Flow mapping content must be more indented than the surrounding "
              "block context (indentation level " +
                  std::to_string(indentation) + ").");
        }
        auto entry = parseInlineKeyValue(source, inLineDictionaryDelimiters,
                                         indentation);
        addInlineDictEntry(NRef<Dictionary>(dictionaryNode), std::move(entry),
                           source);
      }
    } while (source.current() == kComma);
  } // inlineDictionaryDepth decremented here
  checkForEnd(source, kRightCurlyBrace);
  if (source.current() == kColon) {
    throw SyntaxError(
        source.getPosition(),
        "Inline dictionary used as key is meant to be on one line.");
  }
  checkAtFlowClose(source, delimiters, inlineDictionaryDepth);
  return dictionaryNode;
}

} // namespace YAML_Lib