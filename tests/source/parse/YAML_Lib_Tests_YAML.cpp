#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML creation.", "[YAML][API]") {
  SECTION("Create YAML.", "[YAML][API][Constructor]") {
    REQUIRE_NOTHROW(YAML());
  }
  SECTION("Parse YAML.", "[YAML][API][Parse]") {
    YAML yaml;
    BufferSource buffer("---\n doe: \"a deer, a female deer\"");
    REQUIRE_NOTHROW(yaml.parse(buffer));
  }
  SECTION("Get YAML version.", "[YAML][API][Version]") {
    YAML yaml;
    REQUIRE(yaml.version() == "YAML_Lib Version  0.0.1");
  }
}