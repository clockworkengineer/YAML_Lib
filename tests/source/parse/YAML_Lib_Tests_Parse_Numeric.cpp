#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of numeric types.",
          "[YAML][Parse][Scalar][Numeric]") {
  const YAML yaml;
  SECTION("YAML parse an integer.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<int>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 4567000);
  }
  SECTION("YAML parse an negative integer.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n-4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<int>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == -4567000);
  }
  SECTION("YAML parse an positive integer.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n+4567000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<int>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 4567000);
  }
  SECTION("YAML parse a floating point.", "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<float>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<float>() == 3.141592741f);
  }
  SECTION("YAML parse a positive floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n+3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<float>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<float>() == 3.141592741f);
  }
  SECTION("YAML parse a negative floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n-3.1415926535\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<float>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<float>() == -3.141592741f);
  }
  SECTION("YAML parse a scientific floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n323.777e15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<float>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<float>() ==
            323777002962157568.0f);
  }
  SECTION("YAML parse a small scientific floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<float>());
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
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<float>());
  }
  SECTION("YAML parse a small negative scientific floating point.",
          "[YAML][Parse][Scalar][Float]") {
    BufferSource source{"---\n-323.777e-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<float>());
  }
  SECTION("YAML parse an hexadecimal integer.",
          "[YAML][Parse][Scalar][Hexadecimal]") {
    BufferSource source{"---\n0x4444\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE_FALSE(!NRef<Number>(yaml.document(0)).is<int>());
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 0x4444);
  }
  SECTION("YAML parse YAML 1.2 octal integer (0o prefix).",
          "[YAML][Parse][Scalar][Octal]") {
    // YAML 1.2 core schema: octal is 0o<digits>, not C-style 0NNN.
    BufferSource source{"---\n0o4444\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 04444); // 2340
  }
  SECTION("YAML parse zero-padded decimal (not octal in YAML 1.2).",
          "[YAML][Parse][Scalar][Octal]") {
    // YAML 1.2: "04444" has no 0o prefix, so it is a plain decimal integer.
    BufferSource source{"---\n04444\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 4444);
  }
  SECTION("%YAML 1.1: C-style octal '0777' parses as 511 (octal).",
          "[YAML][Parse][Scalar][Octal][Directive]") {
    // YAML 1.1 schema: 0[0-7]+ is treated as an octal integer.
    BufferSource source{"%YAML 1.1\n---\n0777\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 0777); // 511
  }
  SECTION("No %YAML directive (defaults to 1.2): '0777' parses as decimal 777.",
          "[YAML][Parse][Scalar][Octal][Directive]") {
    // No directive → YAML 1.2 schema → leading zero not significant.
    BufferSource source{"---\n0777\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 777);
  }
  SECTION("%YAML 1.1: C-style octal '0o17' (YAML 1.2 form) still works.",
          "[YAML][Parse][Scalar][Octal][Directive]") {
    // The 0o prefix form is valid in both YAML 1.1 and 1.2 documents.
    BufferSource source{"%YAML 1.1\n---\n0o17\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)));
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 017); // 15
  }
  SECTION("YAML parse an integer and float strings.",
          "[YAML][Parse][Scalar][Integer]") {
    BufferSource source{"---\n  -  4567000 String\n  - 5.666666 String\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[1]));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "4567000 String");
    REQUIRE(NRef<String>(yaml.document(0)[1]).value() == "5.666666 String");
  }
}