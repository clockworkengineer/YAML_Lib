#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Dictionarys.", "[YAML][Parse][Dictionary]") {
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
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("doe"));
    REQUIRE(
        YRef<String>(YRef<Dictionary>(yaml.document(0)[0])["doe"]).value() ==
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
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("doe"));
    REQUIRE(
        YRef<String>(YRef<Dictionary>(yaml.document(0)[0])["doe"]).value() ==
        "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("ray"));
    REQUIRE(
        YRef<String>(YRef<Dictionary>(yaml.document(0)[0])["ray"]).value() ==
        "a drop of golden sun");
  }
  SECTION("YAML parse dictionary with three key value pair and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n ray: 'a drop of "
                        "golden sun'\n pi: 3.14159\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("doe"));
    REQUIRE(
        YRef<String>(YRef<Dictionary>(yaml.document(0)[0])["doe"]).value() ==
        "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("ray"));
    REQUIRE(
        YRef<String>(YRef<Dictionary>(yaml.document(0)[0])["ray"]).value() ==
        "a drop of golden sun");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("pi"));
    REQUIRE(YRef<Number>(YRef<Dictionary>(yaml.document(0)[0])["pi"])
                .value<int>() == 3); // check as int to save rounding errors
  }
  SECTION("YAML parse dictionary with one key value pair and the value is an "
          "embedded array of ints.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n numbers:\n   - 1\n   - 2\n   - 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]["numbers"]));
  }
  SECTION("YAML parse dictionary with one key value pair and the value is an "
          "embedded array of strings.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n numbers:\n   - 'one'\n   - 'two'\n   - 'three'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]["numbers"]));
  }
  SECTION(
      "YAML parse dictionary with one key value pair (key has trailing space) "
      "and validate.",
      "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\ndoe   : 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("doe"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["doe"]).value() ==
            "a deer, a female deer");
  }
  SECTION("YAML parse dictionary with two key value pair with one nested.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outer:\n  inner: 'true'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("outer"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["outer"]).contains("inner"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["outer"]["inner"]).value() ==
            "true");
  }
  SECTION("YAML parse dictionary with fhree key value pair with one nested.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outerone:\n  innerone: 'true'\n outertwo: 99\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("outerone"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]["outerone"])
                       .contains("innerone"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["outerone"]["innerone"]).value() ==
            "true");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("outertwo"));
    REQUIRE(YRef<Number>(yaml.document(0)[0]["outertwo"]).value<int>() == 99);
  }
  SECTION("YAML parse dictionary with key value pair nesting on the same line.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outer: inner: 'true'\n"};
    REQUIRE_THROWS_WITH(
        yaml.parse(source),
        "YAML Syntax Error: Only an inline/compact dictionary is allowed.");
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
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]).contains("french-hens"));
    REQUIRE(YRef<Number>(yaml.document(0)[0]["french-hens"]).value<int>() == 3);
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]["calling-birds"]));
    REQUIRE(YRef<Array>(yaml.document(0)[0]["calling-birds"]).size() == 4);
    REQUIRE(YRef<String>(yaml.document(0)[0]["calling-birds"][0]).value() ==
            "huey");
    REQUIRE(YRef<String>(yaml.document(0)[0]["calling-birds"][1]).value() ==
            "dewey");
    REQUIRE(YRef<String>(yaml.document(0)[0]["calling-birds"][2]).value() ==
            "louie");
    REQUIRE(YRef<String>(yaml.document(0)[0]["calling-birds"][3]).value() ==
            "fred");
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]).contains("xmas-fifth-day"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]["xmas-fifth-day"]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]["xmas-fifth-day"])
                       .contains("calling-birds"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]["xmas-fifth-day"])
                       .contains("french-hens"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]["xmas-fifth-day"])
                       .contains("golden-rings"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]["xmas-fifth-day"])
                       .contains("partridges"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]["xmas-fifth-day"])
                       .contains("turtle-doves"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["xmas-fifth-day"]["calling-birds"])
                .value() == "four");
    REQUIRE(YRef<Number>(yaml.document(0)[0]["xmas-fifth-day"]["french-hens"])
                .value<int>() == 3);
    REQUIRE(YRef<Number>(yaml.document(0)[0]["xmas-fifth-day"]["golden-rings"])
                .value<int>() == 5);
    REQUIRE(YRef<String>(yaml.document(0)[0]["xmas-fifth-day"]["turtle-doves"])
                .value() == "two");
    REQUIRE(YRef<Number>(
                yaml.document(0)[0]["xmas-fifth-day"]["partridges"]["count"])
                .value<int>() == 1);
    REQUIRE(YRef<String>(
                yaml.document(0)[0]["xmas-fifth-day"]["partridges"]["location"])
                .value() == "a pear tree");
  }

  SECTION("YAML parse flat dictionary of integers and verify.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nfoo: { thing1: 1, thing2: 2, thing3: 3 }\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("foo"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]["foo"]));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing1"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing2"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing3"));
    REQUIRE(YRef<Number>(yaml.document(0)[0]["foo"]["thing1"]).value<int>() ==
            1);
    REQUIRE(YRef<Number>(yaml.document(0)[0]["foo"]["thing2"]).value<int>() ==
            2);
    REQUIRE(YRef<Number>(yaml.document(0)[0]["foo"]["thing3"]).value<int>() ==
            3);
  }
  SECTION("YAML parse flat dictionary of quoted strings and verify.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\nfoo: { thing1: \"one\", thing2: \"two\", thing3: \"three\" }\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("foo"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]["foo"]));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing1"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing2"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing3"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["foo"]["thing1"]).value() ==
            "one");
    REQUIRE(YRef<String>(yaml.document(0)[0]["foo"]["thing2"]).value() ==
            "two");
    REQUIRE(YRef<String>(yaml.document(0)[0]["foo"]["thing3"]).value() ==
            "three");
  }
  SECTION("YAML parse flat dictionary of unquoted strings and verify.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\nfoo: { thing1: one, thing2: two, thing3: three}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("foo"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]["foo"]));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing1"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing2"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]["foo"]).contains("thing3"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["foo"]["thing1"]).value() ==
            "one");
    REQUIRE(YRef<String>(yaml.document(0)[0]["foo"]["thing2"]).value() ==
            "two");
    REQUIRE(YRef<String>(yaml.document(0)[0]["foo"]["thing3"]).value() ==
            "three");
  }

  SECTION("YAML parse dictionary with space in key name.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nMark McGwire: 55\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.document(0)[0]).contains("Mark McGwire"));
    REQUIRE(YRef<Number>(yaml.document(0)[0]["Mark McGwire"]).value<int>() ==
            55);
  }

  SECTION("YAML parse dictionary with flat dictionary terminator on next line.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"Sammy Sosa: {\n    hr: 63,\n    avg: 0.288\n  }\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }

  SECTION("YAML parse dictionary with duplicate keys.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\nthing1: \"one\"\n thing1: \"two\"\n thing3: \"three\"\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 4 Column: 2]: Dictionary "
                        "already contains key 'thing1'.");
  }

  SECTION("YAML parse dictionarys with duplicate keys in two documents.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\ntime: 20:03:20\nplayer: Sammy Sosa\naction: "
                        "strike (miss)\n\n---\ntime: 20:03:47\nplayer: Sammy "
                        "Sosa\naction: grand slam\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML parse dictionarys with duplicate keys in two "
          "documents(terminating one with end).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\ntime: 20:03:20\nplayer: Sammy Sosa\naction: "
                        "strike (miss)\n\n...\ntime: 20:03:47\nplayer: Sammy "
                        "Sosa\naction: grand slam\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
//     SECTION("YAML parse dictionarys with non string keys (boolean).",
//             "[YAML][Parse][Dictionary]") {
//       BufferSource source{"---\nTrue: On\nFalse: Off\n..."};
//       REQUIRE_NOTHROW(yaml.parse(source));
//       REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
//       REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("True"));
//       REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("False"));
//       REQUIRE(YRef<Boolean>(yaml.document(0)[0]["True"]).value() == true);
//       REQUIRE(YRef<Boolean>(yaml.document(0)[0]["False"]).value() == false);
//       BufferDestination destination;
//       REQUIRE_NOTHROW(yaml.stringify(destination));
//       REQUIRE(destination.toString() == "");
//     }
  SECTION("YAML parse dictionarys with non string key (null).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\nnull: 1\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains(""));
    REQUIRE(YRef<Number>(yaml.document(0)[0][""]).value<int>() == 1);
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\"\": 1\n...\n");
  }
  SECTION("YAML parse dictionarys with non string key (number).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n666: 1\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("666"));
    REQUIRE(YRef<Number>(yaml.document(0)[0]["666"]).value<int>() == 1);
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\"666\": 1\n...\n");
  }

  SECTION("YAML parse dictionarys with non string keys (inline array).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n[one, two]: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\"[one, two]\": 'test'\n...\n");
  }
  SECTION("YAML parse dictionarys with non string keys (inline array).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n[one, two]: 'test1'\n[three, four]: 'test2'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(
        destination.toString() ==
        "---\n\"[one, two]\": \'test1\'\n\"[three, four]\": \'test2\'\n...\n");
  }
  SECTION("YAML parse dictionarys with non string keys (inline dictionary).",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n{one: 1, two: 2}: 'test'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\"{one: 1, two: 2}\": \'test\'\n...\n");
  }
}