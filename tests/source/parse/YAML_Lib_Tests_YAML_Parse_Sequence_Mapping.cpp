#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parse mapping between sequences.",
          "[YAML][Parse][Sequence Mapping]") {
  const YAML yaml;
  SECTION("YAML parse mapping between sequences.",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "---\n? - Detroit Tigers\n  - Chicago cubs\n: - 2001-07-23\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\"[Detroit Tigers, Chicago cubs]\": \n  - 2001-07-23\n...\n");
  }
}