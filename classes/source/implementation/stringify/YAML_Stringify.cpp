
//
// Class: YAML_Stringify
//
// Description: Default YAML stringifier.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Stringify.hpp"
#include "YAML_Translator.hpp"

namespace YAML_Lib {
/// <summary>
///
/// </summary>
/// <param name="target"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
std::vector<std::string> splitString(const std::string &target,
                                     const char delimiter) {

  std::vector<std::string> splitStrings;
  if (!target.empty()) {
    std::stringstream sourceStream(target);

    for (std::string splitOffItem{};
         std::getline(sourceStream, splitOffItem, delimiter);) {
      splitStrings.push_back(splitOffItem);
      splitStrings.back().push_back(delimiter);
    }
    splitStrings.back().pop_back();
  }
  return splitStrings;
}
/// <summary>
///
/// </summary>
/// <param name="destination"></param>
/// <param name="indent"></param>
/// <returns></returns>
std::string calculateIndent(IDestination &destination, unsigned long indent) {
  if (destination.last() == kLineFeed) {
    return std::string(indent, kSpace);
  }
  return "";
}
/// <summary>
///
/// </summary>
/// <param name="destination"></param>
/// <param name="yNode"></param>
void stringifyAnyBlockStyle(IDestination &destination, const YNode &yNode) {
  if (isA<String>(yNode)) {
    auto quote = YRef<String>(yNode).getQuote();
    if (quote == '>' || quote == '|') {
      destination.add("|");
      destination.add(kLineFeed);
    }
  }
}
/// <summary>
///
/// </summary>
/// <param name="destination"></param>
/// <param name="yNode"></param>
/// <param name="indent"></param>
void YAML_Stringify::stringifyYAML(IDestination &destination,
                                   const YNode &yNode, unsigned long indent) {
  YAML_Translator translator;
  if (isA<Number>(yNode)) {
    destination.add(YRef<Number>(yNode).toString());
  } else if (isA<String>(yNode)) {
    char quote = YRef<String>(yNode).getQuote();
    if (quote == '\'' || quote == '"') {
      std::string yamlString{YRef<String>(yNode).toString()};
      if (quote == '"') {
        yamlString = translator.to(yamlString);
      }
      destination.add(quote + yamlString + quote);
    } else {
      for (const auto &line :
           splitString(YRef<String>(yNode).toString(), kLineFeed)) {
        destination.add(calculateIndent(destination, indent));
        destination.add(line);
      }
    }
  } else if (isA<Comment>(yNode)) {
    destination.add("#" + YRef<Comment>(yNode).value() + kLineFeed);
  } else if (isA<Boolean>(yNode)) {
    destination.add(YRef<Boolean>(yNode).toString());
  } else if (isA<Null>(yNode)) {
    destination.add(YRef<Null>(yNode).toString());
  } else if (isA<Hole>(yNode)) {
    destination.add(YRef<Hole>(yNode).toString());
  } else if (isA<Dictionary>(yNode)) {
    for (const auto &entryYNode : YRef<Dictionary>(yNode).value()) {
      destination.add(calculateIndent(destination, indent));
      char quote = YRef<String>(entryYNode.getKeyYNode()).getQuote();
      if (quote == '\'' || quote == '"') {
        destination.add(
            quote + YRef<String>(entryYNode.getKeyYNode()).toString() + quote);
      } else {
        destination.add(YRef<String>(entryYNode.getKeyYNode()).toString());
      }
      destination.add(": ");
      stringifyAnyBlockStyle(destination, entryYNode.getYNode());
      if (isA<Array>(entryYNode.getYNode()) ||
          isA<Dictionary>(entryYNode.getYNode())) {
        destination.add(kLineFeed);
      }
      stringifyYAML(destination, entryYNode.getYNode(),
                    indent + yamlIndentation);
      if (!isA<Array>(entryYNode.getYNode()) &&
          !isA<Dictionary>(entryYNode.getYNode()) &&
          !isA<Comment>(entryYNode.getYNode())) {
        destination.add(kLineFeed);
      }
    }
  } else if (isA<Array>(yNode)) {
    for (const auto &entryYNode : YRef<Array>(yNode).value()) {
      destination.add(calculateIndent(destination, indent) + "- ");
      stringifyAnyBlockStyle(destination, entryYNode);
      stringifyYAML(destination, entryYNode, indent + yamlIndentation);
      if (destination.last() != kLineFeed) {
        destination.add(kLineFeed);
      }
    }
  } else if (isA<Document>(yNode)) {
    destination.add("---");
    if (!YRef<Document>(yNode).value().empty()) {
      stringifyAnyBlockStyle(destination, YRef<Document>(yNode)[0]);
      if (destination.last() != kLineFeed) {
        destination.add(kLineFeed);
      }
    }
    for (const auto &entryYNode : YRef<Document>(yNode).value()) {
      stringifyYAML(destination, entryYNode, 0);
    }
    if (destination.last() != kLineFeed) {
      destination.add(kLineFeed);
    }
    destination.add("...");
    destination.add(kLineFeed);
  } else {
    throw Error("Unknown YNode type encountered during stringification.");
  }
}
/// <summary>
///
/// </summary>
/// <param name="yamlTree"></param>
/// <param name="destination"></param>
void YAML_Stringify::stringify(const std::vector<YNode> &yamlTree,
                               IDestination &destination) const {
  for (const auto &document : yamlTree) {
    stringifyYAML(destination, document, 0);
  }
}

} // namespace YAML_Lib
