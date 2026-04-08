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
/// <param name="indentation">Parent indentation.</param>
/// <returns>Document root Node.</returns>
Node Default_Parser::parseDocument(ISource &source,
                                   const Delimiters &delimiters,
                                   const unsigned long indentation) {
  moveToNextIndent(source);
  for (const auto &[fst, snd] : parsers) {
    if (fst(source)) {
      if (Node yNode = snd(source, delimiters, indentation); !yNode.isEmpty()) {
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
std::vector<Node> Default_Parser::parse(ISource &source) {
  validateInputCharacters(source);
  std::vector<Node> yNodeTree;
  arrayIndentLevel = 0;
  inlineArrayDepth = 0;
  inlineDictionaryDepth = 0;
  yamlAliasMap.clear();
  yamlTagPrefixes.clear();
  activeAliasExpansions.clear();
  yamlDirectiveMinor = 2;
  yamlDirectiveSeen = false;
  for (bool inDocument = false; source.more();) {
    // Directives (%YAML or %TAG) — only valid before a document starts
    if (isDirective(source)) {
      if (inDocument) {
        throw SyntaxError(source.getPosition(),
                          "Directives must appear before document start.");
      }
      source.next(); // consume '%'
      if (source.match("YAML")) {
        // %YAML major.minor
        source.ignoreWS();
        std::string version{extractToNext(source, {kLineFeed, ' '})};
        const auto dot = version.find('.');
        if (dot == std::string::npos) {
          throw SyntaxError(source.getPosition(),
                            "%YAML directive missing version number.");
        }
        const int major = std::stoi(version.substr(0, dot));
        const int minor = std::stoi(version.substr(dot + 1));
        if (major != 1) {
          throw SyntaxError(source.getPosition(),
                            "%YAML directive: unsupported major version " +
                                std::to_string(major) + ".");
        }
        if (yamlDirectiveSeen) {
          throw SyntaxError(
              source.getPosition(),
              "%YAML directive appears more than once for the same document.");
        }
        yamlDirectiveSeen = true;
        yamlDirectiveMinor = minor;
        skipLine(source);
      } else if (source.match("TAG")) {
        // %TAG handle prefix
        source.ignoreWS();
        std::string handle{extractToNext(source, {' '})};
        source.ignoreWS();
        std::string prefix{extractToNext(source, {kLineFeed, ' '})};
        yamlTagPrefixes[handle] = prefix;
        skipLine(source);
      } else {
        // Unknown directive — skip to end of line (per YAML spec: warn)
        skipLine(source);
      }
      // Start of a document
    } else if (isDocumentStart(source)) {
      inDocument = true;
      moveToNext(source, {kLineFeed, '|', '>'});
      moveToNextIndent(source);
      yNodeTree.push_back(Node::make<Document>());
      // End of a document
    } else if (isDocumentEnd(source)) {
      skipLine(source);
      moveToNextIndent(source);
      if (!inDocument) {
        yNodeTree.push_back(Node::make<Document>());
      }
      inDocument = false;
      yamlDirectiveSeen = false;
      // Inter document comment
    } else if (isComment(source) && !inDocument) {
      parseComment(source, {kLineFeed});
      // Skip stray whitespace (e.g. leading-space comment lines between
      // directives)
    } else if (!inDocument &&
               (source.current() == kSpace || source.current() == '\t' ||
                source.current() == kLineFeed)) {
      source.next();
      // Parse document contents
    } else {
      if (!inDocument) {
        yNodeTree.push_back(Node::make<Document>());
      }
      inDocument = true;
      if (NRef<Document>(yNodeTree.back()).size() == 0) {
        NRef<Document>(yNodeTree.back())
            .add(parseDocument(source, {kLineFeed, '#'}, 0));
      } else {
        throw SyntaxError(source.getPosition(), "Invalid YAML encountered.");
      }
    }
  }

  return yNodeTree;
}
} // namespace YAML_Lib