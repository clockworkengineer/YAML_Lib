
//
// Class: YAML_Stringify
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Stringify.hpp"
#include "YAML_Translator.hpp"

namespace YAML_Lib {

std::vector<std::string> splitString(const std::string &target,
                                     const char delimiter) {
  std::stringstream sourceStream(target);
  std::string splitOffItem;
  std::vector<std::string> splitStrings;
  while (std::getline(sourceStream, splitOffItem, delimiter)) {
    splitStrings.push_back(splitOffItem);
  }
  return splitStrings;
}

unsigned long calcIndent(IDestination &destination, unsigned long indent) {
  if (destination.last()!='\n'){
    return (0);
  }
  return indent;
}

void stringifyYAML(IDestination &destination, const YNode &yNode,
                   unsigned long indent) {
  YAML_Translator translator;
  if (isA<Number>(yNode)) {
    destination.add(YRef<Number>(yNode).toString());
  } else if (isA<String>(yNode)) {
    char quote = YRef<String>(yNode).getQuote();
    if (quote == '\'' || quote == '"') {
      if (quote == '"') {
        destination.add(quote + translator.to(YRef<String>(yNode).toString()) +
                        quote);
      } else {
        destination.add(quote + YRef<String>(yNode).toString() + quote);
      }
    } else {
      std::vector<std::string> splitStrings{
          splitString(YRef<String>(yNode).toString(), kLineFeed)};
      if (splitStrings.size() > 1) {
        std::string last = splitStrings.back();
        splitStrings.pop_back();
        for (const auto &line : splitStrings) {
          destination.add(std::string(calcIndent(destination,indent), ' '));
          destination.add(line);
          destination.add("\n");
        }
        destination.add(std::string(calcIndent(destination,indent), ' '));
        destination.add(last);
      } else {
        destination.add(std::string(calcIndent(destination,indent), ' '));
        destination.add(YRef<String>(yNode).toString());
      }
    }
  } else if (isA<Anchor>(yNode)) {
    stringifyYAML(destination, YRef<Anchor>(yNode).value(), indent + 2);
  } else if (isA<Alias>(yNode)) {
    stringifyYAML(destination, YRef<Alias>(yNode).value(), indent + 2);
  } else if (isA<Comment>(yNode)) {
    destination.add("#" + YRef<Comment>(yNode).value() + "\n");
  } else if (isA<Boolean>(yNode)) {
    destination.add(YRef<Boolean>(yNode).toString());
  } else if (isA<Null>(yNode)) {
    destination.add(YRef<Null>(yNode).toString());
  } else if (isA<Hole>(yNode)) {
    destination.add(YRef<Hole>(yNode).toString());
  } else if (isA<Dictionary>(yNode)) {
    for (auto &entry : YRef<Dictionary>(yNode).value()) {
      destination.add(std::string(calcIndent(destination,indent), ' '));
      destination.add(YRef<String>(entry.getKeyYNode()).toString());
      destination.add(": ");
      if (isA<String>(entry.getYNode())) {
        if (YRef<String>(entry.getYNode()).getQuote() == '>' ||
            YRef<String>(entry.getYNode()).getQuote() == '|') {
          destination.add("|\n");
        }
      }
      if (isA<Array>(entry.getYNode()) || isA<Dictionary>(entry.getYNode())) {
        destination.add("\n");
      }
      stringifyYAML(destination, entry.getYNode(), indent + 2);
      if (!isA<Array>(entry.getYNode()) && !isA<Dictionary>(entry.getYNode())) {
        destination.add("\n");
      }
    }
  } else if (isA<Array>(yNode)) {
    if (!YRef<Array>(yNode).value().empty()) {
      for (const auto &entry : YRef<Array>(yNode).value()) {
        destination.add(std::string(calcIndent(destination,indent), ' '));
        destination.add("- ");
        stringifyYAML(destination, entry, indent + 2);
        destination.add("\n");
      }
    }
  } else if (isA<Document>(yNode)) {
    destination.add("---");
    if (!YRef<Document>(yNode).value().empty()) {
      if (isA<String>(YRef<Document>(yNode)[0])) {
        if (YRef<String>(YRef<Document>(yNode)[0]).getQuote() == '>' ||
            YRef<String>(YRef<Document>(yNode)[0]).getQuote() == '|') {
          destination.add(" |\n");
        } else {
          destination.add("\n");
        }
      } else {
        destination.add("\n");
      }
    } else {
      destination.add("\n");
    }
    for (const auto &entry : YRef<Document>(yNode).value()) {
      stringifyYAML(destination, entry, 0);
    }
    if (destination.last() != '\n') {
      destination.add('\n');
    }
    destination.add("...\n");
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
