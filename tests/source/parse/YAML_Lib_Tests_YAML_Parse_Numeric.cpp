#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of numeric types.", "[YAML][Parse][Scalar][Numeric]") {
  const YAML yaml;
  SECTION("YAML parse an integer.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<int>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == 4567000);
  }
  SECTION("YAML parse an negative integer.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n-4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<int>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == -4567000);
  }
  SECTION("YAML parse an positive integer.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n+4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<int>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == 4567000);
  }
  SECTION("YAML parse a floating point.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<float>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<float>() == 3.141592741f);
  }
  SECTION("YAML parse a positive floating point.", "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n+3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<float>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<float>() == 3.141592741f);
  }
  SECTION("YAML parse a negative floating point.", "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n-3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<float>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<float>() == -3.141592741f);
  }
  SECTION("YAML parse a scientific floating point.", "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n323.777e15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<float>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<float>() ==
            323777002962157568.0f);
  }
  SECTION("YAML parse a small scientific floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<float>());
  }
  SECTION("YAML parse a negative scientific floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n-323.777e15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse a small scientific floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<float>());
  }
  SECTION("YAML parse a small negative scientific floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n-323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<float>());
  }
  SECTION("YAML parse an hexadecimal integer.", "[YAML][Parse][Scalar][Hexadecimal]") {
    BufferSource source{"---\n0x4444\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<int>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == 0x4444);
  }
  SECTION("YAML parse an octal integer.", "[YAML][Parse][Scalar][Octal]") {
    BufferSource source{"---\n04444\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Number>(yaml.document(0)[0]).is<int>());
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == 04444);
  }
  SECTION("YAML parse an integer and float strings.",
          "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n  -  4567000 String\n  - 5.666666 String\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][0]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0][1]));
    REQUIRE(YRef<String>(yaml.document(0)[0][0]).value() == "4567000 String");
    REQUIRE(YRef<String>(yaml.document(0)[0][1]).value() == "5.666666 String");
  }
}