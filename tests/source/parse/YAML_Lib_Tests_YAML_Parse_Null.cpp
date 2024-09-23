#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parse of Null/~ .", "[YAML][Parse][Null/~]") {
  const YAML yaml;
  SECTION("YAML parse of Null/~.", "[YAML][Parse][Null]") {
    BufferSource source{"---\nnull\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[0]));
  }
  SECTION("YAML parse of ~.", "[YAML][Parse][~]") {
    BufferSource source{"---\n~\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[0]));
  }
  SECTION("YAML parse of 'null string'.", "[YAML][Parse][Null]") {
    BufferSource source{"---\nnull string\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(isA<Null>(yaml.document(0)[0]));
  }
  SECTION("YAML parse of '~ string'.", "[YAML][Parse][~]") {
    BufferSource source{"---\n~ string \n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(isA<Null>(yaml.document(0)[0]));
  }
}