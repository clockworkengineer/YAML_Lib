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

  SECTION("YAML parse document that is split in two documents by an end.",
          "[YAML][Parse][Split]") {
    BufferSource source{"---\ntime : 1\n...\ntime: 2"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 2);
  }

  SECTION("YAML parse dictionary with null for value entry.", "[YAML][Parse][Null]") {
    BufferSource source{"null:\nbooleans: [ true, false ]\nstring: \'012345\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("null"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("booleans"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("string"));
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[0]["null"]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["string"]));
  }
}