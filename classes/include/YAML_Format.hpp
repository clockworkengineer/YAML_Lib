#pragma once

/**
 * @file YAML_Format.hpp
 * @brief C++23 std::format / std::formatter support for YAML_Lib::Node and YAML_Lib::YAML.
 */

#include <string>
#include <string_view>

#if defined(__has_include)
  #if __has_include(<format>)
    #include <format>
  #endif
#endif

namespace YAML_Lib {
class YAML;
struct Node;

[[nodiscard]] std::string formatNodeHelper(const Node& node);
[[nodiscard]] std::string formatYAMLHelper(const YAML& yaml);
}  // namespace YAML_Lib

#if defined(__cpp_lib_format) && __cpp_lib_format >= 202110L
template <>
struct std::formatter<YAML_Lib::Node> : std::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(const YAML_Lib::Node& node, FormatContext& ctx) const {
    return std::formatter<std::string_view>::format(YAML_Lib::formatNodeHelper(node), ctx);
  }
};

template <>
struct std::formatter<YAML_Lib::YAML> : std::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(const YAML_Lib::YAML& yaml, FormatContext& ctx) const {
    return std::formatter<std::string_view>::format(YAML_Lib::formatYAMLHelper(yaml), ctx);
  }
};
#endif
