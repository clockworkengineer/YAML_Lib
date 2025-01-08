#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML object creation api.", "[YAML][Create][Dictionary]") {
  SECTION("Initialise YAML with YAML object passed to constructor.",
          "[YAML][Create][Constructor]") {
    REQUIRE_NOTHROW(YAML(R"({ "pi" : 3.141 })"));
  }
  SECTION(
      "Initialise YAML with YAML object passed to constructor and validate.",
      "[YAML][Create][Constructor][Validate]") {
    const YAML yaml(R"({ "pi" : 3.141 })");
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)).contains("pi"));
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)["pi"]));
    REQUIRE_FALSE(!equalFloatingPoint(
        YRef<Number>(yaml.document(0)["pi"]).value<float>(), 3.141f,
        0.0001));
  }
  SECTION(
      "Initialise root YAML Dictionary with one entry containing a integer.",
      "[YAML][Create][Dictionary][Number]") {
    YAML yaml;
    yaml["integer"] = 300;
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)["integer"]));
    REQUIRE(YRef<Number>(yaml.document(0)["integer"]).value<int>() == 300);
  }
  SECTION("Initialise root YAML Dictionary with one entry containing a long.",
          "[YAML][Create][Dictionary][Number]") {
    YAML yaml;
    yaml["integer"] = 30000l;
    REQUIRE_FALSE(!isA<Number>(yaml["integer"]));
    REQUIRE(YRef<Number>(yaml.document(0)["integer"]).value<long>() ==
            30000);
  }
  SECTION("Initialise root YAML Dictionary with one entry containing a float.",
          "[YAML][Create][Dictionary][Number]") {
    YAML yaml;
    yaml["pi"] = 3.141f;
    REQUIRE_FALSE(!isA<Number>(yaml["pi"]));
    REQUIRE_FALSE(!equalFloatingPoint(
        YRef<Number>(yaml.document(0)["pi"]).value<float>(), 3.141f,
        0.0001));
  }
  SECTION("Initialise root YAML Dictionary with one entry containing a double.",
          "[YAML][Create][Dictionary][Number]") {
    YAML yaml;
    yaml["pi"] = 3.141;
    REQUIRE_FALSE(!isA<Number>(yaml["pi"]));
    REQUIRE_FALSE(!equalFloatingPoint(
        YRef<Number>(yaml.document(0)["pi"]).value<double>(), 3.141,
        0.0001));
  }
  SECTION("Initialise root YAML Dictionary with one entry containing a const "
          "char *.",
          "[YAML][Create][Dictionary][String]") {
    YAML yaml;
    yaml["name"] = "robert";
    REQUIRE_FALSE(!isA<String>(yaml["name"]));
    REQUIRE(YRef<String>(yaml.document(0)["name"]).value() == "robert");
  }
  SECTION("Initialise root YAML Dictionary with one entry containing a "
          "std::string.",
          "[YAML][Create][Dictionary][String]") {
    YAML yaml;
    yaml["name"] = std::string{"robert"};
    REQUIRE_FALSE(!isA<String>(yaml["name"]));
    REQUIRE(YRef<String>(yaml.document(0)["name"]).value() == "robert");
  }
  SECTION(
      "Initialise root YAML Dictionary with one entry containing a boolean.",
      "[YAML][Create][Dictionary][Boolean]") {
    YAML yaml;
    yaml["flag"] = true;
    REQUIRE_FALSE(!isA<Boolean>(yaml["flag"]));
    REQUIRE_FALSE(!YRef<Boolean>(yaml.document(0)["flag"]).value());
  }
  SECTION("Initialise root YAML Dictionary with one entry containing a null.",
          "[YAML][Create][Dictionary][null]") {
    YAML yaml;
    yaml["nothing"] = nullptr;
    REQUIRE_FALSE(!isA<Null>(yaml["nothing"]));
    REQUIRE(YRef<Null>(yaml.document(0)["nothing"]).value() == nullptr);
  }
  SECTION("Create two level object and null at base.",
          "[YAML][Create][Dictionary][null]") {
    YAML yaml;
    yaml["nothing"]["extra"] = nullptr;
    REQUIRE_FALSE(!isA<Null>(yaml["nothing"]["extra"]));
    REQUIRE(YRef<Null>(yaml.document(0)["nothing"]["extra"]).value() ==
            nullptr);
  }
  SECTION("Create three level object and null at base and stringify.",
          "[YAML][Create][Dictionary][null]") {
    YAML yaml;
    yaml["nothing"]["extra"]["more"] = nullptr;
    REQUIRE_FALSE(!isA<Null>(yaml["nothing"]["extra"]["more"]));
    REQUIRE(
        YRef<Null>(yaml.document(0)["nothing"]["extra"]["more"]).value() ==
        nullptr);
    BufferDestination destinationBuffer;
    REQUIRE_NOTHROW(yaml.stringify(destinationBuffer));
    REQUIRE(destinationBuffer.toString() ==
            "---\nnothing: \n  extra: \n    more: null\n...\n");
  }
}