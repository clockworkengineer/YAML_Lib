#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parse start document.", "[YAML][Parse][Start]") {
  const YAML yaml;
  SECTION("YAML parse start document.", "[YAML][Parse][Start]") {
    BufferSource source{"---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }
  SECTION("YAML parse start/end document.", "[YAML][Parse][StartEnd]") {
    BufferSource source{"---\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }
  SECTION("YAML parse multiple start document.", "[YAML][Parse][Start]") {
    BufferSource source{"---\n---\n---\n---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 4);
  }
  SECTION("YAML parse multiple start/end document.",
          "[YAML][Parse][StartEnd]") {
    BufferSource source{"---\n---\n---\n---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 4);
  }
  SECTION("YAML parse end document.", "[YAML][Parse][End]") {
    BufferSource source{"...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }
  SECTION("YAML parse multiple end document.", "[YAML][Parse][End]") {
    BufferSource source{"...\n...\n...\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 4);
  }
  SECTION("YAML parse multiple document.", "[YAML][Parse][Mutliple]") {
    BufferSource source{
        "---\n...\n---\n...\n---\n...\n---\n...\n---\n...\n---\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 6);
  }
  SECTION("YAML parse multiple document formatting mixed up.",
          "[YAML][Parse][Mutliple]") {
    BufferSource source{
        "---\n...\n---\n...\n---\n...\n...\n---\n...\n---\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 6);
  }
  SECTION("YAML parse document with comments before start.",
          "[YAML][Parse][Comments]") {
    BufferSource source{"# comment 1\n---\n# comment 2\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }

  SECTION("YAML parse document that is spli in two documents by an end.",
          "[YAML][Parse][Comments]") {
    BufferSource source{"---\ntime : 1\n...\ntime: 2"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 2);
  }
}