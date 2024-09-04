#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Dictionarys.", "[YAML][Parse][Dictionary]") {
  const YAML yaml;
  SECTION("YAML Parse dictionary with one key value pair.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse dictionary with one key value pair and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.root()[0][0])["doe"]).value() ==
            "a deer, a female deer");
  }
  SECTION("YAML Parse dictionary with two key value pair.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse dictionary with two key value pair and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.root()[0][0])["doe"]).value() ==
            "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("ray"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.root()[0][0])["ray"]).value() ==
            "a drop of golden sun");
  }
  SECTION("YAML Parse dictionary with three key value pair and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n ray: 'a drop of "
                        "golden sun'\n pi: 3.14159\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.root()[0][0])["doe"]).value() ==
            "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("ray"));
    REQUIRE(YRef<String>(YRef<Dictionary>(yaml.root()[0][0])["ray"]).value() ==
            "a drop of golden sun");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("pi"));
    REQUIRE(YRef<Number>(YRef<Dictionary>(yaml.root()[0][0])["pi"]).value<int>() ==
            3); // check as int to save rounding errors
  }
  SECTION("YAML Parse dictionary with one key value pair and the value is an "
          "embedded array of ints.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n numbers:\n   - 1\n   - 2\n   - 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]["numbers"]));
  }
  SECTION("YAML Parse dictionary with one key value pair and the value is an "
          "embedded array of strings.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{
        "---\n numbers:\n   - 'one'\n   - 'two'\n   - 'three'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]["numbers"]));
  }
  SECTION("YAML Parse dictionary with one key value pair (key has trailing space) "
          "and validate.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n doe : 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(yaml.root()[0][0]["doe"]).value() ==
            "a deer, a female deer");
  }
  SECTION("YAML Parse dictionary with two key value pair with one nested.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outer:\n  inner: 'true'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("outer"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]["outer"]).contains("inner"));
    REQUIRE(YRef<String>(yaml.root()[0][0]["outer"]["inner"]).value() ==
            "true");
  }
  SECTION("YAML Parse dictionary with fhree key value pair with one nested.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n outerone:\n  innerone: 'true'\n outertwo: 99\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("outerone"));
    REQUIRE_FALSE(
        !YRef<Dictionary>(yaml.root()[0][0]["outerone"]).contains("innerone"));
    REQUIRE(YRef<String>(yaml.root()[0][0]["outerone"]["innerone"]).value() ==
            "true");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("outertwo"));
    REQUIRE(YRef<Number>(yaml.root()[0][0]["outertwo"]).value<int>() == 99);
  }
  //   SECTION("YAML Parse dictionary with key value pair nesting on the same
  //   line.",
  //           "[YAML][Parse][Dictionary]") {
  //     BufferSource source{"---\n outer: inner: 'true'\n"};
  //     REQUIRE_THROWS_WITH(yaml.parse(source), "");
  //   }
  SECTION("YAML Parse dictionary with key value pair with key starting with t.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n two: true\n"};
    REQUIRE_NOTHROW(yaml.parse(source), "");
  }
  SECTION("YAML Parse dictionary with key value pair with key starting with f.",
          "[YAML][Parse][Dictionary]") {
    BufferSource source{"---\n four: true\n"};
    REQUIRE_NOTHROW(yaml.parse(source), "");
  }
  SECTION("Parse Dictionary from file and verify.",
          "[YAML][Parse][Examples][File]") {
    BufferSource yamlSource{YAML::fromFile(prefixPath("testfile001.yaml"))};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("french-hens"));
    REQUIRE(YRef<Number>(yaml.root()[0][0]["french-hens"]).value<int>() == 3);
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]["calling-birds"]));
    REQUIRE(YRef<Array>(yaml.root()[0][0]["calling-birds"]).size() == 4);
    REQUIRE(YRef<String>(yaml.root()[0][0]["calling-birds"][0]).value() ==
            "huey");
    REQUIRE(YRef<String>(yaml.root()[0][0]["calling-birds"][1]).value() ==
            "dewey");
    REQUIRE(YRef<String>(yaml.root()[0][0]["calling-birds"][2]).value() ==
            "louie");
    REQUIRE(YRef<String>(yaml.root()[0][0]["calling-birds"][3]).value() ==
            "fred");
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]).contains("xmas-fifth-day"));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.root()[0][0]["xmas-fifth-day"]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]["xmas-fifth-day"])
                       .contains("calling-birds"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]["xmas-fifth-day"])
                       .contains("french-hens"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]["xmas-fifth-day"])
                       .contains("golden-rings"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]["xmas-fifth-day"])
                       .contains("partridges"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.root()[0][0]["xmas-fifth-day"])
                       .contains("turtle-doves"));
    REQUIRE(YRef<String>(yaml.root()[0][0]["xmas-fifth-day"]["calling-birds"])
                .value() == "four");
    REQUIRE(YRef<Number>(yaml.root()[0][0]["xmas-fifth-day"]["french-hens"])
                .value<int>() == 3);
    REQUIRE(YRef<Number>(yaml.root()[0][0]["xmas-fifth-day"]["golden-rings"])
                .value<int>() == 5);
    REQUIRE(YRef<String>(yaml.root()[0][0]["xmas-fifth-day"]["turtle-doves"])
                .value() == "two");
    REQUIRE(
        YRef<Number>(yaml.root()[0][0]["xmas-fifth-day"]["partridges"]["count"])
            .value<int>() == 1);
    REQUIRE(YRef<String>(
                yaml.root()[0][0]["xmas-fifth-day"]["partridges"]["location"])
                .value() == "a pear tree");
  }
}