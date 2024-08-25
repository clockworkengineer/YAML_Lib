#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML stringify.", "[YAML][Stringify]") {
  const YAML yaml;
  SECTION("YAML Stringify document a start.", "[YAML][Stringify][Start]") {
    BufferSource source{"---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n...\n");
  }
  SECTION("YAML Stringify document with just an end.",
          "[YAML][Stringify][End]") {
    BufferSource source{"...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n...\n");
  }
  SECTION("YAML Stringify document with comments.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"...\n# comment 1\n# comment 2\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n# comment 1\n# comment 2\n# comment 3\n...\n");
  }
    SECTION("YAML Stringify document one with integer.", "[YAML][Stringify][Integer]") {
    BufferSource source{"---\n65000"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n65000...\n");
  }
  SECTION("YAML Stringify document one string.", "[YAML][Stringify][String]") {
    BufferSource source{"---\n\" test string \""};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\" test string \"...\n");
  }
    SECTION("YAML Stringify document one boolean.", "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\ntrue"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\ntrue...\n");
  }
  // SECTION("YAML Stringify document with comments before start.",
  // "[YAML][Stringify][Comments]") {
  //   BufferSource source{"# comment 1\n---\n# comment 2\n# comment 3\n"};
  //   REQUIRE_NOTHROW(yaml.parse(source));
  //   BufferDestination destination;
  //   REQUIRE_NOTHROW(yaml.stringify(destination));
  //   // REQUIRE(yaml.getNumberOfDocuments() == 1);
  //   REQUIRE(destination.toString() == "");
  // }
  // SECTION("YAML Parse array with multiple string elements and restringify.",
  //         "[YAML][Parse][Array]") {
  //   BufferSource source{
  //       "---\n   - 'One'\n  - 'Two'\n  - 'Three'\n  - 'Four'\n"};
  //   REQUIRE_NOTHROW(yaml.parse(source));
  //   BufferDestination destination;
  //   REQUIRE_NOTHROW(yaml.stringify(destination));
  //   REQUIRE(destination.toString() == "");
  // }
}