
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
    destination.add('"' + YRef<String>(yNode).toString() + '"');
  } else if (isA<Comment>(yNode)) {
    destination.add("#" + YRef<Comment>(yNode).value() + "\n");
  } else if (isA<Boolean>(yNode)) {
    destination.add(YRef<Boolean>(yNode).toString());
  } else if (isA<Null>(yNode)) {
    destination.add(YRef<Null>(yNode).toString());
  } else if (isA<Hole>(yNode)) {
    destination.add(YRef<Hole>(yNode).toString());
  } else if (isA<Object>(yNode)) {
    // size_t commaCount = YRef<Object>(yNode).value().size() - 1;
    // destination.add('{');
    // if (indent != 0) {
    //   destination.add('\n');
    // }
    // for (auto &entry : YRef<Object>(yNode).value()) {
    //   if (indent != 0) {
    //     destination.add(std::string(indent, ' '));
    //   }
    //   stringify(entry.getKeyyNode(), destination,
    //             indent != 0 ? indent + printIndent : 0);
    //   destination.add(":");
    //   if (indent != 0) {
    //     destination.add(" ");
    //   }
    //   stringify(entry.getyNode(), destination,
    //             indent != 0 ? indent + printIndent : 0);
    //   if (commaCount-- > 0) {
    //     destination.add(",");
    //     if (indent != 0) {
    //       destination.add('\n');
    //     }
    //   }
    // }
    // if (indent != 0) {
    //   destination.add("\n" + std::string(indent - printIndent, ' '));
    // }
    // destination.add("}");
  } else if (isA<Array>(yNode)) {
    if (!YRef<Array>(yNode).value().empty()) {
      for (const auto &entry : YRef<Array>(yNode).value()) {
        destination.add("    - ");
        stringifyYAML(destination, entry);
        destination.add("\n");
      }
    }

  } else if (isA<Document>(yNode)) {
    for (const auto &entry : YRef<Document>(yNode).value()) {
      stringifyYAML(destination, entry);
    }
  } else {
    throw Error("Unknown YNode type encountered during stringification.");
  }
}
void YAML_Impl::stringify(IDestination &destination) const {
  for (auto &document : yamlDocuments) {
    destination.add("---\n");
    stringifyYAML(destination, document);
    destination.add("...\n");
  }
}

} // namespace YAML_Lib
