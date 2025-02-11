#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Dictionary's.", "[YAML][Parse][Dictionary]") {
  const YAML yaml;
  SECTION("YAML parse dictionary with one key value pair.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse dictionary with one key value pair and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("doe"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.document(0))["doe"]).value() ==
            "a deer, a female deer");
  }
  SECTION("YAML parse dictionary with two key value pair.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse dictionary with two key value pair and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("doe"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.document(0))["doe"]).value() ==
            "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("ray"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.document(0))["ray"]).value() ==
            "a drop of golden sun");
  }
  SECTION("YAML parse dictionary with three key value pair and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n ray: 'a drop of "
                        "golden sun'\n pi: 3.14159\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("doe"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.document(0))["doe"]).value() ==
            "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("ray"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.document(0))["ray"]).value() ==
            "a drop of golden sun");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("pi"));
    REQUIRE(
        YRef<Number>(YRef<Dictionary>(yaml.document(0))["pi"]).value<int>() ==
        3); // check as int to save rounding errors
  }
  SECTION("YAML parse dictionary with one key value pair and the value is an "
          "embedded array of ints.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n numbers:\n   - 1\n   - 2\n   - 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["numbers"]));
  }
  SECTION("YAML parse dictionary with one key value pair and the value is an "
          "embedded array of strings.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n numbers:\n   - 'one'\n   - 'two'\n   - 'three'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["numbers"]));
  }
  SECTION(
      "YAML parse dictionary with one key value pair (key has trailing space) "
      "and validate.",
      "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\ndoe   : 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("doe"));
    REQUIRE(YRef<String>(yaml.document(0)["doe"]).value() ==
            "a deer, a female deer");
  }
  SECTION("YAML parse dictionary with two key value pair with one nested.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outer:\n  inner: 'true'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("outer"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["outer"]).contains("inner"));
    REQUIRE(YRef<String>(yaml.document(0)["outer"]["inner"]).value() == "true");
  }
  SECTION("YAML parse dictionary with three key value pair with one nested.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outerone:\n  innerone: 'true'\n outertwo: 99\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("outerone"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["outerone"]).contains("innerone"));
    REQUIRE(YRef<String>(yaml.document(0)["outerone"]["innerone"]).value() ==
            "true");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("outertwo"));
    REQUIRE(YRef<Number>(yaml.document(0)["outertwo"]).value<int>() == 99);
  }
  SECTION("YAML parse dictionary with key value pair nesting on the same line.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outer: inner: 'true'\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 2 Column: 9]: Only an "
                        "inline/compact dictionary is allowed.");
  }
  SECTION("YAML parse dictionary with key value pair and inline dictionary on "
          "the different lines.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outer: \n { inner: 'true'}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("outer"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["outer"]).contains("inner"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)["outer"]["inner"]));
  }
  SECTION("YAML parse dictionary with key value pair and inline dictionary on "
          "the same line.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outer: { inner: 'true'}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("outer"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["outer"]).contains("inner"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)["outer"]["inner"]));
  }
  SECTION("YAML parse dictionary with key value pair with key starting with t.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n two: true\n"};
    REQUIRE_NOTHROW(yaml.parse(source), "");
  }
  SECTION("YAML parse dictionary with key value pair with key starting with f.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n four: true\n"};
    REQUIRE_NOTHROW(yaml.parse(source), "");
  }
  SECTION("Parse Dictionary from file and verify.",
          "[YAML][Parse][Examples][File]") {
    BufferSource yamlSource{YAML::fromFile(prefixPath("testfile001.yaml"))};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("french-hens"));
    REQUIRE(YRef<Number>(yaml.document(0)["french-hens"]).value<int>() == 3);
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)["calling-birds"]));
    REQUIRE(YRef<Array>(yaml.document(0)["calling-birds"]).size() == 4);
    REQUIRE(YRef<String>(yaml.document(0)["calling-birds"][0]).value() ==
            "huey");
    REQUIRE(YRef<String>(yaml.document(0)["calling-birds"][1]).value() ==
            "dewey");
    REQUIRE(YRef<String>(yaml.document(0)["calling-birds"][2]).value() ==
            "louie");
    REQUIRE(YRef<String>(yaml.document(0)["calling-birds"][3]).value() ==
            "fred");
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)).contains("xmas-fifth-day"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)["xmas-fifth-day"]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)["xmas-fifth-day"])
                       .contains("calling-birds"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)["xmas-fifth-day"])
                       .contains("french-hens"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)["xmas-fifth-day"])
                       .contains("golden-rings"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)["xmas-fifth-day"])
                       .contains("partridges"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)["xmas-fifth-day"])
                       .contains("turtle-doves"));
    REQUIRE(YRef<String>(yaml.document(0)["xmas-fifth-day"]["calling-birds"])
                .value() == "four");
    REQUIRE(YRef<Number>(yaml.document(0)["xmas-fifth-day"]["french-hens"])
                .value<int>() == 3);
    REQUIRE(YRef<Number>(yaml.document(0)["xmas-fifth-day"]["golden-rings"])
                .value<int>() == 5);
    REQUIRE(YRef<String>(yaml.document(0)["xmas-fifth-day"]["turtle-doves"])
                .value() == "two");
    REQUIRE(
        YRef<Number>(yaml.document(0)["xmas-fifth-day"]["partridges"]["count"])
            .value<int>() == 1);
    REQUIRE(YRef<String>(
                yaml.document(0)["xmas-fifth-day"]["partridges"]["location"])
                .value() == "a pear tree");
  }

  SECTION("YAML parse flat dictionary of integers and verify.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nfoo: { thing1: 1, thing2: 2, thing3: 3 }\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("foo"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)["foo"]));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing1"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing2"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing3"));
    REQUIRE(YRef<Number>(yaml.document(0)["foo"]["thing1"]).value<int>() == 1);
    REQUIRE(YRef<Number>(yaml.document(0)["foo"]["thing2"]).value<int>() == 2);
    REQUIRE(YRef<Number>(yaml.document(0)["foo"]["thing3"]).value<int>() == 3);
  }
  SECTION("YAML parse flat dictionary of quoted strings and verify.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\nfoo: { thing1: \"one\", thing2: \"two\", thing3: \"three\" }\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("foo"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)["foo"]));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing1"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing2"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing3"));
    REQUIRE(YRef<String>(yaml.document(0)["foo"]["thing1"]).value() == "one");
    REQUIRE(YRef<String>(yaml.document(0)["foo"]["thing2"]).value() == "two");
    REQUIRE(YRef<String>(yaml.document(0)["foo"]["thing3"]).value() == "three");
  }
  SECTION("YAML parse flat dictionary of unquoted strings and verify.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\nfoo: { thing1: one, thing2: two, thing3: three}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("foo"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)["foo"]));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing1"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing2"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["foo"]).contains("thing3"));
    REQUIRE(YRef<String>(yaml.document(0)["foo"]["thing1"]).value() == "one");
    REQUIRE(YRef<String>(yaml.document(0)["foo"]["thing2"]).value() == "two");
    REQUIRE(YRef<String>(yaml.document(0)["foo"]["thing3"]).value() == "three");
  }

  SECTION("YAML parse dictionary with space in key name.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nMark McGwire: 55\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("Mark McGwire"));
    REQUIRE(YRef<Number>(yaml.document(0)["Mark McGwire"]).value<int>() == 55);
  }

  SECTION("YAML parse dictionary with flat dictionary terminator on next line.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"Sammy Sosa: {\n    hr: 63,\n    avg: 0.288\n  }\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }

  SECTION("YAML parse dictionary with duplicate keys.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\nthing1: \"one\"\nthing1: \"two\"\nthing3: \"three\"\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 4 Column: 1]: Dictionary "
                        "already contains key 'thing1'.");
  }
  SECTION("YAML parse dictionary with duplicate keys in two documents.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\ntime: 20:03:20\nplayer: Sammy Sosa\naction: "
                        "strike (miss)\n\n---\ntime: 20:03:47\nplayer: Sammy "
                        "Sosa\naction: grand slam\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse dictionary with duplicate keys in two "
          "documents(terminating one with end).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\ntime: 20:03:20\nplayer: Sammy Sosa\naction: "
                        "strike (miss)\n\n...\ntime: 20:03:47\nplayer: Sammy "
                        "Sosa\naction: grand slam\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse dictionary with non string keys (boolean).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nTrue: On\nFalse: Off\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("true"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("false"));
    REQUIRE(YRef<Boolean>(yaml.document(0)["true"]).value() == true);
    REQUIRE(YRef<Boolean>(yaml.document(0)["false"]).value() == false);
    compareYAML(yaml, "---\n\"true\": On\n\"false\": Off\n...\n");
  }
  SECTION("YAML parse dictionary with non string key (null).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nnull: 1\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains(""));
    REQUIRE(YRef<Number>(yaml.document(0)[""]).value<int>() == 1);
    compareYAML(yaml, "---\n\"\": 1\n...\n");
  }
  SECTION("YAML parse dictionary with non string key (number).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n666: 1\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("666"));
    REQUIRE(YRef<Number>(yaml.document(0)["666"]).value<int>() == 1);
    compareYAML(yaml, "---\n\"666\": 1\n...\n");
  }
  SECTION("YAML parse dictionary with non string keys (inline array).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n[one, two]: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\n\"[one, two]\": 'test'\n...\n");
  }
  SECTION("YAML parse dictionary with non string keys (nested inline array).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n[one, two, [1,2]]: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\n\"[one, two, [1, 2]]\": \'test\'\n...\n");
  }
  SECTION("YAML parse dictionary with non string keys (inline array).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n[one, two]: 'test1'\n[three, four]: 'test2'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(
        yaml,
        "---\n\"[one, two]\": \'test1\'\n\"[three, four]\": \'test2\'\n...\n");
  }
  SECTION("YAML parse dictionary with non string keys (inline dictionary).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n{one: 1, two: 2}: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\n\"{one: 1, two: 2}\": \'test\'\n...\n");
  }
  SECTION("YAML parse dictionary with non string keys (nested inline "
          "dictionary 1).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n{one: 1, { three: 3} }: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\n\"{one: 1, {three: 3}: null}\": \'test\'\n...\n");
  }
  SECTION(
      "YAML parse dictionary with non string keys (nested inline dictionary).",
      "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n{one: 1, two: { three: 3} }: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\n\"{one: 1, two: {three: 3}}\": \'test\'\n...\n");
  }
  SECTION("YAML parse dictionary with non string keys (nested inline array).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n[1,2,3,[4,5,6]]: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\n\"[1, 2, 3, [4, 5, 6]]\": \'test\'\n...\n");
  }
  SECTION("YAML parse inline dictionary on more than line. "
          "(inline dictionary).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n{ one: \n1, \n two : 2\n, \n three: 3, four: \n 4} \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("one"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("two"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("three"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("four"));
    compareYAML(yaml, "---\none: 1\ntwo: 2\nthree: 3\nfour: 4\n...\n");
  }

  SECTION("YAML parse nexted inline dictionary on more than line. "
          "(inline dictionary).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nouter: { one: \n1, \n two : 2\n, \n three: 3, "
                        "four: \n 4} \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)["outer"]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)["outer"]).contains("one"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)["outer"]).contains("two"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["outer"]).contains("three"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)["outer"]).contains("four"));
    compareYAML(
        yaml, "---\nouter: \n  one: 1\n  two: 2\n  three: 3\n  four: 4\n...\n");
  }

  SECTION("YAML parse dictionary with  no key value.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains(""));
    compareYAML(yaml, "---\n\"\": \'test\'\n...\n");
  }

  SECTION("YAML parse dictionary with no key value twice.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n: 'test'\n: 'test'\n...\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 4 Column: 1]: Dictionary "
                        "already contains key ''.");
  }
  SECTION(
      "YAML parse dictionary with non string keys are on more than one line "
      "(inline dictionary).",
      "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n{one: \n1, \ntwo: 2}: 'test'\n...\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 4 Column: 8]: Inline "
                        "dictionary used as key is meant to be on one line.");
  }
  SECTION(
      "YAML parse dictionary with non string keys are on more than one line "
      "(inline array).",
      "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n[1,\n2,\n3]: 'test'\n...\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 4 Column: 3]: Inline array "
                        "used as key is meant to be on one line.");
  }
  SECTION("YAML parse dictionary with out of line key indentation.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"outer: 1\n  inner: 0"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 2 Column: 3]: Mapping key "
                        "has the incorrect indentation.");
  }
  SECTION("YAML parse empty dictionary.", "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
  }
  SECTION("YAML parse in dictionary one key value pair and trailing ','.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { one: 1, } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\none: 1\n...\n");
  }
  SECTION(
      "YAML parse in dictionary one key value pair and multipal trailing ','.",
      "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { one: 1,,,} \n...\n"};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected ',' in in-line dictionary.");
  }
  SECTION("YAML parse in dictionary with no key value pairs just ','.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { , } n...\n"};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected ',' in in-line dictionary.");
  }
  SECTION("YAML parse  dictionary with just keys (example 1).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee, } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\neeee: null\n...\n");
  }
  SECTION("YAML parse  dictionary with just keys (example 2).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee:, } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\neeee: null\n...\n");
  }
  SECTION("YAML parse  dictionary with just keys (example 3).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee:, rrrr } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\neeee: null\nrrrr: null\n...\n");
  }
  SECTION("YAML parse  dictionary with just keys (example 4).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee:, rrrr:, ooooo:, } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\neeee: null\nrrrr: null\nooooo: null\n...\n");
  }
  SECTION("YAML parse  dictionary with just keys (example 5).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee:, \nrrrr:, \nooooo:, } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\neeee: null\nrrrr: null\nooooo: null\n...\n");
  }
  SECTION("YAML parse  dictionary with just keys (example 6).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee, rrrr: } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\neeee: null\nrrrr: null\n...\n");
  }
  SECTION("YAML parse dictionary with just keys (no colons) (example 7).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee, rrrr, oooo } \n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    compareYAML(yaml, "---\neeee: null\nrrrr: null\noooo: null\n...\n");
  }
  SECTION("YAML parse dictionary with s duplicate keys) (example 8).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n { eeee, rrrr, rrrr } \n...\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 2 Column: 21]: Dictionary "
                        "already contains key 'rrrr'.");
  }
  SECTION("YAML parse dictionary with nested dictionary with partial entry "
          "(example 9).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"{one: 1, {three: 3} }"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\none: 1\n\"{three: 3}\": null\n...\n");
  }
  SECTION("YAML parse inline dictionary containing incorrect bracket numbers "
          "(example 1).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n test: { one: 1, two: 2}}\n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token '}'.");
  }
  SECTION("YAML parse inline dictionary containing incorrect bracket numbers "
          "(example 2).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n test: { one: 1, two: 2} { \n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token '{'.");
  }
  SECTION("YAML parse inline dictionary containing '[' at end. "
          "(example 3).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n test: { one: 1, two: 2} [ \n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token '['.");
  }
  SECTION("YAML parse inline dictionary containing ']' at end. "
          "(example 4).",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n test: { one: 1, two: 2} ] \n..."};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token ']'.");
  }
  SECTION("YAML parse dictionary with nested dictionary key (example 5).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        R"({one: ["one","two","three", ["four","]",6]]] })"};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Unexpected flow sequence token ']'.");
  }
  SECTION("YAML parse dictionary with string keys.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n\"[Detroit Tigers, Chicago Cubs]\":\n  - "
                        "2001-07-23\n\"[New York Yankees, Atlanta Braves]\":\n "
                        " - 2001-07-02\n  - 2001-08-12\n  - 2001-08-14\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n\"[Detroit Tigers, Chicago Cubs]\": \n  - "
                      "2001-07-23\n\"[New York Yankees, Atlanta Braves]\": \n  "
                      "- 2001-07-02\n  - 2001-08-12\n  - 2001-08-14\n...\n");
  }
  SECTION("YAML parse string with wit dictionary after.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\ntest string 0\ntest1: 1\ntest2: 2\ntest3: 4\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error: Invalid YAML encountered.");
  }
}