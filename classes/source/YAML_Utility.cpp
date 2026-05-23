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
std::unique_ptr<YAML> YAML::fromString(const std::string_view &yaml_string) {
    return std::make_unique<YAML>(yaml_string);
}

#ifdef YAML_LIB_FILE_IO
/// <summary>
/// Function header.
/// </summary>
std::unique_ptr<YAML> YAML::fromFileToYAML(const std::string_view &file_name) {
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

} // namespace YAML_Lib
