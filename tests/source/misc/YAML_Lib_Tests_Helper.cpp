
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

/// <summary>
/// Compare current stringied object.
/// </summary>
/// <param name="yaml">YAML parser instance</param>
/// <returns>Full path to test data file</returns>
void compareYAML( const YAML_Lib::YAML &yaml, const std::string &destinationYAML)
{
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == destinationYAML);
}
