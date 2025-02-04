//
// Class: YAML_Parser
//
// Description: Default YAML parser.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Parse YAML document on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse document.</param>
/// <returns>Document root YNode.</returns>
YNode YAML_Parser::parseDocument(ISource &source,
                                 const Delimiters &delimiters,
                                 unsigned long indentation) {
  moveToNextIndent(source);
  for (const auto &[fst, snd] : parsers) {
    if (fst(source)) {
      if (YNode yNode = snd(source, delimiters, indentation); !yNode.isEmpty()) {
        moveToNextIndent(source);
        return yNode;
      }
    }
  }
  throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
}
/// <summary>
/// Parse YAML documents on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Array of YAML documents.</returns>
std::vector<YNode> YAML_Parser::parse(ISource &source) {
  std::vector<YNode> yNodeTree;
  arrayIndentLevel = 0;
  inlineArrayDepth = 0;
  inlineDictionaryDepth = 0;
  for (bool inDocument = false; source.more();) {
    // Start of a document
    if (isDocumentStart(source)) {
      inDocument = true;
      moveToNext(source, {kLineFeed, '|', '>'});
      moveToNextIndent(source);
      yNodeTree.push_back(YNode::make<Document>());
      // End of a document
    } else if (isDocumentEnd(source)) {
      moveToNext(source, {kLineFeed});
      moveToNextIndent(source);
      if (!inDocument) {
        yNodeTree.push_back(YNode::make<Document>());
      }
      inDocument = false;
      // Inter document comment
    } else if (isComment(source) && !inDocument) {
      parseComment(source, {kLineFeed}, 0);
      // Parse document contents
    } else {
      if (!inDocument) {
        yNodeTree.push_back(YNode::make<Document>());
      }
      inDocument = true;
      if (YRef<Document>(yNodeTree.back()).size() == 0) {
        YRef<Document>(yNodeTree.back())
            .add(parseDocument(source, {kLineFeed, '#'}, 0));
      } else {
        throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
      }
    }
  }

  return yNodeTree;
}
} // namespace YAML_Lib