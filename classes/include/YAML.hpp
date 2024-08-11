#pragma once

#include <algorithm>
#include <cstring>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "YAML_Config.hpp"
#include "YAML_Interfaces.hpp"

namespace YAML_Lib {

// ====================
// Internal String Type
// ====================
using String = std::u16string;
using Char = String::value_type;

// =========================
// YAML forward declarations
// =========================
class YAML_Impl;
// struct JNode;

class YAML {
public:
  YAML();
  YAML(const YAML &other) = delete;
  YAML &operator=(const YAML &other) = delete;
  YAML(YAML &&other) = delete;
  YAML &operator=(YAML &&other) = delete;
  // Provide own destructor
  ~YAML();
  // Get YAML library version
  [[nodiscard]] std::string version() const;
    // Get YAML library version
  [[nodiscard]] unsigned int getNumberOfDocuments() const;
  // Parse YAML into tree
  void parse(ISource &source) const;
  void parse(ISource &&source) const;
  // Create YAML text string from JNode tree (no whitespace)
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;
  // // Create YAML text string from JNode tree (pretty printed)
  // void print(IDestination &destination) const;
  // void print(IDestination &&destination) const;
  // // Strip whitespace from YAML string
  // void strip(ISource &source, IDestination &destination) const;
  // void strip(ISource &source, IDestination &&destination) const;
  // void strip(ISource &&source, IDestination &destination) const;
  // void strip(ISource &&source, IDestination &&destination) const;
  // // Traverse YAML tree
  // void traverse(IAction &action);
  // void traverse(IAction &action) const;
  // // Set print ident value
  // void setIndent(long indent) const;
  // // Get root of YAML tree
  // [[nodiscard]] JNode &root();
  // [[nodiscard]] const JNode &root() const;
  // // Search for YAML object entry with a given key
  // JNode &operator[](const std::string &key);
  // const JNode &operator[](const std::string &key) const;
  // // Get YAML array entry at index
  // JNode &operator[](std::size_t index);
  // const JNode &operator[](std::size_t index) const;
  // // Read/Write YAML from file
  // static std::string fromFile(const std::string &fileName);
  // static void toFile(const std::string &fileName, const std::string &jsonString, Format format = Format::utf8);
  // // Get YAML file format
  // static Format getFileFormat(const std::string &fileName);

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