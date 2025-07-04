#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {
class Bencode_Stringify final : public IStringify {
public:
  Bencode_Stringify() = default;
  Bencode_Stringify &operator=(const Bencode_Stringify &other) = delete;
  Bencode_Stringify(Bencode_Stringify &&other) = delete;
  Bencode_Stringify &operator=(Bencode_Stringify &&other) = delete;
  ~Bencode_Stringify() override = default;

  /// <summary>
  /// Recursively traverse YNode structure encoding it into Bencode string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">YNode structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified
  /// Bencode.</param> <param name="indent">Current print indentation.</param>
  void stringify(const YNode &yNode, IDestination &destination,
                 const unsigned long indent) const override {
    stringifyYNodes(yNode,destination, indent);
  }

private:
  static void stringifyYNodes(const YNode &yNode, IDestination &destination,
               [[maybe_unused]] const unsigned long indent)   {
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
    destination.add(
        "i" + std::to_string(YRef<Number>(yNode).value<long long>()) + "e");
  }
  static void stringifyString(const YNode &yNode, IDestination &destination) {
    const auto yamlString = YRef<String>(yNode).value();
    destination.add(std::to_string(static_cast<int>(yamlString.length())) +
                    ":" + std::string(yamlString));
  }
  static void stringifyBoolean(const YNode &yNode, IDestination &destination) {
    if (YRef<Boolean>(yNode).value()) {
      destination.add("4:True");
    } else {
      destination.add("5:False");
    }
  }
  static void stringifyNull([[maybe_unused]] const YNode &yNode,
                            IDestination &destination) {
    destination.add("4:null");
  }
  static void stringifyDictionary(const YNode &yNode,
                           IDestination &destination)  {
    destination.add('d');
    for (auto &entry : YRef<Dictionary>(yNode).value()) {
      stringifyYNodes(entry.getKeyYNode(), destination, 0);
      stringifyYNodes(entry.getYNode(), destination, 0);
    }
    destination.add("e");
  }
  static void stringifyAray(const YNode &yNode, IDestination &destination)  {
    destination.add('l');
    for (auto &entry : YRef<Array>(yNode).value()) {
      stringifyYNodes(entry, destination, 0);
    }
    destination.add("e");
  }
};
} // namespace YAML_Lib
