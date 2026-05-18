//// Program: YAML_Custom_IO
//
// Description: Demonstrates how to extend YAML_Lib with a custom parser,
// custom stringifier, and custom output destination using the public interfaces.
//
// Dependencies: C++20, YAML_Lib.
//

#include <iostream>
#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace yl = YAML_Lib;

struct CustomDestination : yl::IDestination {
  std::string output;
  void add(char ch) override { output.push_back(ch); }
  void clear() override { output.clear(); }
  char last() override { return output.empty() ? '\0' : output.back(); }
};

struct PrefixStringify : yl::IStringify {
  void stringify(const yl::Node &yNode, yl::IDestination &destination,
                 unsigned long) const override {
    destination.add('[');
    if (yl::isA<yl::Dictionary>(yNode)) {
      destination.add("DICT]");
    } else if (yl::isA<yl::Array>(yNode)) {
      destination.add("ARRAY]");
    } else {
      destination.add("NODE]");
    }
  }
};

struct ConstantParser : yl::IParser {
  std::vector<yl::Node> parse(yl::ISource &) override {
    std::vector<yl::Node> result;
    result.emplace_back(yl::Node{{{"custom", "parser"}}});
    return result;
  }
};

int main() {
  yl::Options options;
  options.parser = new ConstantParser();
  options.stringifier = yl::makeStringify<PrefixStringify>();

  yl::YAML yaml(options);
  yaml.parse(yl::BufferSource{"---\nignored: yes\n"});

  CustomDestination dest;
  yaml.stringify(dest);

  std::cout << "Custom parser produced: " << dest.output << "\n";
  return 0;
}
