#pragma once

#include "XML_Translator.hpp"
#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Stringify_Helper.hpp"

namespace YAML_Lib {

class XML_Stringify final : public IStringify {
public:
  explicit XML_Stringify(std::unique_ptr<ITranslator> translator =
                             std::make_unique<XML_Translator>()) {
    xmlTranslator = std::move(translator);
  }
  XML_Stringify(const XML_Stringify &other) = delete;
  XML_Stringify &operator=(const XML_Stringify &other) = delete;
  XML_Stringify(XML_Stringify &&other) = delete;
  XML_Stringify &operator=(XML_Stringify &&other) = delete;
  ~XML_Stringify() override = default;

  /// <summary>
  /// Recursively traverse Node structure encoding it into XML string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">Node structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified XML.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringify(const Node &yNode, IDestination &destination,
                 [[maybe_unused]] const unsigned long indent) const override {
    destination.add(R"(<?xml version="1.0" encoding="UTF-8"?>)");
    destination.add("<root>");
    stringifyNodes(yNode, destination, 0);
    destination.add("</root>");
  }

private:
  static void stringifyNodes(const Node &yNode, IDestination &destination,
                             const long indent) {
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
          stringifyArray(yNode, destination);
        });
  }
  // Intentional parallel to JSON_Stringify/Bencode_Stringify: unwrap Document
  // and recurse. Default_Stringify differs (emits --- / ... markers).
  static void stringifyDocument(const Node &yNode, IDestination &destination,
                                const long indent) {
    stringify_detail::stringifyDocument(yNode, destination, indent, stringifyNodes);
  }
  static void stringifyTimestamp(const Node &yNode, IDestination &destination) {
    destination.add(std::string(NRef<Timestamp>(yNode).value()));
  }
  static void stringifyNumber(const Node &yNode, IDestination &destination) {
    destination.add(stringify_detail::integerToString(yNode));
  }
  static void stringifyString(const Node &yNode, IDestination &destination) {
    destination.add(xmlTranslator->to(NRef<String>(yNode).value()));
  }

  static void stringifyBoolean(const Node &yNode, IDestination &destination) {
    destination.add(NRef<Boolean>(yNode).toString());
  }

  static void stringifyNull([[maybe_unused]] const Node &yNode,
                            [[maybe_unused]] IDestination &destination) {}

  static void stringifyDictionary(const Node &yNode,
                                  IDestination &destination) {
    for (const auto &yNodeNext : NRef<Dictionary>(yNode).value()) {
      std::string elementName{yNodeNext.getKey()};
      std::ranges::replace(elementName, ' ', '-');
      destination.add("<" + elementName + ">");
      stringifyNodes(yNodeNext.getNode(), destination, 0);
      destination.add("</" + elementName + ">");
    }
  }
  static void stringifyArray(const Node &yNode, IDestination &destination) {
    if (NRef<Array>(yNode).value().size() > 1) {
      for (const auto &bNodeNext : NRef<Array>(yNode).value()) {
        destination.add("<Row>");
        stringifyNodes(bNodeNext, destination, 0);
        destination.add("</Row>");
      }
    }
  }

  inline static std::unique_ptr<ITranslator> xmlTranslator;
};

} // namespace YAML_Lib