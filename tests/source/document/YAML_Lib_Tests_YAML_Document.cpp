#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Document API.", "[YAML][Document]") {
  const YAML yaml;
  SECTION("Check YAML Document API.", "[YAML][Document]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Document>(yaml.document(0)));
  }
}