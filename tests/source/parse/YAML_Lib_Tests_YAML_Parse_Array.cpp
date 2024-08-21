#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Arrays.", "[YAML][Parse][Arary]") {
  const YAML yaml;
  SECTION("YAML Parse array with one element.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse array with two elements.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n   - 'Two'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse array with one element and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0]));
    REQUIRE(YRef<Array>(yaml.root()[0]).size() == 1);
    // REQUIRE(YRef<String>(yaml.root()[0][0]).value() == "");
  }
}