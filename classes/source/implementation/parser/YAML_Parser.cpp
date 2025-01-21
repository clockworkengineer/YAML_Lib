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
/// Merge overrides/extensions in dictionary. Overrides will have
/// "<<" keys; this function edits them into the YAML tree.
/// </summary>
YNode YAML_Parser::mergeOverrides(YNode &overrideRoot) {
  if (isA<Dictionary>(overrideRoot) &&
      YRef<Dictionary>(overrideRoot).contains(kOverride)) {
    auto &dictionary = YRef<Dictionary>(overrideRoot);
    std::set<std::string> overrideKeys;
    for (auto &entry : dictionary.value()) {
      if (entry.getKey() != kOverride) {
        overrideKeys.insert(entry.getKey());
      }
    }
    auto &innerDictionary = YRef<Dictionary>(dictionary[kOverride]);
    for (auto &entry : overrideKeys) {
      auto overrideEntry = mergeOverrides(dictionary[entry]);
      if (innerDictionary.contains(entry)) {
        innerDictionary[entry] = std::move(overrideEntry);
      } else {
        innerDictionary.add(DictionaryEntry(entry, overrideEntry));
      }
    }
    overrideRoot = std::move(overrideRoot[kOverride]);
  }
  return std::move(overrideRoot);
}

/// <summary>
/// Parse a comment on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse comment.<</param>
/// <returns>Comment YNode.</returns>
YNode YAML_Parser::parseComment(ISource &source,
                                [[maybe_unused]] const Delimiters &delimiters) {
  source.next();
  std::string comment{extractToNext(source, {kLineFeed})};
  if (source.more()) {
    source.next();
  }
  return YNode::make<Comment>(comment);
}

/// <summary>
/// Parse anchor on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse anchor.</param>
/// <returns>Anchor YNode.</returns>
YNode YAML_Parser::parseAnchor(ISource &source, const Delimiters &delimiters) {
  source.next();
  const std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.ignoreWS();
  std::string unparsed{};
  if (source.current() != kLineFeed) {
    unparsed += extractToNext(source, {kLineFeed});
    moveToNextIndent(source);
  } else {
    moveToNextIndent(source);
    const auto anchorIndent = source.getPosition().second;
    do {
      std::string indent(source.getPosition().second, kSpace);
      unparsed += indent + extractToNext(source, {kLineFeed}) + "\n";
      moveToNextIndent(source);
    } while (source.getPosition().second >= anchorIndent);
  }
  yamlAliasMap[name] = unparsed;
  BufferSource anchor{unparsed};
  return parseDocument(anchor, delimiters);
}
/// <summary>
/// Parse alias on source stream and substitute alias.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse alias.</param>
/// <returns>Alias anchor.</returns>
YNode YAML_Parser::parseAlias(ISource &source, const Delimiters &delimiters) {
  source.next();
  const std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  const std::string unparsed{yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  return parseDocument(anchor, delimiters);
}
/// <summary>
/// Parse alias on source stream, substitute alias, and any overrides.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse alias.</param>
/// <returns>Alias anchor with overrides.</returns>
YNode YAML_Parser::parseOverride(ISource &source,
                                 const Delimiters &delimiters) {
  source.next();
  source.next();
  source.next();
  source.ignoreWS();
  if (source.current() != '*') {
    throw SyntaxError(source.getPosition(), "Missing '*' from alias.");
  }
  source.next();
  const std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  const std::string unparsed{yamlAliasMap[name]};
  BufferSource anchor{unparsed};
  YNode parsed = parseDocument(anchor, delimiters);
  return parsed;
}
/// <summary>
/// Parse YAML document on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse document.</param>
/// <returns>Document root YNode.</returns>
YNode YAML_Parser::parseDocument(ISource &source,
                                 const Delimiters &delimiters) {
  moveToNextIndent(source);
  for (const auto &[fst, snd] : parsers) {
    if (fst(source)) {
      if (YNode yNode = snd(source, delimiters); !yNode.isEmpty()) {
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
      parseComment(source, {kLineFeed});
      // Parse document contents
    } else {
      if (!inDocument) {
        yNodeTree.push_back(YNode::make<Document>());
      }
      inDocument = true;
      YRef<Document>(yNodeTree.back())
          .add(parseDocument(source, {kLineFeed, '#'}));
    }
  }

  return yNodeTree;
}
} // namespace YAML_Lib