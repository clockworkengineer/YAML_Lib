
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

std::vector<std::string> splitString(const std::string &target,
                                     const char delimiter) {
  std::stringstream sourceStream(target);
  std::vector<std::string> splitStrings;
  for (std::string splitOffItem{};
       std::getline(sourceStream, splitOffItem, delimiter);) {
    splitStrings.push_back(splitOffItem);
    splitStrings.back().push_back(delimiter);
  }
  splitStrings.back().pop_back();
  return splitStrings;
}

std::string calcIndent(IDestination &destination, unsigned long indent) {
  if (destination.last() == kLineFeed) {
    return std::string(indent, kSpace);
  }
  return "";
}

void YAML_Stringify::stringifyYAML(IDestination &destination,
                                   const YNode &yNode,
                                   unsigned long indent) const {
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
        destination.add(calcIndent(destination, indent));
        destination.add(line);
      }
    }
  } else if (isA<Anchor>(yNode)) {
    stringifyYAML(destination, YRef<Anchor>(yNode).value(),
                  indent + yamlIndentation);
  } else if (isA<Alias>(yNode)) {
    stringifyYAML(destination, YRef<Alias>(yNode).value(),
                  indent + yamlIndentation);
  } else if (isA<Comment>(yNode)) {
    destination.add("#" + YRef<Comment>(yNode).value() + kLineFeed);
  } else if (isA<Boolean>(yNode)) {
    destination.add(YRef<Boolean>(yNode).toString());
  } else if (isA<Null>(yNode)) {
    destination.add(YRef<Null>(yNode).toString());
  } else if (isA<Hole>(yNode)) {
    destination.add(YRef<Hole>(yNode).toString());
  } else if (isA<Dictionary>(yNode)) {
    for (auto &entry : YRef<Dictionary>(yNode).value()) {
      destination.add(calcIndent(destination, indent) +
                      YRef<String>(entry.getKeyYNode()).toString() + ": ");
      if (isA<String>(entry.getYNode())) {
        auto quote = YRef<String>(entry.getYNode()).getQuote();
        if (quote == '>' || quote == '|') {
          destination.add("|");
          destination.add(kLineFeed);
        }
      }
      if (isA<Array>(entry.getYNode()) || isA<Dictionary>(entry.getYNode())) {
        destination.add(kLineFeed);
      }
      stringifyYAML(destination, entry.getYNode(), indent + yamlIndentation);
      if (!isA<Array>(entry.getYNode()) && !isA<Dictionary>(entry.getYNode()) &&
          !isA<Comment>(entry.getYNode())) {
        destination.add(kLineFeed);
      }
    }
  } else if (isA<Array>(yNode)) {
    for (const auto &entry : YRef<Array>(yNode).value()) {
      destination.add(calcIndent(destination, indent) + "- ");
      if (isA<String>(entry)) {
        auto quote = YRef<String>(entry).getQuote();
        if (quote == '>' || quote == '|') {
          destination.add("|");
          destination.add(kLineFeed);
        }
      }
      stringifyYAML(destination, entry, indent + yamlIndentation);
      if (destination.last() != kLineFeed) {
        destination.add(kLineFeed);
      }
    }
  } else if (isA<Document>(yNode)) {
    destination.add("---");
    if (!YRef<Document>(yNode).value().empty()) {
      if (isA<String>(YRef<Document>(yNode)[0])) {
        auto quote = YRef<String>(YRef<Document>(yNode)[0]).getQuote();
        if (quote == '>' || quote == '|') {
          destination.add("|");
        }
      }
    }
    destination.add(kLineFeed);
    for (const auto &entry : YRef<Document>(yNode).value()) {
      stringifyYAML(destination, entry, 0);
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
void YAML_Stringify::stringify(const std::vector<YNode> &yamlTree,
                               IDestination &destination) const {
  for (auto &document : yamlTree) {
    stringifyYAML(destination, document, 0);
  }
}

} // namespace YAML_Lib
