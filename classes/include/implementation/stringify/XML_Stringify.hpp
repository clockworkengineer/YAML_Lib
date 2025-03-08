#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "XML_Translator.hpp"

namespace YAML_Lib {

class XML_Stringify : public IStringify {
public:
  XML_Stringify() = default;
  XML_Stringify(const XML_Stringify &other) = delete;
  XML_Stringify &operator=(const XML_Stringify &other) = delete;
  XML_Stringify(XML_Stringify &&other) = delete;
  XML_Stringify &operator=(XML_Stringify &&other) = delete;
  ~XML_Stringify() override = default;

  /// <summary>
  /// Recursively traverse YNode structure encoding it into YAML string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">YNode structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified YAML.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringify(const YNode &yNode, IDestination &destination, [[maybe_unused]] const unsigned long indent) const override
  {
    destination.add(R"(<?xml version="1.0" encoding="UTF-8"?>)");
    destination.add("<root>");
    stringifyXML(yNode, destination, 0);
    destination.add("</root>");
  }

private:
  /// <summary>
  /// Recursively traverse YNode structure encoding it into XML string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">YNode structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified XML.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringifyXML(const YNode &yNode, IDestination &destination, const long indent) const
  {
    if (isA<Document>(yNode)) {
      stringifyDocument(yNode, destination, indent);
    } else if (isA<Number>(yNode)) {
      stringifyNumber(yNode, destination);
    } else if (isA<String>(yNode)) {
      stringifyString(yNode, destination);
    } else if (isA<Boolean>(yNode)) {
      stringifyBoolean(yNode, destination);
    } else if (isA<Null>(yNode)||isA<Hole>(yNode)) {
    } else if (isA<Dictionary>(yNode)) {
      stringifyDictionary(yNode, destination);
    } else if (isA<Array>(yNode)) {
      stringifyArray(yNode, destination);
    } else {
      throw Error("Unknown YNode type encountered during stringification.");
    }
  }

  void stringifyDocument(const YNode &yNode, IDestination &destination, const long indent) const {
    stringifyXML(YRef<Document>(yNode)[0], destination, indent);
  }

  void stringifyNumber(const YNode &yNode, IDestination &destination) const {
    destination.add(std::to_string(YRef<Number>(yNode).value<long long>()));
  }

  void stringifyString(const YNode &yNode, IDestination &destination) const {
    destination.add(xmlTranslator.to(YRef<String>(yNode).value()));
  }

  void stringifyBoolean(const YNode &yNode, IDestination &destination) const {
    if (YRef<Boolean>(yNode).value()) {
      destination.add("True");
    } else {
      destination.add("False");
    }
  }

  void stringifyNull(const YNode &yNode, IDestination &destination) const {

  }

  void stringifyDictionary(const YNode &yNode, IDestination &destination) const {
    for (const auto &yNodeNext : YRef<Dictionary>(yNode).value()) {
      auto elementName = yNodeNext.getKey();
      std::ranges::replace(elementName, ' ', '-');
      destination.add("<" + elementName + ">");
      stringifyXML(yNodeNext.getYNode(), destination, 0);
      destination.add("</" + elementName + ">");
    }

  }
  void stringifyArray(const YNode &yNode, IDestination &destination) const {
    if (YRef<Array>(yNode).value().size() > 1) {
      for (const auto &bNodeNext : YRef<Array>(yNode).value()) {
        destination.add("<Row>");
        stringifyXML(bNodeNext, destination, 0);
        destination.add("</Row>");
      }
    }
  }

  // void stringifyDocument(const YNode &yNode, IDestination &destination, const long indent) const {
  //
  // }
  //
  // void stringifyNumber(const YNode &yNode, IDestination &destination) const {
  //
  // }
  //
  // void stringifyString(const YNode &yNode, IDestination &destination) const {
  //
  // }
  //
  // void stringifyBoolean(const YNode &yNode, IDestination &destination) const {
  //
  // }
  //
  // void stringifyNull(const YNode &yNode, IDestination &destination) const {
  //
  // }
  //
  // void stringifyDictionary(const YNode &yNode, IDestination &destination) const {
  //
  // }
  // void stringifyAray(const YNode &yNode, IDestination &destination) const {
  //
  // }

  XML_Translator xmlTranslator;

};

}// namespace YAML_Lib