#pragma once

#include "Default_Translator.hpp"
#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class JSON_Stringify final : public IStringify {
public:
  JSON_Stringify() = default;
  JSON_Stringify &operator=(const JSON_Stringify &other) = delete;
  JSON_Stringify(JSON_Stringify &&other) = delete;
  JSON_Stringify &operator=(JSON_Stringify &&other) = delete;
  ~JSON_Stringify() override = default;

  /// <summary>
  /// Recursively traverse YNode structure encoding it into YAML string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">YNode structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified YAML.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringify(const YNode &yNode, IDestination &destination,
                 const unsigned long indent) const override {
    if (isA<Document>(yNode)) {
      stringifyDocument(yNode, destination, 0);
    } else if (isA<Number>(yNode)) {
      stringifyNumber(yNode, destination);
    } else if (isA<String>(yNode)) {
      stringifyString(yNode, destination);
    } else if (isA<Boolean>(yNode)) {
      stringifyBoolean(yNode, destination);
    } else if (isA<Null>(yNode)) {
      stringifyNull(yNode, destination);
    } else if (isA<Hole>(yNode)) {
    } else if (isA<Dictionary>(yNode)) {
      stringifyDictionary(yNode, destination);
    } else if (isA<Array>(yNode)) {
      stringifyAray(yNode, destination);
    } else {
      throw Error("Unknown YNode type encountered during stringification.");
    }
  }

  void stringifyDocument(const YNode &yNode, IDestination &destination,
                         const long indent) const {
    stringify(YRef<Document>(yNode)[0], destination, indent);
  }

  static void stringifyNumber(const YNode &yNode, IDestination &destination) {
    destination.add(YRef<Number>(yNode).toString());
  }

  void stringifyString(const YNode &yNode, IDestination &destination) const {
    const auto yamlString = YRef<String>(yNode).value();
    destination.add("\"" + jsonTranslator.to(yamlString) + "\"");
  }

  static void stringifyBoolean(const YNode &yNode, IDestination &destination) {
    if (YRef<Boolean>(yNode).value()) {
      destination.add("true");
    } else {
      destination.add("false");
    }
  }

  static void stringifyNull([[maybe_unused]] const YNode &yNode,
                            IDestination &destination) {
    destination.add("null");
  }

  void stringifyDictionary(const YNode &yNode,
                           IDestination &destination) const {
    auto comma = YRef<Dictionary>(yNode).value().size() - 1;
    destination.add('{');
    for (auto &entry : YRef<Dictionary>(yNode).value()) {
      stringify(entry.getKeyYNode(), destination, 0);
      destination.add(":");
      stringify(entry.getYNode(), destination, 0);
      if (comma-- > 0) {
        destination.add(",");
      }
    }
    destination.add("}");
  }
  void stringifyAray(const YNode &yNode, IDestination &destination) const {
    auto comma = YRef<Array>(yNode).value().size() - 1;
    destination.add('[');
    for (auto &entry : YRef<Array>(yNode).value()) {
      stringify(entry, destination, 0);
      if (comma-- > 0) {
        destination.add(",");
      }
    }
    destination.add("]");
  }

private:
  Default_Translator jsonTranslator;
};

} // namespace YAML_Lib
