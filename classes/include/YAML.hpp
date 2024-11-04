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

class YAML {
public:
  // Possible YAML Node initializer list types
  using IntializerListTypes =
      std::variant<int, long, long long, float, double, long double, bool,
                   std::string, std::nullptr_t, YNode>;
  // Array initializer list
  using ArrayInitializer = std::initializer_list<IntializerListTypes>;
  // Dictionary initializer list
  using Dictionaryintializer =
      std::initializer_list<std::pair<std::string, IntializerListTypes>>;
  // Pass any user defined parser/stringifier here
  explicit YAML(IStringify *stringify = nullptr, IParser *parser = nullptr);
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
  // Parse YAML into tree
  void parse(ISource &source) const;
  void parse(ISource &&source) const;
  // Create YAML text string from YNode tree (no whitespace)
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;
  // Get root of YAML tree
  [[nodiscard]] std::vector<YNode> &root();
  [[nodiscard]] const std::vector<YNode> &root() const;
  // Get vector of YAML document
  [[nodiscard]] YNode &document(unsigned long index);
  [[nodiscard]] const YNode &document(unsigned long index) const;

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