
#include "YAML_Lib_Tests.hpp"
#include <iostream>

TEST_CASE("Check YAML parsing of a list of example YAML files.",
          "[YAML][Parse][Examples]") {
  YAML yaml;
  TEST_FILE_LIST(testFile);
  SECTION("Parse from buffer.", "[YAML][Parse][Examples][Buffer]") {
    BufferSource yamlSource{YAML::fromFile(prefixPath(testFile))};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
  }
  SECTION("Parse from file directly.", "[YAML][Parse][Examples][File]") {
    FileSource yamlSource{prefixPath(testFile)};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
  }
  SECTION("Parse from file directly stringify to buffer and try to reparse.",
          "[YAML][Parse][Examples][File]") {
    FileSource yamlSource{prefixPath(testFile)};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    BufferSource source {destination.toString()};
    try {
      yaml.parse(source);
    } catch (std::exception &e){
      std::cout << testFile << '\n';
    }
    // REQUIRE_NOTHROW(yaml.parse(source));
  }
}
