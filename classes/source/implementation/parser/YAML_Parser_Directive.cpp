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
/// Supports both single alias (<<: *alias) and sequence of aliases
/// (<<: [*a, *b, ...]) where earlier entries in the sequence have higher
/// priority (first definition wins).
/// </summary>
Node Default_Parser::mergeOverrides(Node &overrideRoot) {
  if (isA<Dictionary>(overrideRoot) &&
      NRef<Dictionary>(overrideRoot).contains(kOverride)) {
    auto &dictionary = NRef<Dictionary>(overrideRoot);
    // Collect explicit (non-override) keys for later merging.
    std::set<std::string> overrideKeys;
    for (auto &entry : dictionary.value()) {
      if (entry.getKey() != kOverride) {
        overrideKeys.insert(std::string(entry.getKey()));
      }
    }
    Node &overrideValue = dictionary[kOverride];
    if (isA<Dictionary>(overrideValue)) {
      // Single-alias merge: <<: *alias
      auto &innerDictionary = NRef<Dictionary>(overrideValue);
      for (auto &entry : overrideKeys) {
        auto overrideEntry = mergeOverrides(dictionary[entry]);
        upsertDictEntry(innerDictionary, entry, std::move(overrideEntry));
      }
      overrideRoot = std::move(overrideValue);
    } else if (isA<Array>(overrideValue)) {
      // Multi-alias merge: <<: [*a, *b, ...]
      // Earlier entries in the sequence have higher priority (first wins).
      auto mergedBase = Node::make<Dictionary>();
      auto &mergedDict = NRef<Dictionary>(mergedBase);
      for (auto &element : NRef<Array>(overrideValue).value()) {
        if (!isA<Dictionary>(element)) {
          throw SyntaxError(
              "Merge key '<<' sequence must contain only mappings.");
        }
        for (auto &entry : NRef<Dictionary>(element).value()) {
          const std::string key{entry.getKey()};
          if (!mergedDict.contains(key)) {
            mergedDict.add(DictionaryEntry(key, entry.getNode()));
          }
        }
      }
      // Explicit outer keys override the merged base.
      for (auto &entry : overrideKeys) {
        auto overrideEntry = mergeOverrides(dictionary[entry]);
        upsertDictEntry(mergedDict, entry, std::move(overrideEntry));
      }
      overrideRoot = std::move(mergedBase);
    }
  }
  return std::move(overrideRoot);
}

/// <summary>/// Look up alias name in the alias map; throw SyntaxError if not
/// found.
/// </summary>
/// <param name="name">Alias name to resolve.</param>
/// <param name="source">Source stream (used only for error position).</param>
/// <returns>Reference to the stored unparsed alias value.</returns>
const std::string &Default_Parser::resolveAlias(const std::string &name,
                                                ISource &source) {
  if (!yamlAliasMap.count(name)) {
    throw SyntaxError(source.getPosition(), "Undefined alias '" + name + "'.");
  }
  return yamlAliasMap[name];
}

/// <summary>/// Parse a comment on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse comment.</param>
/// <returns>Comment Node.</returns>
Node Default_Parser::parseComment(
    ISource &source, [[maybe_unused]] const Delimiters &delimiters) {
  source.next();
  std::string comment{extractToNext(source, {kLineFeed})};
  if (source.more()) {
    source.next();
  }
  return Node::make<Comment>(comment);
}

/// <summary>
/// Parse anchor on source stream.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse anchor.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Anchor Node.</returns>
Node Default_Parser::parseAnchor(ISource &source, const Delimiters &delimiters,
                                 const unsigned long indentation) {
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
    // Only capture lines that are MORE indented than the parent (indentation).
    // If anchorIndent <= indentation the next line is a sibling mapping key,
    // not the anchor's value — leave source positioned there and store an
    // empty value so the anchor resolves to null.
    if (anchorIndent > indentation) {
      unparsed = captureIndentedBlock(source, anchorIndent);
    }
  }
  yamlAliasMap[name] = unparsed;
  if (unparsed.empty()) {
    return Node::make<Null>();
  }
  return parseFromBuffer(unparsed, delimiters, indentation);
}
/// <summary>
/// Parse alias on source stream and substitute alias.
/// In flow contexts (inline arrays/dicts), the alias name is delimited by
/// ',' or ']'/'}'.  The passed `delimiters` are merged with the fixed
/// name-stop set so that names are correctly extracted in all contexts.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse alias.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Alias anchor.</returns>
Node Default_Parser::parseAlias(ISource &source, const Delimiters &delimiters,
                                const unsigned long indentation) {
  source.next(); // consume '*'
  // Stop alias-name extraction at flow separators as well as space/linefeed.
  Delimiters nameDelimiters{kLineFeed, kSpace, kComma, kRightSquareBracket,
                            kRightCurlyBrace};
  const std::string name{extractToNext(source, nameDelimiters)};
  // Advance past trailing spaces; consume a terminating linefeed in block
  // context only (flow terminators such as ',' or ']' must not be consumed).
  source.ignoreWS();
  if (source.more() && source.current() == kLineFeed) {
    source.next();
  }
  if (activeAliasExpansions.count(name)) {
    throw SyntaxError(source.getPosition(),
                      "Recursive anchor detected: '" + name + "'.");
  }
  const std::string &unparsed = resolveAlias(name, source);
  if (unparsed.empty()) {
    return Node::make<Null>();
  }
  activeAliasExpansions.insert(name);
  try {
    auto result = parseFromBuffer(unparsed, delimiters, indentation);
    activeAliasExpansions.erase(name);
    return result;
  } catch (...) {
    activeAliasExpansions.erase(name);
    throw;
  }
}
/// <summary>
/// Parse alias on source stream, substitute alias, and any overrides.
/// </summary>
/// <param name="source">Source stream.</param>
/// <param name="delimiters">Delimiters used to parse alias.</param>
/// <param name="indentation">Parent indentation.</param>
/// <returns>Alias anchor with overrides.</returns>
Node Default_Parser::parseOverride(ISource &source,
                                   const Delimiters &delimiters,
                                   const unsigned long indentation) {
  [[maybe_unused]] const bool consumed = source.match("<<:");
  source.ignoreWS();
  if (source.current() != '*') {
    throw SyntaxError(source.getPosition(), "Missing '*' from alias.");
  }
  source.next();
  const std::string name{extractToNext(source, {kLineFeed, kSpace})};
  source.next();
  const std::string &unparsed = resolveAlias(name, source);
  return parseFromBuffer(unparsed, delimiters, indentation);
}

} // namespace YAML_Lib