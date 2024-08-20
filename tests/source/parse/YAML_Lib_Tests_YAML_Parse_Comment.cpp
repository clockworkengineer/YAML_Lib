#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of comments.", "[YAML][Parse][Comment]") {
  const YAML yaml;
  SECTION("YAML Parse of comment.", "[YAML][Parse][Comment]") {
    BufferSource source{"---\n   - 'One'\n# Test Comment"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0][1]).value() == " Test Comment");
  }
  SECTION("YAML Parse of comments.", "[YAML][Parse][Comment]") {
    BufferSource source{"---\n# Comment 1\n   - 'One'\n# Comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0][0]).value() == " Comment 1");
    REQUIRE(YRef<Comment>(yaml.root()[0][2]).value() == " Comment 2");
  }
  SECTION("YAML Parse of comments and one before start.",
          "[YAML][Parse][Comment]") {
    BufferSource source{
        "# Comment 1\n---\n# Comment 2\n   - 'One'\n# Comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0][0]).value() == " Comment 1");
    REQUIRE(YRef<Comment>(yaml.root()[1][0]).value() == " Comment 2");
    REQUIRE_FALSE(!isA<Array>(yaml.root()[1][1]));
    REQUIRE(YRef<Comment>(yaml.root()[1][2]).value() == " Comment 3");
  }
  SECTION("YAML Parse of comments and three before start.",
          "[YAML][Parse][Comment]") {
    BufferSource source{"# Comment 1\n# Comment 2\n# Comment 3\n---\n# Comment "
                        "4\n   - 'One'\n# Comment 5\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0][0]).value() == " Comment 1");
    REQUIRE(YRef<Comment>(yaml.root()[0][1]).value() == " Comment 2");
    REQUIRE(YRef<Comment>(yaml.root()[0][2]).value() == " Comment 3");
    REQUIRE(YRef<Comment>(yaml.root()[1][0]).value() == " Comment 4");
    REQUIRE_FALSE(!isA<Array>(yaml.root()[1][1]));
    REQUIRE(YRef<Comment>(yaml.root()[1][2]).value() == " Comment 5");
  }
  SECTION("YAML Parse of comments one on the same line as start.",
          "[YAML][Parse][Comment]") {
    BufferSource source{"---# Comment 1\n   - 'One'\n# Comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<Comment>(yaml.root()[0][0]).value() == " Comment 1");
    REQUIRE(YRef<Comment>(yaml.root()[0][2]).value() == " Comment 2");
  }
}