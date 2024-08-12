#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parse start document.", "[YAML][Parse][Start]") {
  const YAML yaml;
  SECTION("YAML Parse start document.", "[YAML][Parse][Start]") {
    BufferSource source{"---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }
  SECTION("YAML Parse start/end document.", "[YAML][Parse][StartEnd]") {
    BufferSource source{"---\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }
  SECTION("YAML Parse multiple start document.", "[YAML][Parse][Start]") {
    BufferSource source{"---\n---\n---\n---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 4);
  }
  SECTION("YAML Parse multiple start/end document.",
          "[YAML][Parse][StartEnd]") {
    BufferSource source{"---\n---\n---\n---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 4);
  }
  SECTION("YAML Parse end document.", "[YAML][Parse][End]") {
    BufferSource source{"...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }
  SECTION("YAML Parse multiple end document.", "[YAML][Parse][End]") {
    BufferSource source{"...\n...\n...\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 4);
  }
  SECTION("YAML Parse multiple documents.", "[YAML][Parse][Mutliple]") {
    BufferSource source{
        "---\n...\n---\n...\n---\n...\n---\n...\n---\n...\n---\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 6);
  }
    SECTION("YAML Parse multiple documents formatting mixed up.", "[YAML][Parse][Mutliple]") {
    BufferSource source{
        "---\n...\n---\n...\n---\n...\n...\n---\n...\n---\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 6);
  }
}