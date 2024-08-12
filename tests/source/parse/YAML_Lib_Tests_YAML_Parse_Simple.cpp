#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of simple types.", "[YAML][Parse][Arary]") {
  const YAML yaml;
  SECTION("YAML Parse a boolean (true).", "[YAML][Parse][True]") {
    BufferSource source{"---\ntrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse a boolean (false).", "[YAML][Parse][True]") {
    BufferSource source{"---\ntrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
//   SECTION("YAML Parse an integer.", "[YAML][Parse][Integer]") {
//     BufferSource source{"---\n4567000\n"};
//     REQUIRE_NOTHROW(yaml.parse(source));
//   }
}