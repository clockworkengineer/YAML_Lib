#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "XML_Translator.hpp"

namespace YAML_Lib {

class XML_Stringify final : public IStringify {
public:
  explicit XML_Stringify(std::unique_ptr<ITranslator> translator =
                            std::make_unique<XML_Translator>())
  {xmlTranslator = std::move(translator);}
  XML_Stringify(const XML_Stringify &other) = delete;
  XML_Stringify &operator=(const XML_Stringify &other) = delete;
  XML_Stringify(XML_Stringify &&other) = delete;
  XML_Stringify &operator=(XML_Stringify &&other) = delete;
  ~XML_Stringify() override = default;

  /// <summary>
  /// Recursively traverse YNode structure encoding it into XML string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">YNode structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified XML.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringify(const YNode &yNode, IDestination &destination,
                 [[maybe_unused]] const unsigned long indent) const override {
    destination.add(R"(<?xml version="1.0" encoding="UTF-8"?>)");
    destination.add("<root>");
    stringifyYNodes(yNode, destination, 0);
    destination.add("</root>");
  }

private:
  static void stringifyYNodes(const YNode &yNode, IDestination &destination,
                    const long indent)  {
    if (isA<Document>(yNode)) {
      stringifyDocument(yNode, destination, indent);
    } else if (isA<Number>(yNode)) {
      stringifyNumber(yNode, destination);
    } else if (isA<String>(yNode)) {
      stringifyString(yNode, destination);
    } else if (isA<Boolean>(yNode)) {
      stringifyBoolean(yNode, destination);
    } else if (isA<Null>(yNode) || isA<Hole>(yNode)) {
    } else if (isA<Dictionary>(yNode)) {
      stringifyDictionary(yNode, destination);
    } else if (isA<Array>(yNode)) {
      stringifyArray(yNode, destination);
    } else {
      throw Error("Unknown YNode type encountered during stringification.");
    }
  }
  static void stringifyDocument(const YNode &yNode, IDestination &destination,
                         const long indent)  {
    stringifyYNodes(YRef<Document>(yNode)[0], destination, indent);
  }
  static void stringifyNumber(const YNode &yNode, IDestination &destination) {
    destination.add(std::to_string(YRef<Number>(yNode).value<long long>()));
  }
  static void stringifyString(const YNode &yNode, IDestination &destination)  {
    destination.add(xmlTranslator->to(YRef<String>(yNode).value()));
  }

  static void stringifyBoolean(const YNode &yNode, IDestination &destination) {
    if (YRef<Boolean>(yNode).value()) {
      destination.add("True");
    } else {
      destination.add("False");
    }
  }

  static void stringifyNull([[maybe_unused]] const YNode &yNode,
                            [[maybe_unused]] IDestination &destination) {}

  static void stringifyDictionary(const YNode &yNode,
                           IDestination &destination) {
    for (const auto &yNodeNext : YRef<Dictionary>(yNode).value()) {
      std::string elementName { yNodeNext.getKey()};
      std::ranges::replace(elementName, ' ', '-');
      destination.add("<" + elementName + ">");
      stringifyYNodes(yNodeNext.getYNode(), destination, 0);
      destination.add("</" + elementName + ">");
    }
  }
  static void stringifyArray(const YNode &yNode, IDestination &destination)  {
    if (YRef<Array>(yNode).value().size() > 1) {
      for (const auto &bNodeNext : YRef<Array>(yNode).value()) {
        destination.add("<Row>");
        stringifyYNodes(bNodeNext, destination, 0);
        destination.add("</Row>");
      }
    }
  }

  inline static std::unique_ptr<ITranslator> xmlTranslator;
};

} // namespace YAML_Lib