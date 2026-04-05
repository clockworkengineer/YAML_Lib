#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check ISource (Stream) interface.", "[YAML][ISource][Stream]") {
  SECTION("Create StreamSource from a std::istringstream.",
          "[YAML][ISource][Stream][Construct]") {
    std::istringstream ss{"---\n- 1\n- 2\n"};
    REQUIRE_NOTHROW(StreamSource(ss));
  }
  SECTION("Create StreamSource with a stream that is not good.",
          "[YAML][ISource][Stream][Construct][Exception]") {
    std::istringstream ss;
    ss.setstate(std::ios_base::failbit);
    REQUIRE_THROWS_AS(StreamSource(ss), ISource::Error);
  }
  SECTION("Create StreamSource and verify first character is correct.",
          "[YAML][ISource][Stream][Current]") {
    std::istringstream ss{"---\n- 1\n"};
    StreamSource source{ss};
    REQUIRE(source.more());
    REQUIRE(source.current() == '-');
  }
  SECTION("Create StreamSource and advance a few characters.",
          "[YAML][ISource][Stream][Next]") {
    std::istringstream ss{"---\n"};
    StreamSource source{ss};
    source.next(); // '-'
    source.next(); // '-'
    source.next(); // '-'
    REQUIRE(source.more());
    REQUIRE(source.current() == kLineFeed);
  }
  SECTION("Create StreamSource and exhaust it, then verify more() is false.",
          "[YAML][ISource][Stream][More]") {
    std::istringstream ss{"ab"};
    StreamSource source{ss};
    REQUIRE(source.more());
    source.next();
    REQUIRE(source.more());
    source.next();
    REQUIRE_FALSE(source.more());
  }
  SECTION("Check StreamSource position() advances correctly.",
          "[YAML][ISource][Stream][Position]") {
    std::istringstream ss{R"(doe: "a deer, a female deer")"};
    StreamSource source{ss};
    while (source.more() && !source.match("deer")) {
      source.next();
    }
    REQUIRE(source.position() == 12);
    while (source.more()) {
      source.next();
    }
    REQUIRE(source.position() == 28);
  }
  SECTION("Create StreamSource, exhaust it, reset and verify back at start.",
          "[YAML][ISource][Stream][Reset]") {
    std::istringstream ss{"---\nkey: value\n"};
    StreamSource source{ss};
    while (source.more()) {
      source.next();
    }
    REQUIRE_FALSE(source.more());
    source.reset();
    REQUIRE(source.more());
    REQUIRE(source.position() == 0);
    REQUIRE(source.current() == '-');
  }
  SECTION("Check StreamSource save() and restore() produce the correct state.",
          "[YAML][ISource][Stream][Save][Restore]") {
    std::istringstream ss{"abc"};
    StreamSource source{ss};
    REQUIRE(source.current() == 'a');
    source.save();
    source.next(); // 'b'
    source.next(); // 'c'
    REQUIRE(source.current() == 'c');
    source.restore();
    REQUIRE(source.current() == 'a');
  }
  SECTION(
      "Check that StreamSource match() finds a string and advances past it.",
      "[YAML][ISource][Stream][Match]") {
    std::istringstream ss{R"(doe: "a deer, a female deer")"};
    StreamSource source{ss};
    REQUIRE_FALSE(source.match("dow")); // not there
    REQUIRE(source.match("doe"));       // match
    REQUIRE(source.position() == 3);
  }
  SECTION("Parse YAML document through StreamSource.",
          "[YAML][ISource][Stream][Parse]") {
    const YAML yaml;
    std::istringstream ss{"---\n- 1\n- 1\n- 2\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(StreamSource{ss}));
    compareYAML(yaml, "---\n- 1\n- 1\n- 2\n...\n");
  }
  SECTION("Parse YAML mapping through StreamSource.",
          "[YAML][ISource][Stream][Parse]") {
    const YAML yaml;
    std::istringstream ss{"---\nname: Alice\nage: 30\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(StreamSource{ss}));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["name"]).value() == "Alice");
    REQUIRE(NRef<Number>(yaml.document(0)["age"]).value<int>() == 30);
  }
  SECTION("try to read past end of StreamSource throws.",
          "[YAML][ISource][Stream][Exception]") {
    std::istringstream ss{"x"};
    StreamSource source{ss};
    source.next(); // consume 'x' — stream now at EOF
    REQUIRE_FALSE(source.more());
    REQUIRE_THROWS_AS(source.next(), ISource::Error);
  }
}
