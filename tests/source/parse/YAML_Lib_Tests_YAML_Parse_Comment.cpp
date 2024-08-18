#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of comments.", "[YAML][Parse][Comment]") {
  const YAML yaml;
  SECTION("YAML Parse of comment.", "[YAML][Parse][Comment]") {
    BufferSource source{"---\n   - 'One'\n# tests Comment"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse of comments.", "[YAML][Parse][Comment]") {
    BufferSource source{"---\n# comment 1\n   - 'One'\n# comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse of comments and one before start.",
          "[YAML][Parse][Comment]") {
    BufferSource source{
        "#comment 1\n---\n# comment 2\n   - 'One'\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse of comments one on the same line as start.",
          "[YAML][Parse][Comment]") {
    BufferSource source{"---# comment 2\n   - 'One'\n# comment 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
}