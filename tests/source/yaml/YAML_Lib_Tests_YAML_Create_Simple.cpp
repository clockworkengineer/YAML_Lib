#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML object creation api.", "[YAML][Create][Object]") {
  SECTION("Initialise YAML with YAML object passed to constructor.",
          "[YAML][Create][Constructor]") {
    REQUIRE_NOTHROW(YAML(R"({ "pi" : 3.141 })"));
  }
  SECTION(
      "Initialise YAML with YAML object passed to constructor and validate.",
      "[YAML][Create][Constructor][Validate]") {
    const YAML yaml(R"({ "pi" : 3.141 })");
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("pi"));
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]["pi"]));
    REQUIRE_FALSE(!equalFloatingPoint(
        YRef<Number>(yaml.document(0)[0]["pi"]).value<float>(), 3.141f, 0.0001));
  }
  //   SECTION("Initialise root YAML JNode with one entry containing a
  //   integer.", "[YAML][Create][Object][Number]")
  //   {
  //     YAML yaml;
  //     yaml["integer"] = 300;
  //     REQUIRE_FALSE(!isA<Number>(yaml["integer"]));
  //     REQUIRE(JRef<Number>(yaml.root()["integer"]).value<int>() == 300);
  //   }
  //   SECTION("Initialise root YAML JNode with one entry containing a long.",
  //   "[YAML][Create][Object][Number]")
  //   {
  //     YAML yaml;
  //     yaml["integer"] = 30000l;
  //     REQUIRE_FALSE(!isA<Number>(yaml["integer"]));
  //     REQUIRE(JRef<Number>(yaml.root()["integer"]).value<long>() == 30000);
  //   }
  //   SECTION("Initialise root YAML JNode with one entry containing a float.",
  //   "[YAML][Create][Object][Number]")
  //   {
  //     YAML yaml;
  //     yaml["pi"] = 3.141f;
  //     REQUIRE_FALSE(!isA<Number>(yaml["pi"]));
  //     REQUIRE_FALSE(!equalFloatingPoint(JRef<Number>(yaml.root()["pi"]).value<float>(),
  //     3.141f, 0.0001));
  //   }
  //   SECTION("Initialise root YAML JNode with one entry containing a double.",
  //   "[YAML][Create][Object][Number]")
  //   {
  //     YAML yaml;
  //     yaml["pi"] = 3.141;
  //     REQUIRE_FALSE(!isA<Number>(yaml["pi"]));
  //     REQUIRE_FALSE(!equalFloatingPoint(JRef<Number>(yaml.root()["pi"]).value<double>(),
  //     3.141, 0.0001));
  //   }
  //   SECTION("Initialise root YAML JNode with one entry containing a const
  //   char *.", "[YAML][Create][Object][String]")
  //   {
  //     YAML yaml;
  //     yaml["name"] = "robert";
  //     REQUIRE_FALSE(!isA<String>(yaml["name"]));
  //     REQUIRE(JRef<String>(yaml.root()["name"]).value() == "robert");
  //   }
  //   SECTION("Initialise root YAML JNode with one entry containing a
  //   std::string.", "[YAML][Create][Object][String]")
  //   {
  //     YAML yaml;
  //     yaml["name"] = std::string{ "robert" };
  //     REQUIRE_FALSE(!isA<String>(yaml["name"]));
  //     REQUIRE(JRef<String>(yaml.root()["name"]).value() == "robert");
  //   }
  //   SECTION("Initialise root YAML JNode with one entry containing a
  //   boolean.", "[YAML][Create][Object][Boolean]")
  //   {
  //     YAML yaml;
  //     yaml["flag"] = true;
  //     REQUIRE_FALSE(!isA<Boolean>(yaml["flag"]));
  //     REQUIRE_FALSE(!JRef<Boolean>(yaml.root()["flag"]).value());
  //   }
  //   SECTION("Initialise root YAML JNode with one entry containing a null.",
  //   "[YAML][Create][Object][null]")
  //   {
  //     YAML yaml;
  //     yaml["nothing"] = nullptr;
  //     REQUIRE_FALSE(!isA<Null>(yaml["nothing"]));
  //     REQUIRE(JRef<Null>(yaml.root()["nothing"]).value() == nullptr);
  //   }
  //   SECTION("Create two level object and null at base.",
  //   "[YAML][Create][Object][null]")
  //   {
  //     YAML yaml;
  //     yaml["nothing"]["extra"] = nullptr;
  //     REQUIRE_FALSE(!isA<Null>(yaml["nothing"]["extra"]));
  //     REQUIRE(JRef<Null>(yaml.root()["nothing"]["extra"]).value() ==
  //     nullptr);
  //   }
  //   SECTION("Create three level object and null at base and stringify.",
  //   "[YAML][Create][Object][null]")
  //   {
  //     YAML yaml;
  //     yaml["nothing"]["extra"]["more"] = nullptr;
  //     REQUIRE_FALSE(!isA<Null>(yaml["nothing"]["extra"]["more"]));
  //     REQUIRE(JRef<Null>(yaml.root()["nothing"]["extra"]["more"]).value() ==
  //     nullptr); BufferDestination destinationBuffer;
  //     REQUIRE_NOTHROW(yaml.stringify(destinationBuffer));
  //     REQUIRE(destinationBuffer.toString() ==
  //     R"({"nothing":{"extra":{"more":null}}})");
  //   }
}