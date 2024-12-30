#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check ISource (File) interface.", "[YAML][ISource][File]") {
  const YAML yaml;
  SECTION("Create FileSource.", "[YAML][ISource][File][Construct]") {
    REQUIRE_NOTHROW(FileSource(prefixPath(kSingleSmallYAMLFile)));
  }
  SECTION("Create FileSource from testfile000.yaml and parse",
          "[YAML][ISource][File][Parse]") {
    REQUIRE_NOTHROW(yaml.parse(FileSource(prefixPath(kSingleSmallYAMLFile))));
  }
  SECTION("Create FileSource from testfile000.yaml, parse and stringify.",
          "[YAML][ISource][File][Parse]") {
    REQUIRE_NOTHROW(yaml.parse(FileSource(prefixPath(kSingleSmallYAMLFile))));
    compareYAML(yaml, "---\n- 1\n- 1\n- 2\n...\n");
  }
  SECTION("Check that FileSource position() works correctly.",
          "[YAML][ISource][File][Position]") {
    FileSource source{FileSource(prefixPath(kSingleYAMLFile))};
    while (source.more() && !source.match("deer")) {
      source.next();
    }
    REQUIRE(source.position() == 17);
    while (source.more()) {
      source.next();
    }
    REQUIRE(source.position() == 330);
  }
  SECTION("Create FileSource and that it is positioned on the correct first "
          "character.",
          "[YAML][ISource][File][Position]") {
    FileSource source{prefixPath(kSingleSmallYAMLFile)};
    REQUIRE_FALSE(!source.more());
    REQUIRE(static_cast<char>(source.current()) == '-');
  }
  SECTION("Create FileSource and then check next positions to correct next "
          "character",
          "[YAML][ISource][File][Next]") {
    FileSource source{prefixPath(kSingleSmallYAMLFile)};
    source.next();
    source.next();
    source.next();
    REQUIRE_FALSE(!source.more());
    REQUIRE(static_cast<char>(source.current()) == kLineFeed);
  }
  SECTION("Create FileSource move past last character, check it and the "
          "bytes moved.",
          "[YAML][ISource][File][More]") {
    FileSource source{prefixPath(kSingleYAMLFile)};
    while (source.more()) {
      source.next();
    }
    REQUIRE(source.position() == 330);                   // eof
    REQUIRE(source.current() == static_cast<char>(EOF)); // eof
  }
  SECTION("Create FileSource, move past last character, reset and then check "
          "back at the beginning.",
          "[YAML][ISource][File][Reset]") {
    FileSource source{prefixPath(kSingleYAMLFile)};
    while (source.more()) {
      source.next();
    }
    source.reset();
    REQUIRE(source.position() == 0); //  Check at the beginning
  }

  SECTION("Check that FileSource finds a string at the current position and "
          "moves on past it in stream.",
          "[YAML][ISource][File][Match]") {
    FileSource source{prefixPath(kSingleYAMLFile)};
    while (source.more() && source.current() != 'd') {
      source.next();
    }
    REQUIRE(source.position() == 5);
    REQUIRE_FALSE(source.match("dow")); // Not there
    REQUIRE(source.position() == 5);
    REQUIRE_FALSE(!source.match("doe")); // Match
    REQUIRE(source.position() == 8);     // new position
  }
  SECTION("Create FileSource and then try to read off the end.",
          "[YAML][ISource][File][Exception]") {
    FileSource source{prefixPath(kSingleYAMLFile)};
    while (source.more()) {
      source.next();
    }
    REQUIRE_THROWS_AS(source.next(), ISource::Error);
    REQUIRE_THROWS_WITH(source.next(),
                        "ISource Error: Tried to read past end of file.");
  }
  SECTION("Check that FileSource finds sav/restore working.",
          "[YAML][ISource][File][Match]") {
    FileSource source{prefixPath(kSingleYAMLFile)};
    while (source.more() && source.current() != 'd') {
      source.next();
    }
    source.save();
    REQUIRE(source.position() == 5);
    source.restore();
    REQUIRE(source.position() == 5);
  }
  SECTION("Check that FileSource works with line array that has newlines.",
          "[YAML][ISource][File][Match]") {
    FileSource source{prefixPath("testfile032.yaml")};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
}
