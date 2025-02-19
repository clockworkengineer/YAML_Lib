#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Translator.hpp"

namespace YAML_Lib {

class Bencode_Stringify final : public IStringify {
public:
  Bencode_Stringify() = default;
  Bencode_Stringify &operator=(const Bencode_Stringify &other) = delete;
  Bencode_Stringify(Bencode_Stringify &&other) = delete;
  Bencode_Stringify &operator=(Bencode_Stringify &&other) = delete;
  ~Bencode_Stringify() override = default;
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
      stringify(YRef<Document>(yNode)[0], destination, indent);
    } else if (isA<Number>(yNode)) {
      destination.add(
          "i" + std::to_string(YRef<Number>(yNode).value<long long>()) + "e");
    } else if (isA<String>(yNode)) {
      const auto yamlString = YRef<String>(yNode).value();
      destination.add(std::to_string(static_cast<int>(yamlString.length())) +
                      ":" + yamlString);
    } else if (isA<Boolean>(yNode)) {
      if (YRef<Boolean>(yNode).value()) {
        destination.add("4:True");
      } else {
        destination.add("5:False");
      }
    } else if (isA<Null>(yNode)) {
      destination.add("4:null");
    } else if (isA<Hole>(yNode)) {
    } else if (isA<Dictionary>(yNode)) {
      destination.add('d');
      for (auto &entry : YRef<Dictionary>(yNode).value()) {
        stringify(entry.getKeyYNode(), destination, indent);
        stringify(entry.getYNode(), destination, indent);
      }
      destination.add("e");
    } else if (isA<Array>(yNode)) {
      destination.add('l');
      for (auto &entry : YRef<Array>(yNode).value()) {
        stringify(entry, destination, indent);
      }
      destination.add("e");
    } else {
      throw Error("Unknown YNode type encountered during stringification.");
    }
  }

private:
  // YAML_Translator bencodeTranslator;
};

} // namespace YAML_Lib
