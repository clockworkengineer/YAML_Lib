#pragma once

#include <memory_resource>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include "implementation/common/YAML_Error.hpp"

namespace YAML_Lib {

/**
 * @brief Encapsulates anchor storage, alias expansion, and recursion guard checks.
 *
 * Implements Single Responsibility Principle (SRP) by isolating &anchor definition,
 * *alias resolution, and expansion limit checking away from syntactical parsing logic.
 */
class AliasResolver {
public:
  using AliasMap = std::pmr::unordered_map<std::string, std::string,
                                          std::hash<std::string_view>,
                                          std::equal_to<>>;
  using AliasSet = std::pmr::set<std::string>;

  explicit AliasResolver(std::pmr::memory_resource *mr = nullptr)
      : aliasMap(mr ? mr : std::pmr::get_default_resource()),
        activeExpansions(mr ? mr : std::pmr::get_default_resource()) {}

  void clear() noexcept {
    aliasMap.clear();
    activeExpansions.clear();
    totalAliases = 0;
  }

  void addAnchor(const std::string &name, const std::string &value, unsigned long maxAliases = 0) {
    if (maxAliases > 0 && totalAliases >= maxAliases) {
      YAML_THROW(Error, "YAML anchor count exceeds configured limit.");
    }
    aliasMap[name] = value;
    ++totalAliases;
  }

  [[nodiscard]] bool contains(const std::string_view &name) const {
    return aliasMap.find(std::string(name)) != aliasMap.end();
  }

  [[nodiscard]] std::string getValue(const std::string_view &name) const {
    auto it = aliasMap.find(std::string(name));
    if (it != aliasMap.end()) {
      return it->second;
    }
    return "";
  }

  [[nodiscard]] bool isExpanding(const std::string_view &name) const {
    return activeExpansions.find(std::string(name)) != activeExpansions.end();
  }

  void enterExpansion(const std::string &name, unsigned long maxExpansions = 0) {
    if (activeExpansions.find(name) != activeExpansions.end()) {
      YAML_THROW(Error, "Recursive anchor detected: '" + name + "'.");
    }
    if (maxExpansions > 0 && activeExpansions.size() >= maxExpansions) {
      YAML_THROW(Error, "YAML alias expansion depth exceeds limit.");
    }
    activeExpansions.insert(name);
  }

  void exitExpansion(const std::string &name) {
    activeExpansions.erase(name);
  }

  [[nodiscard]] std::size_t size() const noexcept {
    return aliasMap.size();
  }

  [[nodiscard]] unsigned long getTotalAliases() const noexcept {
    return totalAliases;
  }

private:
  AliasMap aliasMap;
  AliasSet activeExpansions;
  unsigned long totalAliases{0};
};

} // namespace YAML_Lib
