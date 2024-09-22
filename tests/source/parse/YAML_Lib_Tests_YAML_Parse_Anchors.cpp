#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Anchors.", "[YAML][Parse][Anchors]") {
  const YAML yaml;
  SECTION("YAML parse array with one one anchor.", "[YAML][Parse][Anchors]") {
    BufferSource source{
        "----\nhr:\n  - Mark McGwire\n  # Following node labeled SS\n  - &SS "
        "Sammy Sosa\nrbi:\n  - *SS # Subsequent occurance\n  - Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    // REQUIRE_FALSE(!isA<Comment>(yaml.document(0)[1]));
    // REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[2]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("hr"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("rbi"));
  }
}