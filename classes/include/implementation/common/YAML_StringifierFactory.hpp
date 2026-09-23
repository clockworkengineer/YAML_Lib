#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include "interface/IStringify.hpp"

namespace YAML_Lib {

enum class StringifyFormat {
  YAML,
  JSON,
  XML,
  Bencode
};

/**
 * @brief Factory and registry strategy for creating IStringify instances.
 *
 * Implements Open/Closed Principle (OCP) and Dependency Inversion Principle (DIP).
 * Allows registering custom stringifiers without modifying internal format switch statements.
 */
class StringifierFactory {
public:
  using Creator = std::function<std::unique_ptr<IStringify>()>;

  static StringifierFactory &instance();

  void registerCreator(StringifyFormat format, Creator creator);
  void registerCreator(std::string_view formatName, Creator creator);

  [[nodiscard]] std::unique_ptr<IStringify> create(StringifyFormat format) const;
  [[nodiscard]] std::unique_ptr<IStringify> create(std::string_view formatName) const;

private:
  StringifierFactory();
  void registerDefaults();
  std::unordered_map<StringifyFormat, Creator> creators;
  std::unordered_map<std::string, Creator> namedCreators;
};

} // namespace YAML_Lib
