#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Stringify_Helper.hpp"

namespace YAML_Lib {

class JSON_Stringify final : public IStringify {
public:
  explicit JSON_Stringify(std::unique_ptr<ITranslator> translator =
                              std::make_unique<Default_Translator>()) {
    jsonTranslator = std::move(translator);
  }
  JSON_Stringify &operator=(const JSON_Stringify &other) = delete;
  JSON_Stringify(JSON_Stringify &&other) = delete;
  JSON_Stringify &operator=(JSON_Stringify &&other) = delete;
  ~JSON_Stringify() override = default;

  /// <summary>
  /// Recursively traverse Node structure encoding it into JSON string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">Node structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified JSON.</param>
  /// <param name="indent">Current print indentation.</param>
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

  // Intentional parallel to XML_Stringify/Bencode_Stringify: unwrap Document
  // and recurse. Default_Stringify differs (emits --- / ... markers).
  static void stringifyDocument(const Node &yNode, IDestination &destination,
                                const long indent) {
    stringify_detail::stringifyDocument(yNode, destination, indent, stringifyNodes);
  }
  static void stringifyNumber(const Node &yNode, IDestination &destination) {
    destination.add(NRef<Number>(yNode).toString());
  }
  static void stringifyString(const Node &yNode, IDestination &destination) {
    const std::string_view yamlString = NRef<String>(yNode).value();
    destination.add("\"" + jsonTranslator->to(yamlString) + "\"");
  }
  static void stringifyBoolean(const Node &yNode, IDestination &destination) {
    stringify_detail::addBooleanLiteral(destination,
                                        NRef<Boolean>(yNode).value(),
                                        "true", "false");
  }
  static void stringifyNull([[maybe_unused]] const Node &yNode,
                            IDestination &destination) {
    destination.add("null");
  }
  static void stringifyTimestamp(const Node &yNode, IDestination &destination) {
    destination.add('"');
    destination.add(std::string(NRef<Timestamp>(yNode).value()));
    destination.add('"');
  }
  static void stringifyDictionary(const Node &yNode,
                                  IDestination &destination) {
    const auto &entries = NRef<Dictionary>(yNode).value();
    stringify_detail::addDelimited(
        destination, '{', '}', entries.size(), ",",
        [&](const std::size_t index) {
          const auto &entry = entries[index];
          destination.add('"' + jsonTranslator->to(entry.getKey()) + '"');
          destination.add(":");
          stringifyNodes(entry.getNode(), destination, 0);
        });
  }
  static void stringifyAray(const Node &yNode, IDestination &destination) {
    const auto &entries = NRef<Array>(yNode).value();
    stringify_detail::addDelimited(
        destination, '[', ']', entries.size(), ",",
        [&](const std::size_t index) {
          stringifyNodes(entries[index], destination, 0);
        });
  }

  inline static std::unique_ptr<ITranslator> jsonTranslator;
};

} // namespace YAML_Lib
