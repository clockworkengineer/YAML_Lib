#pragma once

#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Translator.hpp"

namespace YAML_Lib {

class JSON_Stringify final : public IStringify
{
public:
  JSON_Stringify() = default;
  JSON_Stringify &operator=(const JSON_Stringify &other) = delete;
  JSON_Stringify(JSON_Stringify &&other) = delete;
  JSON_Stringify &operator=(JSON_Stringify &&other) = delete;
  ~JSON_Stringify() override = default;
    /// <summary>
    /// Recursively traverse YNode structure encoding it into YAML string on
    /// the destination stream passed in.
    /// </summary>
    /// <param name="yNode">YNode structure to be traversed.</param>
    /// <param name="destination">Destination stream for stringified YAML.</param>
    /// <param name="indent">Current print indentation.</param>
    void stringify(const YNode &yNode, IDestination &destination, const unsigned long indent) const override
    {
        if (isA<Document>(yNode)) {
            stringify(YRef<Document>(yNode)[0], destination, indent);
        } else if (isA<Number>(yNode)) {
            destination.add(YRef<Number>(yNode).toString());
        } else if (isA<String>(yNode)) {
            const auto yamlString = YRef<String>(yNode).value();
            destination.add("\""+yamlString+"\"");
        } else if (isA<Boolean>(yNode)) {
            if (YRef<Boolean>(yNode).value()) {
                destination.add("true");
            } else {
                destination.add("false");
            }
        } else if (isA<Null>(yNode)) {
            destination.add("null");
        } else if (isA<Hole>(yNode)) {
        } else if (isA<Dictionary>(yNode)) {
            auto comma=YRef<Dictionary>(yNode).value().size()-1;
            destination.add('{');
            for (auto &entry : YRef<Dictionary>(yNode).value()) {
                stringify(entry.getKeyYNode(), destination, indent);
                destination.add(":");
                stringify(entry.getYNode(), destination, indent);
                if (comma-- > 0) {destination.add(","); }
            }
            destination.add("}");
        } else if (isA<Array>(yNode)) {
            auto comma=YRef<Array>(yNode).value().size()-1;
            destination.add('[');
            for (auto &entry : YRef<Array>(yNode).value()) {
                stringify(entry, destination, indent);
                if (comma-- > 0) {destination.add(","); }
            }
            destination.add("]");
        } else {
            throw Error("Unknown YNode type encountered during stringification.");
        }
    }

private:
 YAML_Translator bencodeTranslator;
};
    
}// namespace YAML_Lib
