
#include "YAML_Lib_Tests.hpp"

/// <summary>
/// Prefix path to test data file name.
/// </summary>
/// <param name="yamlFileName">Test YAML data file name</param>
/// <returns>Full path to test data file</returns>
const std::string prefixPath(const std::string &yamlFileName)
{
  return (std::filesystem::current_path() / "files" / yamlFileName).string();
}
