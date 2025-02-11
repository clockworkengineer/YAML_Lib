#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Document API.", "[YAML][Document]") {
  const YAML yaml;
  SECTION("Check YAML Document API for a document.", "[YAML][Document]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
  }
  SECTION("Check YAML Document API doe non-existent document.",
          "[YAML][Document]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_THROWS_WITH(!isA<Document>(yaml.document(1)),
                        "YAML Error: Document does not exist.");
  }
  SECTION("Check YAML Document API for two documents.", "[YAML][Document]") {
    BufferSource source{"---\n   - 'One'\n...\n---\n   - 'Two'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 2);
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Array>(yaml.document(1)));
  }
}