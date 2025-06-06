#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class JSON_Stringify final : public IStringify {
public:
  explicit JSON_Stringify(std::unique_ptr<ITranslator> translator =
                            std::make_unique<Default_Translator>())
  {jsonTranslator = std::move(translator);}
  JSON_Stringify &operator=(const JSON_Stringify &other) = delete;
  JSON_Stringify(JSON_Stringify &&other) = delete;
  JSON_Stringify &operator=(JSON_Stringify &&other) = delete;
  ~JSON_Stringify() override = default;

  /// <summary>
  /// Recursively traverse YNode structure encoding it into JSON string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">YNode structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified JSON.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringify(const YNode &yNode, IDestination &destination,
                 const unsigned long indent) const override {
   stringifyYNodes(yNode,destination, indent);
  }

private:
  static void stringifyYNodes(const YNode &yNode, IDestination &destination,
                [[maybe_unused]]  const unsigned long indent)   {
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

  static void stringifyDocument(const YNode &yNode, IDestination &destination,
                         const long indent)  {
    stringifyYNodes(YRef<Document>(yNode)[0], destination, indent);
  }
  static void stringifyNumber(const YNode &yNode, IDestination &destination) {
    destination.add(YRef<Number>(yNode).toString());
  }
  static void stringifyString(const YNode &yNode, IDestination &destination)  {
    const auto yamlString = YRef<String>(yNode).value();
    destination.add("\"" + jsonTranslator->to(yamlString) + "\"");
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
  static void stringifyDictionary(const YNode &yNode,
                           IDestination &destination)  {
    auto comma = YRef<Dictionary>(yNode).value().size() - 1;
    destination.add('{');
    for (auto &entry : YRef<Dictionary>(yNode).value()) {
      stringifyYNodes(entry.getKeyYNode(), destination, 0);
      destination.add(":");
      stringifyYNodes(entry.getYNode(), destination, 0);
      if (comma-- > 0) {
        destination.add(",");
      }
    }
    destination.add("}");
  }
  static void stringifyAray(const YNode &yNode, IDestination &destination)  {
    auto comma = YRef<Array>(yNode).value().size() - 1;
    destination.add('[');
    for (auto &entry : YRef<Array>(yNode).value()) {
      stringifyYNodes(entry, destination, 0);
      if (comma-- > 0) {
        destination.add(",");
      }
    }
    destination.add("]");
  }

  inline static std::unique_ptr<ITranslator> jsonTranslator;
};

} // namespace YAML_Lib
