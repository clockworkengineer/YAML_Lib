#pragma once

#ifdef YAML_LIB_FILE_IO

#include <fstream>
#include <string>
#include <string_view>
#include "YAML.hpp"

namespace YAML_Lib {

/**
 * @brief Extracted single-responsibility component for reading and detecting YAML file formats.
 *
 * Implements Single Responsibility Principle (SRP) by decoupling file system stream reading
 * and BOM detection from the core YAML_Impl facade.
 */
class YAML_FileReader {
public:
  [[nodiscard]] static YAML::Format getFileFormat(const std::string_view &fileName);
  [[nodiscard]] static std::string read(const std::string_view &fileName);
};

/**
 * @brief Extracted single-responsibility component for writing formatted YAML strings to disk.
 *
 * Implements Single Responsibility Principle (SRP) by isolating file system stream writing
 * and character encoding serialization.
 */
class YAML_FileWriter {
public:
  static void write(const std::string_view &fileName,
                    const std::string_view &yamlString,
                    YAML::Format format);
};

} // namespace YAML_Lib

#endif // YAML_LIB_FILE_IO
