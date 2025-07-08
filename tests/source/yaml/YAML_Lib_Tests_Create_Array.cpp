
#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML array creation api.", "[YAML][Create][Array]") {
  SECTION("Initialise YAML with YAML array  passed to constructor.",
          "[YAML][Create][Constructor]") {
    REQUIRE_NOTHROW(YAML(R"([ "pi", 3.141 ])"));
  }
  SECTION("Initialise YAML with YAML array passed to constructor and validate.",
          "[YAML][Create][Constructor][Validate]") {
    const YAML yaml(R"([ "pi", 3.141 ])");
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[1]));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "pi");
    REQUIRE_FALSE(!equalFloatingPoint(
        NRef<Number>(yaml.document(0)[1]).value<float>(), 3.141f, 0.0001));
  }
  SECTION("Initialise root YAML array with one entry containing a integer.",
          "[YAML][Create][Array][Number]") {
    YAML yaml;
    yaml[0] = 300;
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE(NRef<Number>(yaml.document(0)[0]).value<int>() == 300);
  }
  SECTION("Initialise root YAML array with one entry containing a long.",
          "[YAML][Create][Array][Number]") {
    YAML yaml;
    yaml[0] = 30000l;
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE(NRef<Number>(yaml.document(0)[0]).value<long>() == 30000);
  }
  SECTION("Initialise root YAML array with one entry containing a long long.",
          "[YAML][Create][Array][Number]") {
    YAML yaml;
    yaml[0] = 30000ll;
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE(NRef<Number>(yaml.document(0)[0]).value<long long>() == 30000);
  }
  SECTION("Initialise root YAML array with one entry containing a float.",
          "[YAML][Create][Array][Number]") {
    YAML yaml;
    yaml[0] = 3.141f;
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!equalFloatingPoint(
        NRef<Number>(yaml.document(0)[0]).value<float>(), 3.141f, 0.0001));
  }
  SECTION("Initialise root YAML array with one entry containing a double.",
          "[YAML][Create][Array][Number]") {
    YAML yaml;
    yaml[0] = 3.141l;
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!equalFloatingPoint(
        NRef<Number>(yaml.document(0)[0]).value<long double>(), 3.141l,
        0.0001));
  }
  SECTION("Initialise root YAML array with one entry containing a long double.",
          "[YAML][Create][Array][Number]") {
    YAML yaml;
    yaml[0] = 3.141;
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[0]));
    REQUIRE_FALSE(!equalFloatingPoint(
        NRef<Number>(yaml.document(0)[0]).value<double>(), 3.141, 0.0001));
  }
  SECTION(
      "Initialise root YAML array with one entry containing a const char *.",
      "[YAML][Create][Array][String]") {
    YAML yaml;
    yaml[0] = "robert";
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "robert");
  }
  SECTION("Initialise root YAML array with one entry containing a std::string.",
          "[YAML][Create][Array][String]") {
    YAML yaml;
    yaml[0] = std::string{"robert"};
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "robert");
  }
  SECTION("Initialise root YAML array with one entry containing a boolean.",
          "[YAML][Create][Array][Boolean]") {
    YAML yaml;
    yaml[0] = true;
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)[0]).value());
  }
  SECTION("Initialise root YAML array with one entry containing a null.",
          "[YAML][Create][Array][null]") {
    YAML yaml;
    yaml[0] = nullptr;
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[0]));
    REQUIRE(NRef<Null>(yaml.document(0)[0]).value() == nullptr);
  }
  SECTION("Create two level array with null at the base and stringify.",
          "[YAML][Create][Array][null]") {
    YAML yaml;
    yaml[0][0] = nullptr;
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[0][0]));
    REQUIRE(NRef<Null>(yaml.document(0)[0][0]).value() == nullptr);
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() == "---\n- - null\n...\n");
  }
  SECTION("Create array with free spaces string at the base and stringify.",
          "[YAML][Create][Array][null]") {
    YAML yaml;
    yaml[5] = "test";
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[5]));
    REQUIRE(NRef<String>(yaml.document(0)[5]).value() == "test");
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() ==
            "---\n- null\n- null\n- null\n- null\n- null\n- \"test\"\n...\n");
  }
  SECTION(
      "Create array with free spaces add an number at the base and stringify.",
      "[YAML][Create][Array][null]") {
    YAML yaml;
    yaml[5] = "test";
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[5]));
    REQUIRE(NRef<String>(yaml.document(0)[5]).value() == "test");
    yaml[3] = 15;
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[3]));
    REQUIRE(NRef<Number>(yaml.document(0)[3]).value<int>() == 15);
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() ==
            "---\n- null\n- null\n- null\n- 15\n- null\n- \"test\"\n...\n");
  }
  SECTION("Create array with initializer list assignment.",
          "[YAML][Create][Array][initializer]") {
    YAML yaml;
    yaml[5] = {1.0,   2.0,    3, 4.333, "5.0", "test test test test",
               false, nullptr};
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[5][0]));
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[5][1]));
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[5][2]));
    REQUIRE_FALSE(!isA<Number>(yaml.document(0)[5][3]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[5][4]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[5][5]));
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[5][6]));
    REQUIRE_FALSE(!isA<Null>(yaml.document(0)[5][7]));
    REQUIRE(NRef<Array>(yaml.document(0)[5]).size() == 8);
    REQUIRE_FALSE(!equalFloatingPoint(
        NRef<Number>(yaml.document(0)[5][0]).value<double>(), 1.0, 0.0001));
    REQUIRE_FALSE(!equalFloatingPoint(
        NRef<Number>(yaml.document(0)[5][1]).value<double>(), 2.0, 0.0001));
    REQUIRE(NRef<Number>(yaml.document(0)[5][2]).value<int>() == 3);
    REQUIRE_FALSE(!equalFloatingPoint(
        NRef<Number>(yaml.document(0)[5][3]).value<double>(), 4.333, 0.0001));
    REQUIRE(NRef<String>(yaml.document(0)[5][4]).value() == "5.0");
    REQUIRE(NRef<String>(yaml.document(0)[5][5]).value() ==
            "test test test test");
    REQUIRE_FALSE(NRef<Boolean>(yaml.document(0)[5][6]).value());
    REQUIRE(NRef<Null>(yaml.document(0)[5][7]).value() == nullptr);
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() ==
            "---\n- null\n- null\n- null\n- null\n- null\n- - 1.0\n  - 2.0\n  "
            "- 3\n  - 4.333\n  - \"5.0\"\n  - \"test test test test\"\n  - "
            "False\n  - null\n...\n");
  }
}
