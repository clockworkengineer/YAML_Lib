#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Stringify_Helper.hpp"

namespace YAML_Lib {
class Bencode_Stringify final : public IStringify {
public:
  Bencode_Stringify() = default;
  Bencode_Stringify &operator=(const Bencode_Stringify &other) = delete;
  Bencode_Stringify(Bencode_Stringify &&other) = delete;
  Bencode_Stringify &operator=(Bencode_Stringify &&other) = delete;
  ~Bencode_Stringify() override = default;

  /// <summary>
  /// Recursively traverse Node structure encoding it into Bencode string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">Node structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified
  /// Bencode.</param> <param name="indent">Current print indentation.</param>
  void stringify(const Node &yNode, IDestination &destination,
                 const unsigned long indent) const override {
    stringifyNodes(yNode, destination, indent);
  }

private:
  static void stringifyNodes(const Node &yNode, IDestination &destination,
                             [[maybe_unused]] const unsigned long indent) {
    stringify_detail::dispatchStringifyNode(
        yNode, destination, indent,
        [](const Node &yNode, IDestination &destination, const long indent) {
          stringifyDocument(yNode, destination, indent);
        },
        [](const Node &yNode, IDestination &destination) {
          stringifyNumber(yNode, destination);
        },
        [](const Node &yNode, IDestination &destination) {
          stringifyString(yNode, destination);
        },
        [](const Node &yNode, IDestination &destination) {
          stringifyBoolean(yNode, destination);
        },
        [](const Node &yNode, IDestination &destination) {
          stringifyNull(yNode, destination);
        },
        [](const Node &yNode, IDestination &destination) {
          stringifyTimestamp(yNode, destination);
        },
        [](const Node &yNode, IDestination &destination) {
          stringifyDictionary(yNode, destination);
        },
        [](const Node &yNode, IDestination &destination) {
          stringifyAray(yNode, destination);
        });
  }
  // Intentional parallel to JSON_Stringify/XML_Stringify: unwrap Document
  // and recurse. Default_Stringify differs (emits --- / ... markers).
  static void stringifyDocument(const Node &yNode, IDestination &destination,
                                const long indent) {
    stringify_detail::stringifyDocument(yNode, destination, indent, stringifyNodes);
  }

  static void stringifyNumber(const Node &yNode, IDestination &destination) {
    destination.add("i" + stringify_detail::integerToString(yNode) + "e");
  }
  static void addBencodeString(IDestination &destination,
                               const std::string_view sv) {
    destination.add(std::to_string(sv.length()) + ":" + std::string(sv));
  }
  static void stringifyString(const Node &yNode, IDestination &destination) {
    addBencodeString(destination, NRef<String>(yNode).value());
  }
  static void stringifyBoolean(const Node &yNode, IDestination &destination) {
    stringify_detail::addBooleanLiteral(destination,
                                        NRef<Boolean>(yNode).value(),
                                        "4:True", "5:False");
  }
  static void stringifyNull([[maybe_unused]] const Node &yNode,
                            IDestination &destination) {
    destination.add("4:null");
  }
  static void stringifyTimestamp(const Node &yNode, IDestination &destination) {
    addBencodeString(destination, NRef<Timestamp>(yNode).value());
  }
  static void stringifyDictionary(const Node &yNode,
                                  IDestination &destination) {
    destination.add('d');
    for (auto &entry : NRef<Dictionary>(yNode).value()) {
      addBencodeString(destination, entry.getKey());
      stringifyNodes(entry.getNode(), destination, 0);
    }
    destination.add("e");
  }
  static void stringifyAray(const Node &yNode, IDestination &destination) {
    destination.add('l');
    for (auto &entry : NRef<Array>(yNode).value()) {
      stringifyNodes(entry, destination, 0);
    }
    destination.add("e");
  }
};
} // namespace YAML_Lib
