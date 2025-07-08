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
  /// Recursively traverse Node structure encoding it into YAML string on
  /// the destination stream passed in.
  /// </summary>
  /// <param name="yNode">Node structure to be traversed.</param>
  /// <param name="destination">Destination stream for stringified YAML.</param>
  /// <param name="indent">Current print indentation.</param>
  void stringify(const Node &yNode, IDestination &destination,
                 const unsigned long indent) const override {
   stringifyNodes(yNode, destination, indent);
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
                                     const Node &yNode) {
    if (isA<String>(yNode)) {
      if (const auto quote = NRef<String>(yNode).getQuote();
          quote == '>' || quote == '|') {
        destination.add("|");
        destination.add(kLineFeed);
      }
    }
  }
  static void stringifyNodes(const Node &yNode, IDestination &destination,
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
      throw Error("Unknown Node type encountered during stringification.");
    }
  }
  static void stringifyNumber(const Node &yNode, IDestination &destination,
                              [[maybe_unused]] const unsigned long indent) {
    destination.add(NRef<Number>(yNode).toString());
  }
  static void stringifyString(const Node &yNode, IDestination &destination, const unsigned long indent)  {
    if (const char quote = NRef<String>(yNode).getQuote();
     quote == kApostrophe || quote == kDoubleQuote) {
      std::string yamlString{NRef<String>(yNode).toString()};
      if (quote == kDoubleQuote) {
        yamlString = yamlTranslator->to(yamlString);
      }
      destination.add(quote + yamlString + quote);
     } else {
       for (const auto &line :
            splitString(NRef<String>(yNode).toString(), kLineFeed)) {
         destination.add(calculateIndent(destination, indent));
         destination.add(line);
            }
     }
  }
  static void stringifyComment(const Node &yNode, IDestination &destination, [[maybe_unused]]const unsigned long indent) {
    destination.add("#" + std::string(NRef<Comment>(yNode).value()) + kLineFeed);
  }
  static void stringifyBoolean(const Node &yNode, IDestination &destination, [[maybe_unused]]const unsigned long indent) {
    destination.add(NRef<Boolean>(yNode).toString());
  }
  static void stringifyNull(const Node &yNode, IDestination &destination, [[maybe_unused]]const unsigned long indent) {
    destination.add(NRef<Null>(yNode).toString());
  }
  static void stringifyHole(const Node &yNode, IDestination &destination, [[maybe_unused]] const unsigned long indent) {
    destination.add(NRef<Hole>(yNode).toString());
  }
  static void stringifyDictionary(const Node &yNode, IDestination &destination, const unsigned long indent)  {
    for (const auto &entryNode : NRef<Dictionary>(yNode).value()) {
      destination.add(calculateIndent(destination, indent));
      if (const char quote =
              NRef<String>(entryNode.getKeyNode()).getQuote();
          quote == kApostrophe || quote == kDoubleQuote) {
        destination.add(quote +
                        NRef<String>(entryNode.getKeyNode()).toString() +
                        quote);
          } else {
            destination.add(NRef<String>(entryNode.getKeyNode()).toString());
          }
      destination.add(": ");
      stringifyAnyBlockStyle(destination, entryNode.getNode());
      if (isA<Array>(entryNode.getNode()) ||
          isA<Dictionary>(entryNode.getNode())) {
        destination.add(kLineFeed);
          }
      stringifyNodes(entryNode.getNode(), destination, indent + yamlIndentation);
      if (!isA<Array>(entryNode.getNode()) &&
          !isA<Dictionary>(entryNode.getNode()) &&
          !isA<Comment>(entryNode.getNode())) {
        destination.add(kLineFeed);
          }
    }
  }
  static void stringifyArray(const Node &yNode, IDestination &destination, const unsigned long indent)  {
    for (const auto &entryNode : NRef<Array>(yNode).value()) {
      destination.add(calculateIndent(destination, indent) + "- ");
      stringifyAnyBlockStyle(destination, entryNode);
      stringifyNodes(entryNode, destination, indent + yamlIndentation);
      if (destination.last() != kLineFeed) {
        destination.add(kLineFeed);
      }
    }
  }
  static void stringifyDocument(const Node &yNode, IDestination &destination, [[maybe_unused]] const unsigned long indent)  {
    destination.add("---");
    if (!NRef<Document>(yNode).value().empty()) {
      stringifyAnyBlockStyle(destination, NRef<Document>(yNode)[0]);
      if (destination.last() != kLineFeed) {
        destination.add(kLineFeed);
      }
    }
    for (const auto &entryNode : NRef<Document>(yNode).value()) {
      stringifyNodes(entryNode, destination, 0);
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