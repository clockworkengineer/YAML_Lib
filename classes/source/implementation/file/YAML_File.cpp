#include "YAML_Impl.hpp"

#ifdef YAML_LIB_FILE_IO

#include "implementation/io/YAML_FileIO.hpp"

namespace YAML_Lib {

YAML::Format YAML_Impl::getFileFormat(const std::string_view &fileName) {
  return YAML_FileReader::getFileFormat(fileName);
}

std::string YAML_Impl::fromFile(const std::string_view &fileName) {
  return YAML_FileReader::read(fileName);
}

void YAML_Impl::toFile(const std::string_view &fileName,
                       const std::string_view &yamlString,
                       const YAML::Format format) {
  YAML_FileWriter::write(fileName, yamlString, format);
}

} // namespace YAML_Lib

#endif // YAML_LIB_FILE_IO
