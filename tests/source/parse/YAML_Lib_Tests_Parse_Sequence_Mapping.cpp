#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parse mapping between sequences.",
          "[YAML][Parse][Sequence Mapping]") {
  const YAML yaml;
  SECTION("YAML parse mapping between sequences (one key valur pair).",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "---\n? - Detroit Tigers\n - Chicago cubs\n: - 2001-07-23\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(
        yaml,
        "---\n\"[Detroit Tigers, Chicago cubs]\": \n  - 2001-07-23\n...\n");
  }
  SECTION("YAML parse mapping between sequences (two key value pairs).",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "? - Detroit Tigers\n - Chicago cubs\n: - 2001-07-23\n\n? [ New York "
        "Yankees,\n    Atlanta Braves ]\n: [ 2001-07-02, 2001-08-12,\n    "
        "2001-08-14 ]"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(
        yaml,
        "---\n\"[Detroit Tigers, Chicago cubs]\": \n  - 2001-07-23\n\"[New "
        "York Yankees, Atlanta Braves]\": \n  - 2001-07-02\n  - "
        "2001-08-12\n  - 2001-08-14\n...\n");
  }
  SECTION("YAML parse mapping between sequences (three key value pairs).",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "? - Detroit Tigers\n - Chicago cubs\n: - 2001-07-23\nNew York "
        "Yankees: 2012-08-12\n? [ New York Yankees,\n    Atlanta Braves ]\n: [ "
        "2001-07-02, 2001-08-12,\n    2001-08-14 ]"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(
        yaml,
        "---\n\"[Detroit Tigers, Chicago cubs]\": \n  - 2001-07-23\nNew "
        "York Yankees: 2012-08-12\n\"[New York Yankees, Atlanta Braves]\": "
        "\n  - 2001-07-02\n  - 2001-08-12\n  - 2001-08-14\n...\n");
  }
  SECTION("YAML parse mapping between sequences (two key value pairs and "
          "comments).",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "? # PLAY SCHEDULE\n  - Detroit Tigers\n  - Chicago Cubs\n:\n  - "
        "2001-07-23\n\n? [ New York Yankees,\n    Atlanta Braves ]\n: [ "
        "2001-07-02, 2001-08-12,\n    2001-08-14 ]"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(
        yaml,
        "---\n\"[Detroit Tigers, Chicago Cubs]\": \n  - 2001-07-23\n\"[New "
        "York Yankees, Atlanta Braves]\": \n  - 2001-07-02\n  - "
        "2001-08-12\n  - 2001-08-14\n...\n");
  }
  SECTION("YAML parse mapping (strings) between sequences.",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{"? Mark McGwire\n? Sammy Sosa\n? Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(
        yaml,
        "---\nMark McGwire: null\nSammy Sosa: null\nKen Griffey: null\n...\n");
  }
    SECTION("YAML parse mapping (integers) between sequences.",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{"? 10\n? 20\n? 30"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(
        yaml,
        "---\n\"10\": null\n\"20\": null\n\"30\": null\n...\n");
  }
}