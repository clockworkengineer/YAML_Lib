#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of simple types.", "[YAML][Parse][Arary]") {
  const YAML yaml;
  SECTION("YAML Parse a boolean (true).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\ntrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Boolean>(yaml.root()[0][0]).value());
  }
  SECTION("YAML Parse a boolean (false).", "[YAML][Parse][Boolean]") {
    BufferSource source{"---\nfalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Boolean>(yaml.root()[0][0]));
    REQUIRE_FALSE(YAML_Lib::YRef<Boolean>(yaml.root()[0][0]).value());
  }
  SECTION("YAML Parse an integer.", "[YAML][Parse][Integer]") {
    BufferSource source{"---\n4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<int>());
    REQUIRE(YAML_Lib::YRef<Number>(yaml.root()[0][0]).value<int>() == 4567000);
  }
  SECTION("YAML Parse an negative integer.", "[YAML][Parse][Integer]") {
    BufferSource source{"---\n-4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<int>());
    REQUIRE(YAML_Lib::YRef<Number>(yaml.root()[0][0]).value<int>() == -4567000);
  }
  SECTION("YAML Parse an positive integer.", "[YAML][Parse][Integer]") {
    BufferSource source{"---\n+4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<int>());
    REQUIRE(YAML_Lib::YRef<Number>(yaml.root()[0][0]).value<int>() == 4567000);
  }
  SECTION("YAML Parse a floating point.", "[YAML][Parse][Float]") {
    BufferSource source{"---\n3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<float>());
    REQUIRE(YAML_Lib::YRef<Number>(yaml.root()[0][0]).value<float>() ==
            3.141592741f);
  }
  SECTION("YAML Parse a positive floating point.", "[YAML][Parse][Float]") {
    BufferSource source{"---\n+3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<float>());
    REQUIRE(YAML_Lib::YRef<Number>(yaml.root()[0][0]).value<float>() ==
            3.141592741f);
  }
  SECTION("YAML Parse a negative floating point.", "[YAML][Parse][Float]") {
    BufferSource source{"---\n-3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<float>());
    REQUIRE(YAML_Lib::YRef<Number>(yaml.root()[0][0]).value<float>() ==
            -3.141592741f);
  }
  SECTION("YAML Parse a scientific floating point.", "[YAML][Parse][Float]") {
    BufferSource source{"---\n323.777e15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<float>());
    REQUIRE(YAML_Lib::YRef<Number>(yaml.root()[0][0]).value<float>() ==
            323777002962157568.0f);
  }
  SECTION("YAML Parse a small scientific floating point.",
          "[YAML][Parse][Float]") {
    BufferSource source{"---\n323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<float>());
  }
  SECTION("YAML Parse a negative scientific floating point.",
          "[YAML][Parse][Float]") {
    BufferSource source{"---\n-323.777e15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse a small scientific floating point.",
          "[YAML][Parse][Float]") {
    BufferSource source{"---\n323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<float>());
  }
  SECTION("YAML Parse a small negative scientific floating point.",
          "[YAML][Parse][Float]") {
    BufferSource source{"---\n-323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YAML_Lib::YRef<Number>(yaml.root()[0][0]).is<float>());
  }
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
}