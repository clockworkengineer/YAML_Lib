#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of boolean types.", "[YAML][Parse][Boolean]") {
  const YAML yaml;
  SECTION("YAML Parse a boolean (true).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\ntrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.root()[0][0]).value());
  }
  SECTION("YAML Parse a boolean (false).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nfalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.root()[0][0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.root()[0][0]).value());
  }
}