#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of simple types.", "[YAML][Parse][Simple]") {
  const YAML yaml;
  SECTION("YAML Parse a single quoted string.", "[YAML][Parse][String]") {
    BufferSource source{"---\n'test string.'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.root()[0][0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.root()[0][0]).value() ==
            "test string.");
  }
  SECTION("YAML Parse a double quoted string.", "[YAML][Parse][String]") {
    BufferSource source{"---\n\"test string.\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.root()[0][0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.root()[0][0]).value() ==
            "test string.");
  }
  SECTION("YAML Parse a qouted string with some escape sequence in.", "[YAML][Parse][String]") {
    BufferSource source{"---\n\"test string.\\ta tab\\na newline .\\b.\\r.\\f\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.root()[0][0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.root()[0][0]).value() ==
            "test string.\ta tab\na newline .\b.\r.\f");
  }
}