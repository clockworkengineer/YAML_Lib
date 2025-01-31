#pragma once

#include <algorithm>
#include <cstring>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "YAML_Config.hpp"
#include "YAML_Interfaces.hpp"

namespace YAML_Lib {

// =========================
// YAML forward declarations
// =========================
class YAML_Impl;
struct YNode;

// ========================
// YAML character constants
// ========================
constexpr char kNull{0x00};
constexpr char kCarriageReturn{0x0D};
constexpr char kLineFeed{0x0A};
constexpr char kSpace{0x20};
constexpr char kDoubleQuote{0x22};
constexpr char kApostrophe{0x27};
constexpr char kComma{0x2C};
constexpr char kColon{0x3A};
constexpr char kLeftSquareBracket{0x5B};
constexpr char kRightSquareBracket{0x5D};
constexpr char kLeftCurlyBrace{0x7B};
constexpr char kRightCurlyBrace{0x7D};

// =====================
// YAML string constants
// =====================
constexpr auto kOverride{"<<"};
constexpr auto kStartDocument{"---"};
constexpr auto kEndDocument{"..."};

class YAML {
public:
  // Possible YAML Node initializer list types
  using InitializerListTypes =
      std::variant<int, long, long long, float, double, long double, bool,
                   std::string, std::nullptr_t, YNode>;
  // Array initializer list
  using ArrayInitializer = std::initializer_list<InitializerListTypes>;
  // Dictionary initializer list
  using DictionaryInitializer =
      std::initializer_list<std::pair<std::string, InitializerListTypes>>;
  // Pass any user defined parser/stringifier here
  explicit YAML(IStringify *stringify = nullptr, IParser *parser = nullptr);
  // Pass in default YAML to parse
  explicit YAML(const std::string &yamlString);
  // Construct an array
  YAML(const ArrayInitializer &array);
  // Construct object
  YAML(const DictionaryInitializer &dictionary);
  YAML(const YAML &other) = delete;
  YAML &operator=(const YAML &other) = delete;
  YAML(YAML &&other) = delete;
  YAML &operator=(YAML &&other) = delete;
  // Provide own destructor
  ~YAML();
  // Get YAML library version
  [[nodiscard]] static std::string version();
  // Get YAML library version
  [[nodiscard]] unsigned int getNumberOfDocuments() const;
  // Parse YAML into a tree
  void parse(ISource &source) const;
  void parse(ISource &&source) const;
  // Create YAML text string from YNode tree (no whitespace)
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;
  // Get the root of YAML tree
  [[nodiscard]] std::vector<YNode> &root();
  [[nodiscard]] const std::vector<YNode> &root() const;
  // Get vector of YAML document
  [[nodiscard]] YNode &document(unsigned long index);
  [[nodiscard]] const YNode &document(unsigned long index) const;
  // Traverse YAML tree
  void traverse(IAction &action);
  void traverse(IAction &action) const;
  // Search for YAML object entry with a given key
  YNode &operator[](const std::string &key);
  const YNode &operator[](const std::string &key) const;
  // Get YAML array entry at index
  YNode &operator[](std::size_t index);
  const YNode &operator[](std::size_t index) const;

  static std::string fromFile(const std::string &yamlFileName) {
    std::ifstream yamlFile{yamlFileName, std::ios_base::binary};
    std::ostringstream yamlFileBuffer;
    yamlFileBuffer << yamlFile.rdbuf();
    return yamlFileBuffer.str();
  }

private:
  // YAML implementation
  const std::unique_ptr<YAML_Impl> implementation;
};

} // namespace YAML_Lib