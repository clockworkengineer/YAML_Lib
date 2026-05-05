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
  // Document markers (--- / ...) are not permitted as values inside a flow
  // collection.  Encountering one here means the input is malformed.
  if (isInsideFlowContext() && isDocumentBoundary(source)) {
    YAML_THROW_POS(source, "Document marker not permitted inside flow collection.");
  }
  for (std::size_t i = 0; i < parsers_.size(); ++i) {
    const auto &[fst, snd] = parsers_[i];
    if ((this->*fst)(source)) {
      if (Node yNode = (this->*snd)(source, delimiters, indentation); !yNode.isEmpty()) {
        moveToNextIndent(source);
        return yNode;
      }
    }
  }
  YAML_THROW_POS(source, "Invalid YAML encountered.");
}
/// <summary>
/// Parse YAML documents on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <returns>Array of YAML documents.</returns>
std::vector<Node> Default_Parser::parse(ISource &source) {
  std::vector<Node> yNodeTree;
  ctx_.arrayIndentLevel = 0;
  ctx_.inlineArrayDepth = 0;
  ctx_.inlineDictionaryDepth = 0;
  ctx_.blockFlowValueIndent = 0;
  ctx_.yamlAliasMap.clear();
  ctx_.yamlAliasMap.reserve(16);
  ctx_.yamlTagPrefixes.clear();
  ctx_.activeAliasExpansions.clear();
  ctx_.yamlDirectiveMinor = 2;
  ctx_.yamlDirectiveSeen = false;
  const auto resetDocumentState = [&]() {
    ctx_.yamlAliasMap.clear();
    ctx_.yamlAliasMap.reserve(16);
    ctx_.activeAliasExpansions.clear();
    ctx_.yamlTagPrefixes.clear();
    ctx_.yamlDirectiveMinor = 2;
    ctx_.yamlDirectiveSeen = false;
  };
  for (bool inDocument = false, pendingDirectives = false; source.more();) {
    // Directives (%YAML or %TAG) — only valid before a document starts
    if (isDirective(source)) {
      parseDirective(source, inDocument);
      pendingDirectives = true;
      // Start of a document
    } else if (isDocumentStart(source)) {
      if (inDocument) {
        resetDocumentState();
      }
      inDocument = true;
      pendingDirectives = false;
      yNodeTree.push_back(Node::make<Document>());
      source.next();
      source.next();
      source.next(); // consume '-', '-', '-'
      source.ignoreWS();
      if (source.more() && source.current() != kLineFeed &&
          !isComment(source) &&
          (isKey(source) || isMapping(source) || isArray(source))) {
        YAML_THROW_POS(source, "Block collection cannot start on the same line as "
                          "document start.");
      }
      if (!source.more() || source.current() == kLineFeed ||
          isComment(source)) {
        moveToNextIndent(source);
      }
      // End of a document
    } else if (isDocumentEnd(source)) {
      if (!inDocument && pendingDirectives) {
        YAML_THROW_POS(source, "Directive must be followed by a document.");
      }
      // Consume "..." then validate what follows on the same line.
      // Per the YAML spec, "..." is a document-end suffix, not an indicator
      // embedded in content. Content after "... " (space-separated) is invalid;
      // the pattern "...x" (no space) is treated as an embedded token and
      // skipped for backward compatibility.
      source.next();
      source.next();
      source.next(); // consume '.', '.', '.'
      if (source.more() && source.isWS()) {
        // "... something" form — validate only whitespace/comment allowed.
        source.ignoreWS();
        if (source.more() && source.current() != kLineFeed &&
            !isComment(source)) {
          YAML_THROW_POS(source, "Invalid content after document-end marker '...'.");
        }
      }
      skipLine(source);
      moveToNextIndent(source);
      if (!inDocument) {
        yNodeTree.push_back(Node::make<Document>());
      }
      inDocument = false;
      resetDocumentState();
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
        pendingDirectives = false;
      }
      inDocument = true;
      if (NRef<Document>(yNodeTree.back()).size() == 0) {
        NRef<Document>(yNodeTree.back())
            .add(parseDocument(source, {kLineFeed, '#'}, 0));
      } else {
        YAML_THROW_POS(source, "Invalid YAML encountered.");
      }
    }
    if (!source.more() && pendingDirectives) {
      YAML_THROW_POS(source, "Directive must be followed by a document.");
    }
  }

  return yNodeTree;
}
/// <summary>
/// Parse a single YAML directive line (%YAML or %TAG).
/// </summary>
/// <param name="source">Source stream (positioned at '%').</param>
/// <param name="inDocument">True if a document has already started.</param>
void Default_Parser::parseDirective(ISource &source, const bool inDocument) {
  if (inDocument) {
    YAML_THROW_POS(source, "Directives must appear before document start.");
  }
  source.next(); // consume '%'
  // Extract the full directive name so we don't mistake "%YAMLL" for "%YAML"
  // (source.match does a prefix-match that would consume "YAML" from "YAMLL").
  const std::string directiveName{
      extractToNext(source, {kLineFeed, kSpace, '\t'})};
  if (directiveName == "YAML") {
    // %YAML major.minor
    source.ignoreWS();
    std::string version{extractToNext(source, {kLineFeed, ' '})};
    const auto dot = version.find('.');
    if (dot == std::string::npos) {
      YAML_THROW_POS(source, "%YAML directive missing version number.");
    }
    // Validate: version must be all-digit . all-digit (no stray chars like '#')
    const std::string majorStr = version.substr(0, dot);
    const std::string minorStr = version.substr(dot + 1);
    const auto isAllDigits = [](const std::string &s) {
      return !s.empty() && std::all_of(s.begin(), s.end(), [](unsigned char c) {
        return std::isdigit(c) != 0;
      });
    };
    if (!isAllDigits(majorStr) || !isAllDigits(minorStr)) {
      YAML_THROW_POS(source, "%YAML directive has invalid version number '" +
                            version + "'.");
    }
    const int major = std::stoi(majorStr);
    const int minor = std::stoi(minorStr);
    if (major != 1) {
      YAML_THROW_POS(source, "%YAML directive: unsupported major version " +
                            std::to_string(major) + ".");
    }
    if (ctx_.yamlDirectiveSeen) {
      YAML_THROW_POS(source, "%YAML directive appears more than once for the same document.");
    }
    ctx_.yamlDirectiveSeen = true;
    ctx_.yamlDirectiveMinor = minor;
    source.ignoreWS();
    if (source.more() && source.current() != kLineFeed &&
        source.current() != '#') {
      // Allow one extra version-looking token such as "1.2" in weird but
      // valid legacy directive forms (e.g. ZYU8/2).
      const std::string trailing{extractToNext(source, {kLineFeed, ' '})};
      const auto dot2 = trailing.find('.');
      if (dot2 == std::string::npos) {
        YAML_THROW_POS(source, "%YAML directive has unexpected content after version.");
      }
      const std::string majorSuffix = trailing.substr(0, dot2);
      const std::string minorSuffix = trailing.substr(dot2 + 1);
      if (!isAllDigits(majorSuffix) || !isAllDigits(minorSuffix)) {
        YAML_THROW_POS(source, "%YAML directive has unexpected content after version.");
      }
      source.ignoreWS();
      if (source.more() && source.current() != kLineFeed &&
          source.current() != '#') {
        YAML_THROW_POS(source, "%YAML directive has unexpected content after version.");
      }
    }
  } else if (directiveName == "TAG") {
    // %TAG handle prefix
    source.ignoreWS();
    std::string handle{extractToNext(source, {' '})};
    source.ignoreWS();
    std::string prefix{extractToNext(source, {kLineFeed, ' '})};
    ctx_.yamlTagPrefixes[handle] = prefix;
  } else {
    // Unknown directive — YAML spec says warn and ignore
  }
  skipLine(source); // always advance past the directive line
}
} // namespace YAML_Lib