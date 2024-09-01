#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Objects.", "[YAML][Parse][Object]") {
  const YAML yaml;
  SECTION("YAML Parse object with one key value pair.",
          "[YAML][Parse][Object]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse object with one key value pair and validate.",
          "[YAML][Parse][Object]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Object>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(YRef<Object>(yaml.root()[0][0])["doe"]).value() ==
            "a deer, a female deer");
  }
  SECTION("YAML Parse object with two key value pair.",
          "[YAML][Parse][Object]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
  SECTION("YAML Parse object with two key value pair and validate.",
          "[YAML][Parse][Object]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Object>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(YRef<Object>(yaml.root()[0][0])["doe"]).value() ==
            "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("ray"));
    REQUIRE(YRef<String>(YRef<Object>(yaml.root()[0][0])["ray"]).value() ==
            "a drop of golden sun");
  }
  SECTION("YAML Parse object with three key value pair and validate.",
          "[YAML][Parse][Object]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n ray: 'a drop of "
                        "golden sun'\npi: 3.14159\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Object>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(YRef<Object>(yaml.root()[0][0])["doe"]).value() ==
            "a deer, a female deer");
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("ray"));
    REQUIRE(YRef<String>(YRef<Object>(yaml.root()[0][0])["ray"]).value() ==
            "a drop of golden sun");
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("pi"));
    REQUIRE(YRef<Number>(YRef<Object>(yaml.root()[0][0])["pi"]).value<int>() ==
            3); // check as int to save rounding errors
  }
  SECTION("YAML Parse object with one key value pair and the value is an "
          "embedded array of ints.",
          "[YAML][Parse][Object]") {
    BufferSource source{"---\n numbers:\n   - 1\n   - 2\n   - 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Object>(yaml.root()[0][0]));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]["numbers"]));
  }
  SECTION("YAML Parse object with one key value pair and the value is an "
          "embedded array of strings.",
          "[YAML][Parse][Object]") {
    BufferSource source{
        "---\n numbers:\n   - 'one'\n   - 'two'\n   - 'three'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Object>(yaml.root()[0][0]));
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]["numbers"]));
  }
  SECTION("YAML Parse object with one key value pair (key has trailing space) "
          "and validate.",
          "[YAML][Parse][Object]") {
    BufferSource source{"---\n doe : 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Object>(yaml.root()[0][0]));
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("doe"));
    REQUIRE(YRef<String>(YRef<Object>(yaml.root()[0][0])["doe"]).value() ==
            "a deer, a female deer");
  }
  SECTION("Parse Object from file and verify.",
          "[YAML][Parse][Examples][File]") {
    BufferSource yamlSource{YAML::fromFile(prefixPath("testfile001.yaml"))};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
    REQUIRE_FALSE(!YRef<Object>(yaml.root()[0][0]).contains("french-hens"));
    REQUIRE(YRef<Number>(yaml.root()[0][0]["french-hens"]).value<int>() == 3);
    REQUIRE_FALSE(!isA<Array>(yaml.root()[0][0]["calling-birds"]));
    REQUIRE(YRef<String>(yaml.root()[0][0]["calling-birds"][2]).value() ==
            "louie");
    REQUIRE_FALSE(!isA<Number>(yaml.root()[0][0]["golden-rings"])); // NEEDS IDENTATION TO WORK
  }
}