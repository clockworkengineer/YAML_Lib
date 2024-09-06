#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Document API.", "[YAML][Document]") {
  const YAML yaml;
  SECTION("Check YAML Document API for non-existant document.", "[YAML][Document]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Document>(yaml.document(0)));
  }
    SECTION("Check YAML Document API.", "[YAML][Document]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_THROWS_WITH(!isA<Document>(yaml.document(1)), "YAML Error: Document does not exist.");
  }
}