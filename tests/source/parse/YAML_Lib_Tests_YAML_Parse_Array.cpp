#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Arrays.", "[YAML][Parse][Array]") {
  const YAML yaml;
  SECTION("YAML parse array with one element.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse array with two elements.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n   - 'Two'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse array with one element and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Document>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 1);
    REQUIRE(YRef<String>(yaml.document(0)[0][0]).value() == "One");
  }
  SECTION("YAML parse array with multiple string elements and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{
        "---\n   - 'One'\n   - 'Two'\n   - 'Three'\n   - 'Four'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Document>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 4);
    REQUIRE(YRef<String>(yaml.document(0)[0][0]).value() == "One");
    REQUIRE(YRef<String>(yaml.document(0)[0][1]).value() == "Two");
    REQUIRE(YRef<String>(yaml.document(0)[0][2]).value() == "Three");
    REQUIRE(YRef<String>(yaml.document(0)[0][3]).value() == "Four");
  }
  SECTION("YAML parse array with multiple integer elements and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - -1\n   - +2\n   - -3\n   - 4\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Document>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 4);
    REQUIRE(YRef<Number>(yaml.document(0)[0][0]).value<int>() == -1);
    REQUIRE(YRef<Number>(yaml.document(0)[0][1]).value<int>() == 2);
    REQUIRE(YRef<Number>(yaml.document(0)[0][2]).value<int>() == -3);
    REQUIRE(YRef<Number>(yaml.document(0)[0][3]).value<int>() == 4);
  }
  SECTION("YAML parse array with one negative integer and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - -1\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Document>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)[0][0]).value<int>() == -1);
  }
  SECTION("YAML parse array with one element and non space shitspace",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n  - 'One'\n  - 'Two'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse nested array ", "[YAML][Parse][Array]") {
    BufferSource source{"---\n - - 1\n - - 4\n   - 5\n   - 6\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Document>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 2);
    REQUIRE(YRef<Array>(yaml.document(0)[0][0]).size() == 1);
    REQUIRE(YRef<Array>(yaml.document(0)[0][0]).size() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0][0][0]));
    REQUIRE(YRef<Number>(yaml.document(0)[0][0][0]).value<int>() == 1);
    REQUIRE(YRef<Array>(yaml.document(0)[0][1]).size() == 3);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0][1][0]));
    REQUIRE(YRef<Number>(yaml.document(0)[0][1][0]).value<int>() == 4);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0][1][1]));
    REQUIRE(YRef<Number>(yaml.document(0)[0][1][1]).value<int>() == 5);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0][1][2]));
    REQUIRE(YRef<Number>(yaml.document(0)[0][1][2]).value<int>() == 6);
  }
  SECTION("Parse Array from file and verify.",
          "[YAML][Parse][Examples][File]") {
    BufferSource yamlSource{YAML::fromFile(prefixPath("testfile003.yaml"))};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 3);
    REQUIRE(YRef<String>(yaml.document(0)[0][0]).value() == "Mark Joseph");
    REQUIRE(YRef<String>(yaml.document(0)[0][1]).value() == "James Stephen");
    REQUIRE(YRef<String>(yaml.document(0)[0][2]).value() == "Ken Griffey");
    REQUIRE_FALSE(!isA<Array>(yaml.document(1)[0]));
    REQUIRE(YRef<Array>(yaml.document(1)[0]).size() == 2);
    REQUIRE(YRef<String>(yaml.document(1)[0][0]).value() == "Chicago Cubs");
    REQUIRE(YRef<String>(yaml.document(1)[0][1]).value() ==
            "St Louis Cardinals");
  }
  SECTION("YAML parse flat array and verify.", "[YAML][Parse][Array]") {
    // BufferSource source{"---\nitems: [ 1, 2, 3, 4, 5 ]\nnames: [ \"one\", "
    //                     "\"two\", \"three\", \"four\" ]"};
    BufferSource source{"---\nitems: [1, 2, 3, 4, 5 ]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]["items"]));
    // REQUIRE(YRef<Array>(yaml.document(0)[0]["items"]).size() == 5);
    // REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]["names"]));
    // REQUIRE(YRef<Array>(yaml.document(0)[0]["names"]).size() == 5);
    // REQUIRE(YRef<Number>(yaml.document(0)[0]["items"][0]).value<int>() == 1);
    // REQUIRE(YRef<Number>(yaml.document(0)[0]["items"][1]).value<int>() == 2);
    // REQUIRE(YRef<Number>(yaml.document(0)[0]["items"][2]).value<int>() == 3);
    // REQUIRE(YRef<Number>(yaml.document(0)[0]["items"][3]).value<int>() == 4);
    // REQUIRE(YRef<Number>(yaml.document(0)[0]["items"][4]).value<int>() == 5);
  }
}