#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of boolean types.", "[YAML][Parse][Boolean]") {
  const YAML yaml;
  SECTION("YAML Parse a boolean (True).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nTrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML Parse a boolean (False).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nFalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML Parse a boolean (On).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nOn\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML Parse a boolean (Off).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nOff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML Parse a boolean (Yes).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nYes\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML Parse a boolean (No).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nNo\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
}