#include <string_view>
#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "implementation/io/YAML_Sources.hpp"
#include "implementation/io/YAML_Destinations.hpp"
#include <fstream>

namespace YAML_Lib {

/// <summary>
/// Function header.
/// </summary>
std::unique_ptr<YAML> YAML::fromString(const std::string_view& yaml_string) {
  return std::make_unique<YAML>(yaml_string);
}

#ifdef YAML_LIB_FILE_IO
/// <summary>
/// Function header.
/// </summary>
std::unique_ptr<YAML> YAML::fromFileToYAML(const std::string_view& file_name) {
  std::string content = YAML::fromFile(file_name);
  return std::make_unique<YAML>(content);
}
#endif

/// <summary>
/// Function header.
/// </summary>
std::string YAML::toString() const {
  BufferDestination dest;
  dest.reserve(4096);
  this->stringify(dest);
  return dest.toString();
}

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#if !defined(YAML_LIB_NO_EXCEPTIONS) && defined(__cpp_exceptions)
std::expected<YAML, std::string> YAML::loadExpected(const std::string_view& yaml_string) {
  try {
    BufferSource source{yaml_string};
    YAML yaml;
    yaml.parse(source);
    return yaml;
  } catch (const std::exception& ex) {
    return std::unexpected(std::string(ex.what()));
  } catch (...) {
    return std::unexpected(std::string("Unknown parse error occurred."));
  }
}

std::expected<YAML, std::string> YAML::loadExpected(ISource& source) {
  try {
    YAML yaml;
    yaml.parse(source);
    return yaml;
  } catch (const std::exception& ex) {
    return std::unexpected(std::string(ex.what()));
  } catch (...) {
    return std::unexpected(std::string("Unknown parse error occurred."));
  }
}

std::expected<void, std::string> YAML::parseExpected(ISource& source) {
  try {
    parse(source);
    return {};
  } catch (const std::exception& ex) {
    return std::unexpected(std::string(ex.what()));
  } catch (...) {
    return std::unexpected(std::string("Unknown parse error occurred."));
  }
}

std::expected<void, std::string> YAML::parseExpected(ISource&& source) {
  return parseExpected(source);
}
#endif
#endif

std::string formatNodeHelper(const Node& node) {
  return node.toString();
}

std::string formatYAMLHelper(const YAML& yaml) {
  return yaml.toString();
}

}  // namespace YAML_Lib
