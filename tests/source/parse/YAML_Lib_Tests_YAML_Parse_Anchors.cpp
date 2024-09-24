#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Anchors.", "[YAML][Parse][Anchors]") {
  const YAML yaml;
  SECTION("YAML parse array with one anchor.", "[YAML][Parse][Anchors]") {
    BufferSource source{
        "----\nhr:\n  - Mark McGwire\n  # Following node labeled SS\n  - &SS "
        "Sammy Sosa\nrbi:\n  - *SS # Subsequent occurance\n  - Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("hr"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("rbi"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["hr"][0]).value() ==
            "Mark McGwire");
    REQUIRE_FALSE(!isA<Anchor>(yaml.document(0)[0]["hr"][1]));
    REQUIRE(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).toString() == "Sammy Sosa");
    REQUIRE_FALSE(!isA<String>(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).value()));
    REQUIRE(YRef<String>(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).value()).value() == "Sammy Sosa");
    // REQUIRE(YRef<String>(yaml.document(0)[0]["rbi"][0]).value() ==
    //         "*SS # Subsequent occurance");
    REQUIRE(YRef<String>(yaml.document(0)[0]["rbi"][1]).value() ==
            "Ken Griffey");
  }
}