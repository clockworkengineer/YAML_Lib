#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parse mapping between sequences.",
          "[YAML][Parse][Sequence Mapping]") {
  const YAML yaml;
  SECTION("YAML parse mapping between sequences (one key valur pair).",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "---\n? - Detroit Tigers\n  - Chicago cubs\n: - 2001-07-23\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n\"[Detroit Tigers, Chicago cubs]\": \n  - 2001-07-23\n...\n");
  }
  SECTION("YAML parse mapping between sequences (two key value pairs).",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "? - Detroit Tigers\n  - Chicago cubs\n: - 2001-07-23\n\n? [ New York "
        "Yankees,\n    Atlanta Braves ]\n: [ 2001-07-02, 2001-08-12,\n    "
        "2001-08-14 ]"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n\"[Detroit Tigers, Chicago cubs]\": \n  - 2001-07-23\n\"[New "
            "York Yankees, Atlanta Braves]\": \n  - 2001-07-02\n  - "
            "2001-08-12\n  - 2001-08-14\n...\n");
  }
  SECTION("YAML parse mapping between sequences (three key value pairs).",
          "[YAML][Parse][Sequence Mapping]]") {
    BufferSource source{
        "? - Detroit Tigers\n  - Chicago cubs\n: - 2001-07-23\nNew York "
        "Yankees: 2012-08-12\n? [ New York Yankees,\n    Atlanta Braves ]\n: [ "
        "2001-07-02, 2001-08-12,\n    2001-08-14 ]"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n\"[Detroit Tigers, Chicago cubs]\": \n  - 2001-07-23\nNew "
            "York Yankees: 2012-08-12\n\"[New York Yankees, Atlanta Braves]\": "
            "\n  - 2001-07-02\n  - 2001-08-12\n  - 2001-08-14\n...\n");
  }
  //   SECTION("YAML parse mapping between sequences (two key value pairs and "
//           "comments).",
//           "[YAML][Parse][Sequence Mapping]]") {
//     BufferSource source{
//         "? # PLAY SCHEDULE\n  - Detroit Tigers\n  - Chicago Cubs\n:\n  - "
//         "2001-07-23\n\n? [ New York Yankees,\n    Atlanta Braves ]\n: [ "
//         "2001-07-02, 2001-08-12,\n    2001-08-14 ]"};
//     REQUIRE_NOTHROW(yaml.parse(source));
//     REQUIRE(yaml.getNumberOfDocuments() == 1);
//     REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
//     BufferDestination destination;
//     REQUIRE_NOTHROW(yaml.stringify(destination));
//     REQUIRE(destination.toString() ==
//             "---\n\"[Detroit Tigers, Chicago Cubs]\": \n  - 2001-07-23\n\"[New "
//             "York Yankees, Atlanta Braves]\": \n  - 2001-07-02\n  - "
//             "2001-08-12\n  - 2001-08-14\n...\n");
//   }
}