#include "YAML_Lib_Tests.hpp"

//
// COMMENTS THAT DO NOT START A LINE ARE STRIPPED AT PRESENT
//
TEST_CASE("Check YAML Parsing of comments.", "[YAML][parse][Comment]") {
  const YAML yaml;
  SECTION("YAML parse of comment.", "[YAML][parse][Comment]") {
    BufferSource source{"---\n   - 'One'\n# Test Comment"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0][1]).value() == " Test Comment");
  }
  SECTION("YAML parse of comments.", "[YAML][parse][Comment]") {
    BufferSource source{"---\n# Comment 1\n   - 'One'\n# Comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0][0]).value() == " Comment 1");
    REQUIRE(YRef<Comment>(yaml.root()[0][2]).value() == " Comment 2");
  }
  SECTION("YAML parse of comments and one before start.",
          "[YAML][parse][Comment]") {
    BufferSource source{
        "# Comment 1\n---\n# Comment 2\n   - 'One'\n# Comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0]).value() == " Comment 1");
    REQUIRE(YRef<Comment>(yaml.root()[1][0]).value() == " Comment 2");
    REQUIRE_FALSE(!isA<Array>(yaml.root()[1][1]));
    REQUIRE(YRef<Comment>(yaml.root()[1][2]).value() == " Comment 3");
  }
  SECTION("YAML parse of comments and three before start.",
          "[YAML][parse][Comment]") {
    BufferSource source{"# Comment 1\n# Comment 2\n# Comment 3\n---\n# Comment "
                        "4\n   - 'One'\n# Comment 5\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0]).value() == " Comment 1");
    REQUIRE(YRef<Comment>(yaml.root()[1]).value() == " Comment 2");
    REQUIRE(YRef<Comment>(yaml.root()[2]).value() == " Comment 3");
    REQUIRE(YRef<Comment>(yaml.root()[3][0]).value() == " Comment 4");
    REQUIRE_FALSE(!isA<Array>(yaml.root()[3][1]));
    REQUIRE(YRef<Comment>(yaml.root()[3][2]).value() == " Comment 5");
  }
  SECTION("YAML parse of comments one on the same line as start.",
          "[YAML][parse][Comment]") {
    BufferSource source{"---# Comment 1\n   - 'One'\n# Comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
    REQUIRE(YRef<Comment>(yaml.root()[0][1]).value() == " Comment 2");
  }
  SECTION("YAML parse of comment after '>'.", "[YAML][parse][Comment]") {
    BufferSource source{"---\nbar: > # test comment 1\n  this is not a normal "
                        "string it\n  spans more than\n  one line\n  see?\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!isA<String>(yaml.root()[0][0]["bar"]));
    REQUIRE(YRef<String>(yaml.root()[0][0]["bar"]).value() ==
            "this is not a normal string it spans more than one line see?");
  }
  // SECTION("YAML parse of comment one same line as a string.",
  //         "[YAML][parse][Comment]") {
  //   BufferSource source{"---\n   - One String\n"};
  //   REQUIRE_NOTHROW(yaml.parse(source));
  //   REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]));
  //   REQUIRE_FALSE(!isA<String>(yaml.root()[0][0][0]));
  //   // REQUIRE(YRef<String>(yaml.root()[0][0][0]).value() == "One String");
  // }
}