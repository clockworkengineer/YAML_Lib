#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check IDestination (Stream) interface.",
          "[YAML][IDestination][Stream]") {
  SECTION("Create StreamDestination from a std::ostringstream.",
          "[YAML][IDestination][Stream][Construct]") {
    std::ostringstream ss;
    REQUIRE_NOTHROW(StreamDestination(ss));
  }
  SECTION("Create StreamDestination and add a single character.",
          "[YAML][IDestination][Stream][Add]") {
    std::ostringstream ss;
    StreamDestination dest{ss};
    dest.add('x');
    REQUIRE(ss.str() == "x");
    REQUIRE(dest.last() == 'x');
  }
  SECTION("Create StreamDestination and add a string.",
          "[YAML][IDestination][Stream][Add]") {
    std::ostringstream ss;
    StreamDestination dest{ss};
    dest.add(std::string{"hello"});
    REQUIRE(ss.str() == "hello");
    REQUIRE(dest.last() == 'o');
  }
  SECTION("Create StreamDestination and add a C-string.",
          "[YAML][IDestination][Stream][Add]") {
    std::ostringstream ss;
    StreamDestination dest{ss};
    dest.add("world");
    REQUIRE(ss.str() == "world");
    REQUIRE(dest.last() == 'd');
  }
  SECTION("Create StreamDestination and add a string_view.",
          "[YAML][IDestination][Stream][Add]") {
    std::ostringstream ss;
    StreamDestination dest{ss};
    dest.add(std::string_view{"view"});
    REQUIRE(ss.str() == "view");
    REQUIRE(dest.last() == 'w');
  }
  SECTION("Create StreamDestination and add content with linefeeds.",
          "[YAML][IDestination][Stream][Linefeed]") {
    std::ostringstream ss;
    StreamDestination dest{ss};
    dest.add("line1\n");
    dest.add("line2\n");
    REQUIRE(dest.last() == kLineFeed);
    REQUIRE(ss.str() == "line1\nline2\n");
  }
  SECTION("Create StreamDestination and verify clear() resets last().",
          "[YAML][IDestination][Stream][Clear]") {
    std::ostringstream ss;
    StreamDestination dest{ss};
    dest.add("text");
    REQUIRE(dest.last() == 't');
    dest.clear();
    REQUIRE(dest.last() == kNull);
  }
  SECTION("Stringify YAML through StreamDestination.",
          "[YAML][IDestination][Stream][Stringify]") {
    const YAML yaml;
    std::istringstream in{"---\n- 1\n- 1\n- 2\n...\n"};
    yaml.parse(StreamSource{in});
    std::ostringstream out;
    StreamDestination dest{out};
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(out.str() == "---\n- 1\n- 1\n- 2\n...\n");
  }
  SECTION("Stringify YAML mapping through StreamDestination.",
          "[YAML][IDestination][Stream][Stringify]") {
    const YAML yaml;
    std::istringstream in{"---\nname: Alice\nage: 30\n...\n"};
    yaml.parse(StreamSource{in});
    std::ostringstream out;
    StreamDestination dest{out};
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE_FALSE(out.str().empty());
    // Verify round-trip: re-parse the stringified output
    std::istringstream reparsed{out.str()};
    const YAML yaml2;
    REQUIRE_NOTHROW(yaml2.parse(StreamSource{reparsed}));
    REQUIRE(isA<Dictionary>(yaml2.document(0)));
    REQUIRE(NRef<String>(yaml2.document(0)["name"]).value() == "Alice");
    REQUIRE(NRef<Number>(yaml2.document(0)["age"]).value<int>() == 30);
  }
  SECTION("Stringify to another std::ostringstream via StreamDestination "
          "(smoke test).",
          "[YAML][IDestination][Stream][Stringify]") {
    const YAML yaml;
    std::istringstream in{"key: value\n"};
    yaml.parse(StreamSource{in});
    std::ostringstream out;
    StreamDestination dest{out};
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE_FALSE(out.str().empty());
  }
}
