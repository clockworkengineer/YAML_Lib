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
  SECTION("YAML Parse an integer.", "[YAML][Parse][Integer]") {
    BufferSource source{"---\n4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse an negative integer.", "[YAML][Parse][Integer]") {
    BufferSource source{"---\n-4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse an positive integer.", "[YAML][Parse][Integer]") {
    BufferSource source{"---\n+4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse a floating point.", "[YAML][Parse][Float]") {
    BufferSource source{"---\n3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse a floating point.", "[YAML][Parse][Float]") {
    BufferSource source{"---\n+3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse a negative floating point.", "[YAML][Parse][Float]") {
    BufferSource source{"---\n-3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
}