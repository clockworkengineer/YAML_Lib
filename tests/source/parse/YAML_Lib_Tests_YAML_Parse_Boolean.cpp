#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of boolean types.", "[YAML][Parse][Boolean]") {
  const YAML yaml;
  SECTION("YAML parse a boolean (True).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nTrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML parse a boolean (False).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nFalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML parse a boolean (On).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nOn\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML parse a boolean (Off).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nOff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML parse a boolean (Yes).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nYes\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML parse a boolean (No).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nNo\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("YAML parse a boolean is not confused with string.",
          "[YAML][Parse][Boolean]") {
    BufferSource source{
        "---\n  - True Result\n  - False Result\n  - Yes Result\n  - No "
        "Result\n  - On Result\n  - Off Result\n "};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 6);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][0]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][1]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][2]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][3]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][4]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][5]));
    REQUIRE(YRef<String>(yaml.document(0)[0][0]).value() == "True Result");
    REQUIRE(YRef<String>(yaml.document(0)[0][1]).value() == "False Result");
    REQUIRE(YRef<String>(yaml.document(0)[0][2]).value() == "Yes Result");
    REQUIRE(YRef<String>(yaml.document(0)[0][3]).value() == "No Result");
    REQUIRE(YRef<String>(yaml.document(0)[0][4]).value() == "On Result");
    REQUIRE(YRef<String>(yaml.document(0)[0][5]).value() == "Off Result");
  }
  SECTION("YAML parse a boolean (True) with trailing space.",
          "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nTrue \n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.document(0)[0]).value());
  }
}