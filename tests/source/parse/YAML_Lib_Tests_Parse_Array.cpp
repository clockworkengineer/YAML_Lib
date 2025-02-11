#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Arrays.", "[YAML][Parse][Array]") {
  const YAML yaml;
  SECTION("YAML parse array with one element.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse array with an empty element.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n - \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 1);
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[0]));
    compareYAML(yaml, "---\n- null\n...\n");
  }
  SECTION("YAML parse array with 4 empty elements.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n - \n - \n - \n - \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 4);
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[0]));
    compareYAML(yaml, "---\n- null\n- null\n- null\n- null\n...\n");
  }
  SECTION("YAML parse array with two elements.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n   - 'Two'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse array with one element and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - 'One'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 1);
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "One");
  }
  SECTION("YAML parse array with multiplw integer elemenrs and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n- 1\n- 1\n- 2\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 3);
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<long>() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)[1]).value<long>() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)[2]).value<long>() == 2);
  }
  SECTION("YAML parse array with multiple string elements and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{
        "---\n   - 'One'\n   - 'Two'\n   - 'Three'\n   - 'Four'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 4);
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "One");
    REQUIRE(YRef<String>(yaml.document(0)[1]).value() == "Two");
    REQUIRE(YRef<String>(yaml.document(0)[2]).value() == "Three");
    REQUIRE(YRef<String>(yaml.document(0)[3]).value() == "Four");
  }
  SECTION("YAML parse array with multiple integer elements and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - -1\n   - +2\n   - -3\n   - 4\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 4);
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == -1);
    REQUIRE(YRef<Number>(yaml.document(0)[1]).value<int>() == 2);
    REQUIRE(YRef<Number>(yaml.document(0)[2]).value<int>() == -3);
    REQUIRE(YRef<Number>(yaml.document(0)[3]).value<int>() == 4);
  }
  SECTION("YAML parse array with one negative integer and check result.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - -1\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == -1);
  }
  SECTION("YAML parse array with one element and non space whitespace",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n  - 'One'\n  - 'Two'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse nested array ", "[YAML][Parse][Array]") {
    BufferSource source{"---\n - - 1\n - - 4\n   - 5\n   - 6\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 1);
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 1);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0][0]));
    REQUIRE(YRef<Number>(yaml.document(0)[0][0]).value<int>() == 1);
    REQUIRE(YRef<Array>(yaml.document(0)[1]).size() == 3);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[1][0]));
    REQUIRE(YRef<Number>(yaml.document(0)[1][0]).value<int>() == 4);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[1][1]));
    REQUIRE(YRef<Number>(yaml.document(0)[1][1]).value<int>() == 5);
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[1][2]));
    REQUIRE(YRef<Number>(yaml.document(0)[1][2]).value<int>() == 6);
  }
  SECTION("Parse Array from file and verify.",
          "[YAML][Parse][Examples][File]") {
    BufferSource yamlSource{YAML::fromFile(prefixPath("testfile003.yaml"))};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)).size() == 3);
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "Mark Joseph");
    REQUIRE(YRef<String>(yaml.document(0)[1]).value() == "James Stephen");
    REQUIRE(YRef<String>(yaml.document(0)[2]).value() == "Ken Griffey");
    REQUIRE_FALSE(!isA<Array>(yaml.document(1)));
    REQUIRE(YRef<Array>(yaml.document(1)).size() == 2);
    REQUIRE(YRef<String>(yaml.document(1)[0]).value() == "Chicago Cubs");
    REQUIRE(YRef<String>(yaml.document(1)[1]).value() == "St Louis Cardinals");
  }
  SECTION("YAML parse flat array of integers and verify.",
          "[YAML][Parse][Array]") {

    BufferSource source{"---\nitems: [1, 2, 3, 4, 5 ]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE(YRef<Array>(yaml.document(0)["items"]).size() == 5);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][0]).value<int>() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][1]).value<int>() == 2);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][2]).value<int>() == 3);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][3]).value<int>() == 4);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][4]).value<int>() == 5);
  }

  SECTION("YAML parse flat array of strings and verify.",
          "[YAML][Parse][Array]") {
    BufferSource source{
        "---\nnames: [\"one\", \"two\", \"three\", \"four\", \"five\" ]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["names"]));
    REQUIRE(YRef<Array>(yaml.document(0)["names"]).size() == 5);
    REQUIRE(YRef<String>(yaml.document(0)["names"][0]).value() == "one");
    REQUIRE(YRef<String>(yaml.document(0)["names"][1]).value() == "two");
    REQUIRE(YRef<String>(yaml.document(0)["names"][2]).value() == "three");
    REQUIRE(YRef<String>(yaml.document(0)["names"][3]).value() == "four");
    REQUIRE(YRef<String>(yaml.document(0)["names"][4]).value() == "five");
  }

  SECTION("YAML parse two flat arrays and verify.", "[YAML][Parse][Array]") {
    BufferSource source{"---\nitems: [ 1, 2, 3, 4, 5 ]\nnames: [ \"one\", "
                        "\"two\", \"three\", \"four\", \"five\" ]"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["items"]));
    REQUIRE(YRef<Array>(yaml.document(0)["items"]).size() == 5);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][0]).value<int>() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][1]).value<int>() == 2);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][2]).value<int>() == 3);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][3]).value<int>() == 4);
    REQUIRE(YRef<Number>(yaml.document(0)["items"][4]).value<int>() == 5);
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["names"]));
    REQUIRE(YRef<Array>(yaml.document(0)["names"]).size() == 5);
    REQUIRE(YRef<String>(yaml.document(0)["names"][0]).value() == "one");
    REQUIRE(YRef<String>(yaml.document(0)["names"][1]).value() == "two");
    REQUIRE(YRef<String>(yaml.document(0)["names"][2]).value() == "three");
    REQUIRE(YRef<String>(yaml.document(0)["names"][3]).value() == "four");
    REQUIRE(YRef<String>(yaml.document(0)["names"][4]).value() == "five");
  }

  SECTION("YAML parse flat array of unquoted strings and verify.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\nnames: [ one, two, three, four, five]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["names"]));
    REQUIRE(YRef<Array>(yaml.document(0)["names"]).size() == 5);
    REQUIRE(YRef<String>(yaml.document(0)["names"][0]).value() == "one");
    REQUIRE(YRef<String>(yaml.document(0)["names"][1]).value() == "two");
    REQUIRE(YRef<String>(yaml.document(0)["names"][2]).value() == "three");
    REQUIRE(YRef<String>(yaml.document(0)["names"][3]).value() == "four");
    REQUIRE(YRef<String>(yaml.document(0)["names"][4]).value() == "five");
  }

  SECTION("YAML parse flat array of booleans and verify.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\nnames: [ True, False, False, False, True]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["names"]));
    REQUIRE(YRef<Array>(yaml.document(0)["names"]).size() == 5);
    REQUIRE(YRef<Boolean>(yaml.document(0)["names"][0]).value() == true);
    REQUIRE(YRef<Boolean>(yaml.document(0)["names"][1]).value() == false);
    REQUIRE(YRef<Boolean>(yaml.document(0)["names"][2]).value() == false);
    REQUIRE(YRef<Boolean>(yaml.document(0)["names"][3]).value() == false);
    REQUIRE(YRef<Boolean>(yaml.document(0)["names"][4]).value() == true);
  }

  SECTION("YAML parse flat array of null and verify.", "[YAML][Parse][Array]") {
    BufferSource source{"---\nnames: [ ~, ~, none, none, none]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["names"]));
    REQUIRE(YRef<Array>(yaml.document(0)["names"]).size() == 5);
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)["names"][0]));
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)["names"][0]));
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)["names"][0]));
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)["names"][0]));
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)["names"][0]));
  }
  SECTION("YAML parse inline array on more than line. "
          "(inline dictionary).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n[1\n,2,\n3,4,5,6\n] \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(YRef<Number>(yaml.document(0)[0]).value<int>() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)[1]).value<int>() == 2);
    REQUIRE(YRef<Number>(yaml.document(0)[2]).value<int>() == 3);
    REQUIRE(YRef<Number>(yaml.document(0)[3]).value<int>() == 4);
    REQUIRE(YRef<Number>(yaml.document(0)[4]).value<int>() == 5);
    REQUIRE(YRef<Number>(yaml.document(0)[5]).value<int>() == 6);
  }
  SECTION("YAML parse array with incorrect indentation in element.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - test\n- test"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 3 Column: 1]: Invalid "
                        "indentation for array element.");
  }
  SECTION("YAML parse array with string that looks like continuing array.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n- test - test"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n- test - test\n...\n");
  }
  SECTION("YAML parse empty array.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n[]\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
  }
  SECTION("YAML parse array ending with ','.", "[YAML][Parse][Array]") {
    BufferSource source{"---\n[one ,two, three, ]\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    compareYAML(yaml, "---\n- one\n- two\n- three\n...\n");
  }
  SECTION("YAML parse inline array containing empty elements'.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n[,, three, ]\n..."};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error: Unexpected ',' in in-line array.");
  }
  SECTION("YAML parse inline array containing incorrect bracket numbers "
          "(example 1).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n test: [1,2,3, [4, 5, 6] ] ]\n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token ']'.");
  }
  SECTION("YAML parse inline array containing incorrect bracket numbers "
          "(example 2).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n test: [1,2,3, [4, 5, 6]] [ \n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token '['.");
  }
  SECTION("YAML parse inline array containing '{' at end. "
          "(example 3).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n test: [1,2,3, [4, 5, 6]] { \n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token '{'.");
  }
  SECTION("YAML parse inline array containing '}' at end. "
          "(example 4).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n test: [1,2,3, [4, 5, 6]] } \n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token '}'.");
  }
  SECTION("YAML parse array and then key/value pair at end. ",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n- 1\n- 2\n- 3\nfour: 4\n"};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error [Line: 5 Column: 1]: Invalid YAML encountered.");
  }
  SECTION("YAML parse string follows by an array. ", "[YAML][Parse][Array]") {
    BufferSource source{"---\ntest string 1\n- 2\n- 3\n- 4\n- 5\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\ntest string 1 - 2 - 3 - 4 - 5\n...\n");
  }
}