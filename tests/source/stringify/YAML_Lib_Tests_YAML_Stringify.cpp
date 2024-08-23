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
  SECTION("YAML Stringify document with just an end.", "[YAML][Stringify][End]") {
    BufferSource source{"...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n...\n");
  }
  SECTION("YAML Stringify document with comments.", "[YAML][Stringify][Comments]") {
    BufferSource source{"...\n# comment 1\n# comment 2\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n# comment 1\n# comment 2\n# comment 3\n...\n");
  }
  // SECTION("YAML Stringify document with comments before start.", "[YAML][Stringify][Comments]") {
  //   BufferSource source{"# comment 1\n---\n# comment 2\n# comment 3\n"};
  //   REQUIRE_NOTHROW(yaml.parse(source));
  //   BufferDestination destination;
  //   REQUIRE_NOTHROW(yaml.stringify(destination));
  //   REQUIRE(yaml.getNumberOfDocuments() == 1);
  //   REQUIRE(destination.toString() == "");
  // }
}