#include "YAML_Lib_Tests.hpp"

//
// COMMENTS THAT DO NOT START A LINE ARE STRIPPED AT PRESENT
//
TEST_CASE("Check YAML Parsing of comments.", "[YAML][parse][Comment]") {
  const YAML yaml;
  SECTION("YAML parse of comments (no documents).", "[YAML][parse][Comment]") {
    BufferSource source{"# Test Comment 1\n# Test Comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 0);
  }
  SECTION("YAML parse of comment.", "[YAML][parse][Comment]") {
    BufferSource source{"---\n   - 'One'\n# Test Comment"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
    REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One");
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n- \'One\'\n...\n");
  }
  SECTION("YAML parse of comments.", "[YAML][parse][Comment]") {
    BufferSource source{"---\n# Comment 1\n   - 'One'\n# Comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
    REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One");
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n- \'One\'\n...\n");
  }
  SECTION("YAML parse of comments and one before start.",
          "[YAML][parse][Comment]") {
    BufferSource source{
        "# Comment 1\n---\n# Comment 2\n   - 'One'\n# Comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
    REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One");
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n- \'One\'\n...\n");
  }
  SECTION("YAML parse of comments and three before start.",
          "[YAML][parse][Comment]") {
    BufferSource source{"# Comment 1\n# Comment 2\n# Comment 3\n---\n# Comment "
                        "4\n   - 'One'\n# Comment 5\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
    REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One");
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n- \'One\'\n...\n");
  }
  SECTION("YAML parse of comments one on the same line as start.",
          "[YAML][parse][Comment]") {
    BufferSource source{"---# Comment 1\n   - 'One'\n# Comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
    REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One");
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n- \'One\'\n...\n");
  }
  SECTION("YAML parse of comment after '>'.", "[YAML][parse][Comment]") {
    BufferSource source{"---\nbar: > # test comment 1\n  this is not a normal "
                        "string it\n  spans more than\n  one line\n see?\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!isA<String>(yaml.root()[0][0]["bar"]));
    REQUIRE(YRef<String>(yaml.root()[0][0]["bar"]).value() ==
            "this is not a normal string it spans more than one line see?");
  }

  // SECTION("YAML parse of comment after '|'.", "[YAML][parse][Comment]") {
  //   BufferSource source{"---\nbar: | # test comment 1\n  this is not a normal
  //   "
  //                       "string it\n  spans more than\n  one line\n see?\n"};
  //   REQUIRE_NOTHROW(yaml.parse(source));
  //   REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
  //   REQUIRE_FALSE(!isA<String>(yaml.root()[0][0]["bar"]));
  //   REQUIRE(YRef<String>(yaml.root()[0][0]["bar"]).value() ==
  //           "this is not a normal string it\nspans more than\none
  //           line\nsee?");
  // }

  // SECTION("YAML parse of comment one same line as a string.",
  //         "[YAML][parse][Comment]") {
  //   BufferSource source{"---\n   - One String   # Comment \n"};
  //   REQUIRE_NOTHROW(yaml.parse(source));
  //   REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
  //   REQUIRE_FALSE(!isA<String>(yaml.root()[0][0][0]));
  //   REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One String");
  // }
  // SECTION("YAML parse of comments one same line array elements.",
  //         "[YAML][parse][Comment]") {
  //   BufferSource source{"---\n   - One String   # Comment \n   - Two String "
  //                       "# Comment \n   - Three String   # Comment \n"};
  //   REQUIRE_NOTHROW(yaml.parse(source));
  //   REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
  //   REQUIRE_FALSE(!isA<String>(yaml.root()[0][0][0]));
  //   REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One String");
  //   REQUIRE(YRef<String>(yaml.root()[0][0][1]).value() == "Two String");
  //   REQUIRE(YRef<String>(yaml.root()[0][0][2]).value() == "Three String");
  // }
}