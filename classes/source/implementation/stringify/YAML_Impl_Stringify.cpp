
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

void stringifyYAML(IDestination &destination, const YNode &yNode) {
  if (isA<Number>(yNode)) {
    destination.add(YRef<Number>(yNode).toString());
  } else if (isA<String>(yNode)) {
    destination.add(YRef<String>(yNode).getQuote() + YRef<String>(yNode).toString() + YRef<String>(yNode).getQuote());
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
      destination.add(YRef<String>(entry.getKeyYNode()).toString());
      destination.add(": ");
      stringifyYAML(destination, entry.getYNode());
      destination.add('\n');
    }
  } else if (isA<Array>(yNode)) {
    if (!YRef<Array>(yNode).value().empty()) {
      for (const auto &entry : YRef<Array>(yNode).value()) {
        if (YRef<Array>(yNode).getIndentation() > 1) {
          destination.add(
              std::string(YRef<Array>(yNode).getIndentation() - 1, ' '));
        }
        destination.add("- ");
        stringifyYAML(destination, entry);
        destination.add("\n");
      }
    }

  } else if (isA<Document>(yNode)) {
    destination.add("---\n");
    for (const auto &entry : YRef<Document>(yNode).value()) {
      stringifyYAML(destination, entry);
    }
    destination.add("...\n");
  } else {
    throw Error("Unknown YNode type encountered during stringification.");
  }
}
void YAML_Impl::stringify(IDestination &destination) const {
  for (auto &document : yamlYNodeTree) {
    stringifyYAML(destination, document);
  }
}

} // namespace YAML_Lib
