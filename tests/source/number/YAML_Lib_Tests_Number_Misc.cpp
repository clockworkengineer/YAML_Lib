#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YNode invalid number conversion.",
          "[YAML][YNode][Number][Invalid]"){
  const YAML yaml;
  SECTION("Check integer with invalid characters.",
  "[YAML][YNode][Number][Invalid]")
  {
      BufferSource yamlSource{ "4345u334u" };
      REQUIRE_NOTHROW(yaml.parse(yamlSource));
      REQUIRE_FALSE(!isA<String>(yaml.document(0)));
      REQUIRE(YRef<String>(yaml.document(0)).value()=="4345u334u");
  }
  SECTION("Check floating point with invalid exponent.",
  "[YAML][YNode][Number][Invalid]")
  {
      BufferSource yamlSource{ "78.e43e-2" };
      REQUIRE_NOTHROW(yaml.parse(yamlSource));
      REQUIRE_FALSE(!isA<String>(yaml.document(0)));
      REQUIRE(YRef<String>(yaml.document(0)).value()=="78.e43e-2");
  }
  SECTION("Check floating point with multiple decimal points.",
  "[YAML][YNode][Number][Invalid]")
  {
      BufferSource yamlSource{ "78.5454.545" };
      REQUIRE_NOTHROW(yaml.parse(yamlSource));
      REQUIRE_FALSE(!isA<String>(yaml.document(0)));
      REQUIRE(YRef<String>(yaml.document(0)).value()=="78.5454.545");
  }
}

TEST_CASE("Check YNode Number API(s) for all supported number types.",
          "[YAML][YNode][Number]") {
  YAML yaml;
  SECTION("Check numbers are the correct type.",
          "[YAML][YNode][Number][Addition]") {
    yaml["root"] = {1, 1l, 1ll, 1.0f, 1.0, 1.0l};
    BufferDestination destinationBuffer;
    yaml.stringify(destinationBuffer);
    REQUIRE(
        destinationBuffer.toString() ==
        "---\nroot: \n  - 1\n  - 1\n  - 1\n  - 1.0\n  - 1.0\n  - 1.0\n...\n");
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][0]).is<int>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][1]).is<long>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][2]).is<long long>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][3]).is<float>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][4]).is<double>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][5]).is<long double>());
  }
  SECTION("Simple arithmetic add one to a number.",
          "[YAML][YNode][Number][Get/Set]") {
    yaml["root"] = {1, 1l, 1l, 1.0f, 1.0, 1.0l};
    BufferDestination destinationBuffer;
    yaml.stringify(destinationBuffer);
    REQUIRE(
        destinationBuffer.toString() ==
        "---\nroot: \n  - 1\n  - 1\n  - 1\n  - 1.0\n  - 1.0\n  - 1.0\n...\n");
    Number &integerRef = YRef<Number>(yaml["root"][0]);
    REQUIRE_NOTHROW(integerRef.set(integerRef.value<int>() + 1));
    Number &longRef = YRef<Number>(yaml["root"][1]);
    REQUIRE_NOTHROW(longRef.set(longRef.value<long>() + 1));
    Number &longLongRef = YRef<Number>(yaml["root"][2]);
    REQUIRE_NOTHROW(longLongRef.set(longLongRef.value<long long>() + 1));
    Number &floatRef = YRef<Number>(yaml["root"][3]);
    REQUIRE_NOTHROW(floatRef.set(floatRef.value<float>() + 1.0f));
    Number &doubleRef = YRef<Number>(yaml["root"][4]);
    REQUIRE_NOTHROW(doubleRef.set(doubleRef.value<double>() + 1.0));
    Number &longDoubleRef = YRef<Number>(yaml["root"][5]);
    REQUIRE_NOTHROW(
        longDoubleRef.set(longDoubleRef.value<long double>() + 1.0));
    destinationBuffer.clear();
    yaml.stringify(destinationBuffer);
    REQUIRE(
        destinationBuffer.toString() ==
        "---\nroot: \n  - 2\n  - 2\n  - 2\n  - 2.0\n  - 2.0\n  - 2.0\n...\n");
  }
  SECTION("Change types and values.", "[YAML][YNode][Number][Reset]") {
    yaml["root"] = {1, 1l, 1ll, 1.0f, 1.0, 1.0l};
    BufferDestination destinationBuffer;
    yaml.stringify(destinationBuffer);
    REQUIRE(
        destinationBuffer.toString() ==
        "---\nroot: \n  - 1\n  - 1\n  - 1\n  - 1.0\n  - 1.0\n  - 1.0\n...\n");
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][0]).is<int>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][1]).is<long>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][2]).is<long long>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][3]).is<float>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][4]).is<double>());
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][5]).is<long double>());
    yaml["root"][1] = 3.0;
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][1]).is<double>());
    destinationBuffer.clear();
    yaml.stringify(destinationBuffer);
    REQUIRE(
        destinationBuffer.toString() ==
        "---\nroot: \n  - 1\n  - 3.0\n  - 1\n  - 1.0\n  - 1.0\n  - 1.0\n...\n");
    REQUIRE_NOTHROW(YRef<Number>(yaml["root"][5]).set(445l));
    REQUIRE_FALSE(!YRef<Number>(yaml["root"][5]).is<long>());
    destinationBuffer.clear();
    yaml.stringify(destinationBuffer);
    REQUIRE(
        destinationBuffer.toString() ==
        "---\nroot: \n  - 1\n  - 3.0\n  - 1\n  - 1.0\n  - 1.0\n  - 445\n...\n");
  }
}
