
//
// Class: YAML_Parser_Array
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Return true if node is an unquoted empty string (represents a null element
/// produced by a trailing comma in an inline collection).
/// </summary>
bool Default_Parser::isNullStringNode(const Node &node) {
  return isA<String>(node) && NRef<String>(node).value().empty() &&
         NRef<String>(node).getQuote() == kNull;
}

/// <summary>
/// Parse array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the array.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Array Node.</returns>
Node Default_Parser::parseArray(ISource &source, const Delimiters &delimiters,
                                [[maybe_unused]] unsigned long indentation) {
  const unsigned long arrayIndent = source.getPosition().second;
  auto arrayNode = Node::make<Array>();
  {
    DepthGuard depthGuard(ctx_.arrayIndentLevel);
    while (isArray(source) && arrayIndent == source.getPosition().second) {
      source.next(); // consume '-'
      // YAML 1.2 §6.1: block indentation must use spaces, not tabs.
      // Scan the separator whitespace between '-' and the content: if ANY
      // tab appears in that run (e.g. "- \t-" or "-\t-") and the content
      // that follows starts another block-structure indicator, the
      // indentation level is tab-determined → reject as invalid.
      bool tabInSeparator = false;
      while (source.more() && source.isWS()) {
        if (source.current() == '\t') {
          tabInSeparator = true;
        }
        source.next();
      }
      if (tabInSeparator && source.more() && isArray(source)) {
        YAML_THROW_POS(source, "Tab used as block sequence entry separator followed by another "
            "block structure indicator; block indentation must use spaces, "
            "not tabs (YAML 1.2 \u00a76.1).");
      }
      Node yNode = Node::make<Null>();
      if (source.current() != kLineFeed) {
        yNode = parseDocument(source, delimiters, arrayIndent);
      } else {
        moveToNextIndent(source);
        if (arrayIndent < source.getPosition().second) {
          yNode = parseDocument(source, delimiters, arrayIndent);
        }
      }
      NRef<Array>(arrayNode).add(std::move(yNode));
      moveToNextIndent(source);
    }
  } // ctx_.arrayIndentLevel decremented here (even on exception)
  if (isArray(source) && ctx_.arrayIndentLevel == 0 &&
      arrayIndent > source.getPosition().second) {
    YAML_THROW_POS(source, "Invalid indentation for array element.");
  }
  return arrayNode;
}
/// <summary>
/// Parse inline array on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse the inline array.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Array Node.</returns>
Node Default_Parser::parseInlineArray(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters,
    const unsigned long indentation) {
  const auto inLineArrayDelimiters =
      withExtras(delimiters, {kComma, kRightSquareBracket});
  auto arrayNode = Node::make<Array>();
  auto &yamlArray = NRef<Array>(arrayNode);
  {
    DepthGuard depthGuard(ctx_.inlineArrayDepth);
    do {
      source.next();
      // Patch: Disallow comment immediately after comma in flow sequence (YAML 1.2)
      if (source.current() == '#') {
        YAML_THROW_POS(source, "Comment must be separated from comma by whitespace in flow sequence.");
      }
      if (ctx_.inlineArrayDepth == 1 && source.more() &&
          source.current() == kLineFeed && ctx_.blockFlowValueIndent > 0) {
        SourceGuard guard(source);
        source.next();
        while (source.more() && source.current() == kLineFeed) {
          source.next();
        }
        unsigned long continuationIndent = 1;
        while (source.more() && source.current() == kSpace) {
          continuationIndent++;
          source.next();
        }
        if (source.more() && source.current() != kRightSquareBracket &&
            continuationIndent <= ctx_.blockFlowValueIndent) {
          YAML_THROW_POS(source, "Flow sequence continuation must be indented "
                            "beyond its parent block context.");
        }
      }
      yamlArray.add(parseDocument(source, inLineArrayDelimiters, indentation));
      // YAML 1.2 §7.3.3: A plain scalar consisting of only '-' is not allowed in flow context
      if (!yamlArray.value().empty()) {
        const auto &element = yamlArray.value().back();
        if (isA<String>(element) && NRef<String>(element).value() == "-" && NRef<String>(element).getQuote() == kNull) {
          YAML_THROW_POS(source, "Bare '-' is not a valid plain scalar in flow context.");
        }
      }
      if (auto &element = yamlArray.value().back(); isNullStringNode(element)) {
        if (source.current() != kRightSquareBracket) {
          YAML_THROW_POS(source, "Unexpected ',' in in-line array.");
        }
        yamlArray.value().pop_back();
      }
    } while (source.current() == kComma);
  } // ctx_.inlineArrayDepth decremented here
  checkForEnd(source, kRightSquareBracket);
  checkAtFlowClose(source, delimiters, ctx_.inlineArrayDepth);
  return arrayNode;
}
} // namespace YAML_Lib