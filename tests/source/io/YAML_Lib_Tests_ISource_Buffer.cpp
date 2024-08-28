#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check ISource (Buffer) interface.", "[YAML][ISource][Buffer]") {
  const std::string buffer{YAML::fromFile(prefixPath(kSingleYAMLFile))};
  SECTION("Create BufferSource.", "[YAML][ISource][Buffer][Construct]") {
    REQUIRE_NOTHROW(BufferSource(buffer));
  }
  SECTION("Check that BufferSource position() works correctly.",
          "[YAML][ISource][Buffer][Position]") {
    BufferSource source{R"(doe: "a deer, a female deer")"};
    while (source.more() && !source.match("deer")) {
      source.next();
    }
    REQUIRE(source.position() == 12);
    while (source.more()) {
      source.next();
    }
    REQUIRE(source.position() == 28);
  }
  SECTION("Create BufferSource and that it is positioned on the correct first "
          "character.",
          "[YAML][ISource][Buffer][Position]") {
    BufferSource source{BufferSource(buffer)};
    REQUIRE_FALSE(!source.more());
    REQUIRE(static_cast<char>(source.current()) == '-');
  }
  SECTION("Create BufferSource and then check next positions to correct next "
          "character",
          "[YAML][ISource][Buffer][Next]") {
    BufferSource source{BufferSource(buffer)};
    source.next();
    source.next();
    source.next();
    REQUIRE_FALSE(!source.more());
    REQUIRE(static_cast<char>(source.current()) == '\n');
  }
  SECTION("Create BufferSource move past last character, check it and the "
          "bytes moved.",
          "[YAML][ISource][Buffer][More]") {
    BufferSource source{BufferSource(buffer)};
    while (source.more()) {
      source.next();
    }
    REQUIRE(source.position() == 330);                   // eof
    REQUIRE(source.current() == static_cast<char>(EOF)); // eof
  }
  SECTION("Create BufferSource, move past last character, reset and then check "
          "back at the beginning.",
          "[YAML][ISource][Buffer][Reset]") {
    BufferSource source{BufferSource(buffer)};
    while (source.more()) {
      source.next();
    }
    source.reset();
    REQUIRE(source.position() == 0); //  Check at the beginning
  }

  SECTION("Check that BufferSource finds a string at the current position and "
          "moves on past it in stream.",
          "[YAML][ISource][Buffer][Match]") {
    BufferSource source{R"(doe: "a deer, a female deer")"};
    REQUIRE_FALSE(source.match("dow"));  // Not there
    REQUIRE_FALSE(!source.match("doe")); // Match
    REQUIRE(source.position() == 3);     // new position
  }
  SECTION("Create BufferSource with empty buffer.",
          "[YAML][ISource][Buffer][Exception]") {
    REQUIRE_THROWS_AS(BufferSource(""), ISource::Error);
    REQUIRE_THROWS_WITH(
        BufferSource(""),
        "ISource Error: Empty source buffer passed to be parsed.");
  }
  SECTION("Create BufferSource and then try to read off the end.",
          "[YAML][ISource][Buffer][Exception]") {
    BufferSource source{BufferSource(buffer)};
    while (source.more()) {
      source.next();
    }
    REQUIRE_THROWS_AS(source.next(), ISource::Error);
    REQUIRE_THROWS_WITH(source.next(),
                        "ISource Error: Tried to read past and of buffer.");
  }
  // SECTION("Check that BufferSource moves to the next line",
  //         "[YAML][ISource][Buffer][NextLine]") {
  //   BufferSource source{"---\ndoe: \"a deer, a female deer\"\n..."};
  //   REQUIRE(source.current() == '-');
  //   source.nextLine();
  //   REQUIRE(source.current() == 'd');
  //   source.nextLine();
  //   REQUIRE(source.current() == '.');
  // }
}
