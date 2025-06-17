#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {

class Default_Stringify final : public IStringify {
public:
  explicit Default_Stringify(std::unique_ptr<ITranslator> translator) {
    yamlTranslator = std::move(translator);
  }
  Default_Stringify(const Default_Stringify &other) = delete;
  Default_Stringify &operator=(const Default_Stringify &other) = delete;
  Default_Stringify(Default_Stringify &&other) = delete;
  Default_Stringify &operator=(Default_Stringify &&other) = delete;
  ~Default_Stringify() override = default;

  /// <summary>
  /// Recursively traverse YNode structure encoding it into YAML string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">YNode structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified YAML.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringify(const YNode &yNode, IDestination &destination,
                 const unsigned long indent) const override {
   stringifyYNodes(yNode, destination, indent);
  }
  // Indentation increment
  static void setIndentation(const unsigned long indentation) {
    yamlIndentation = indentation;
  }

private:
  static std::vector<std::string> splitString(const std::string_view &target,
                                              const char delimiter) {
    std::vector<std::string> splitStrings;
    if (!target.empty()) {
      std::stringstream sourceStream{target.data()};

      for (std::string splitOffItem{};
           std::getline(sourceStream, splitOffItem, delimiter);) {
        splitStrings.push_back(splitOffItem);
        splitStrings.back().push_back(delimiter);
      }
      splitStrings.back().pop_back();
    }
    return splitStrings;
  }
  static std::string  calculateIndent(IDestination &destination,
                                      const unsigned long indent) {
    if (destination.last() == kLineFeed) {
      return std::string(indent, kSpace);
    }
    return std::string("");
  }
  static void stringifyAnyBlockStyle(IDestination &destination,
                                     const YNode &yNode) {
    if (isA<String>(yNode)) {
      if (const auto quote = YRef<String>(yNode).getQuote();
          quote == '>' || quote == '|') {
        destination.add("|");
        destination.add(kLineFeed);
      }
    }
  }
  static void stringifyYNodes(const YNode &yNode, IDestination &destination,
                 const unsigned long indent)  {
    if (isA<Number>(yNode)) {
      stringifyNumber(yNode, destination, indent);
    } else if (isA<String>(yNode)) {
      stringifyString(yNode, destination, indent);
    } else if (isA<Comment>(yNode)) {
      stringifyComment(yNode, destination, indent);
    } else if (isA<Boolean>(yNode)) {
      stringifyBoolean(yNode, destination, indent);
    } else if (isA<Null>(yNode)) {
      stringifyNull(yNode,destination, indent);
    } else if (isA<Hole>(yNode)) {
      stringifyHole(yNode, destination, indent);
    } else if (isA<Dictionary>(yNode)) {
      stringifyDictionary(yNode, destination, indent);
    } else if (isA<Array>(yNode)) {
      stringifyArray(yNode, destination, indent);
    } else if (isA<Document>(yNode)) {
      stringifyDocument(yNode, destination, indent);
    } else {
      throw Error("Unknown YNode type encountered during stringification.");
    }
  }
  static void stringifyNumber(const YNode &yNode, IDestination &destination,
                              [[maybe_unused]] const unsigned long indent) {
    destination.add(YRef<Number>(yNode).toString());
  }
  static void stringifyString(const YNode &yNode, IDestination &destination, const unsigned long indent)  {
    if (const char quote = YRef<String>(yNode).getQuote();
     quote == kApostrophe || quote == kDoubleQuote) {
      std::string yamlString{YRef<String>(yNode).toString()};
      if (quote == kDoubleQuote) {
        yamlString = yamlTranslator->to(yamlString);
      }
      destination.add(quote + yamlString + quote);
     } else {
       for (const auto &line :
            splitString(YRef<String>(yNode).toString(), kLineFeed)) {
         destination.add(calculateIndent(destination, indent));
         destination.add(line);
            }
     }
  }
  static void stringifyComment(const YNode &yNode, IDestination &destination, [[maybe_unused]]const unsigned long indent) {
    destination.add("#" + std::string(YRef<Comment>(yNode).value()) + kLineFeed);
  }
  static void stringifyBoolean(const YNode &yNode, IDestination &destination, [[maybe_unused]]const unsigned long indent) {
    destination.add(YRef<Boolean>(yNode).toString());
  }
  static void stringifyNull(const YNode &yNode, IDestination &destination, [[maybe_unused]]const unsigned long indent) {
    destination.add(YRef<Null>(yNode).toString());
  }
  static void stringifyHole(const YNode &yNode, IDestination &destination, [[maybe_unused]] const unsigned long indent) {
    destination.add(YRef<Hole>(yNode).toString());
  }
  static void stringifyDictionary(const YNode &yNode, IDestination &destination, const unsigned long indent)  {
    for (const auto &entryYNode : YRef<Dictionary>(yNode).value()) {
      destination.add(calculateIndent(destination, indent));
      if (const char quote =
              YRef<String>(entryYNode.getKeyYNode()).getQuote();
          quote == kApostrophe || quote == kDoubleQuote) {
        destination.add(quote +
                        YRef<String>(entryYNode.getKeyYNode()).toString() +
                        quote);
          } else {
            destination.add(YRef<String>(entryYNode.getKeyYNode()).toString());
          }
      destination.add(": ");
      stringifyAnyBlockStyle(destination, entryYNode.getYNode());
      if (isA<Array>(entryYNode.getYNode()) ||
          isA<Dictionary>(entryYNode.getYNode())) {
        destination.add(kLineFeed);
          }
      stringifyYNodes(entryYNode.getYNode(), destination, indent + yamlIndentation);
      if (!isA<Array>(entryYNode.getYNode()) &&
          !isA<Dictionary>(entryYNode.getYNode()) &&
          !isA<Comment>(entryYNode.getYNode())) {
        destination.add(kLineFeed);
          }
    }
  }
  static void stringifyArray(const YNode &yNode, IDestination &destination, const unsigned long indent)  {
    for (const auto &entryYNode : YRef<Array>(yNode).value()) {
      destination.add(calculateIndent(destination, indent) + "- ");
      stringifyAnyBlockStyle(destination, entryYNode);
      stringifyYNodes(entryYNode, destination, indent + yamlIndentation);
      if (destination.last() != kLineFeed) {
        destination.add(kLineFeed);
      }
    }
  }
  static void stringifyDocument(const YNode &yNode, IDestination &destination, [[maybe_unused]] const unsigned long indent)  {
    destination.add("---");
    if (!YRef<Document>(yNode).value().empty()) {
      stringifyAnyBlockStyle(destination, YRef<Document>(yNode)[0]);
      if (destination.last() != kLineFeed) {
        destination.add(kLineFeed);
      }
    }
    for (const auto &entryYNode : YRef<Document>(yNode).value()) {
      stringifyYNodes(entryYNode, destination, 0);
    }
    if (destination.last() != kLineFeed) {
      destination.add(kLineFeed);
    }
    destination.add("...");
    destination.add(kLineFeed);

  }
  // Current indentation level
  inline static unsigned long yamlIndentation{2};
  // Translator
  inline static std::unique_ptr<ITranslator> yamlTranslator;
};

} // namespace YAML_Lib