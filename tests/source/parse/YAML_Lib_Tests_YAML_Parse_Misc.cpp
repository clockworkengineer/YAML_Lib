
#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parsing of a list of example YAML files.", "[YAML][Parse][Examples]")
{
  YAML yaml;
  TEST_FILE_LIST(testFile);
//   SECTION("Parse from buffer.", "[YAML][Parse][Examples][Buffer]")
//   {
//     BufferSource yamlSource{ YAML::fromFile(prefixPath(testFile)) };
//     REQUIRE_NOTHROW(yaml.parse(yamlSource));
//     yamlSource.reset();
//     yaml.parse(yamlSource);
//     REQUIRE_FALSE(!(isA<Object>(yaml.root()) || (isA<Array>(yaml.root()))));
//   }
  SECTION("Parse from file directly.", "[YAML][Parse][Examples][File]")
  {
    // FileSource yamlSource{ prefixPath(testFile) };
    // REQUIRE_NOTHROW(yaml.parse(yamlSource));
    // yamlSource.reset();
    // yaml.parse(yamlSource);
    // REQUIRE_FALSE(!(isA<Object>(yaml.root()) || (isA<Array>(yaml.root()))));
  }
}
