#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML creation.", "[YAML][API]") {
  SECTION("Create YAML.", "[YAML][API][Constructor]") {
    REQUIRE_NOTHROW(YAML());
  }
  SECTION("Get YAML version.", "[YAML][API][Version]") {
    YAML yaml;
    REQUIRE(yaml.version() == "YAML_Lib Version  1.1.0");
    REQUIRE(yaml.getNumberOfDocuments() == 0);
  }
  SECTION("Parse YAML.", "[YAML][API][Parse]") {
    YAML yaml;
    BufferSource buffer("---\n...\n");
    REQUIRE_NOTHROW(yaml.parse(buffer));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }
  SECTION("Create Null YNode.", "[YAML][API][Parse]") {
    YNode yNode = YNode::make<Null>() ;
    REQUIRE_FALSE(!isA<Null>(yNode));
  }
}