#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of boolean types.",
          "[YAML][Parse][Scalar][Boolean]") {
  const YAML yaml;
  SECTION("YAML parse a boolean (True).", "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{"---\nTrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)).value());
  }
  SECTION("YAML parse a boolean (False).", "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{"---\nFalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(NRef<Boolean>(yaml.document(0)).value());
  }
  SECTION("YAML parse a boolean (On).", "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{"---\nOn\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)).value());
  }
  SECTION("YAML parse a boolean (Off).", "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{"---\nOff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(NRef<Boolean>(yaml.document(0)).value());
  }
  SECTION("YAML parse a boolean (Yes).", "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{"---\nYes\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)).value());
  }
  SECTION("YAML parse a boolean (No).", "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{"---\nNo\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(NRef<Boolean>(yaml.document(0)).value());
  }
  SECTION("YAML parse a boolean is not confused with string.",
          "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{
        "---\n  - True Result\n  - False Result\n  - Yes Result\n  - No "
        "Result\n  - On Result\n  - Off Result\n "};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 6);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[1]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[2]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[3]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[4]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[5]));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "True Result");
    REQUIRE(NRef<String>(yaml.document(0)[1]).value() == "False Result");
    REQUIRE(NRef<String>(yaml.document(0)[2]).value() == "Yes Result");
    REQUIRE(NRef<String>(yaml.document(0)[3]).value() == "No Result");
    REQUIRE(NRef<String>(yaml.document(0)[4]).value() == "On Result");
    REQUIRE(NRef<String>(yaml.document(0)[5]).value() == "Off Result");
  }
  SECTION("YAML parse a boolean (True) with trailing space.",
          "[YAML][Parse][Scalar][Boolean]") {
    BufferSource source{"---\nTrue \n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)).value());
  }
}

TEST_CASE("Check YAML Parsing of booleans in strict YAML 1.2 mode.",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
  const YAML yaml;
  SECTION("Strict mode: 'true' parses as boolean true.",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\ntrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)).value());
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'false' parses as boolean false.",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nfalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)));
    REQUIRE_FALSE(NRef<Boolean>(yaml.document(0)).value());
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'Yes' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nYes\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "Yes");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'No' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nNo\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "No");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'On' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nOn\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "On");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'Off' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nOff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "Off");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'yes' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nyes\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "yes");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'no' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nno\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "no");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'on' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\non\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "on");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'off' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\noff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "off");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'True' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nTrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "True");
    YAML::setStrictBooleans(false);
  }
  SECTION("Strict mode: 'False' parses as plain string (not boolean).",
          "[YAML][Parse][Scalar][Boolean][Strict]") {
    YAML::setStrictBooleans(true);
    BufferSource source{"---\nFalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "False");
    YAML::setStrictBooleans(false);
  }
}