#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML create complex YAML structures.",
          "[YAML][Create][Complex]") {
  SECTION("A Single level object.", "[YAML][Create][Complex]") {
    YAML yaml;
    yaml["pi"] = 3.141;
    yaml["happy"] = true;
    yaml["name"] = "Niels";
    yaml["nothing"] = nullptr;
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(
        yamlDestination.toString() ==
        "---\npi: 3.141\nhappy: True\nname: \"Niels\"\nnothing: null\n...\n");
  }
  SECTION("A two level object.", "[YAML][Create][Complex]") {
    YAML yaml;
    yaml["pi"] = 3.141;
    yaml["happy"] = true;
    yaml["name"] = "Niels";
    yaml["nothing"] = nullptr;
    yaml["answer"]["everything"] = 42;
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() ==
            "---\npi: 3.141\nhappy: True\nname: \"Niels\"\nnothing: "
            "null\nanswer: \n  everything: 42\n...\n");
  }
  SECTION("A three level object.", "[YAML][Create][Complex]") {
    YAML yaml;
    yaml["pi"] = 3.141;
    yaml["happy"] = true;
    yaml["name"][5] = "Niels";
    yaml["nothing"] = nullptr;
    yaml["answer"]["everything"][5] = 42;
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() ==
            "---\npi: 3.141\nhappy: True\nname: \n  - null\n  - null\n  - "
            "null\n  - null\n  - null\n  - \"Niels\"\nnothing: null\nanswer: "
            "\n  everything: \n    - null\n    - null\n    - null\n    - "
            "null\n    - null\n    - 42\n...\n");
  }
  SECTION("Object with sub array/object create using initializer list.",
          "[YAML][Create][Complex]") {
    YAML yaml;
    yaml["pi"] = 3.141;
    yaml["happy"] = true;
    yaml["name"] = "Niels";
    yaml["nothing"] = nullptr;
    yaml["answer"]["everything"] = 42;
    yaml["list"] = {1, 0, 2};
    yaml["object"] = {{"currency", "USD"}, {"value", 42.99}};
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() ==
            "---\npi: 3.141\nhappy: True\nname: \"Niels\"\nnothing: "
            "null\nanswer: \n  everything: 42\nlist: \n  - 1\n  - 0\n  - "
            "2\nobject: \n  currency: \"USD\"\n  value: 42.99\n...\n");
  }
  SECTION("Object with sub array/object with an embedded array create using "
          "initializer list.",
          "[YAML][Create][Complex]") {
    YAML yaml;
    yaml["pi"] = 3.141;
    yaml["happy"] = true;
    yaml["name"] = "Niels";
    yaml["nothing"] = nullptr;
    yaml["answer"]["everything"] = 42;
    yaml["list"] = {1, 0, 2};
    yaml["object"] = {{"currency", "USD"}, {"value", YNode{1, 2, 3, 4, 5}}};
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(yamlDestination.toString() ==
            "---\npi: 3.141\nhappy: True\nname: \"Niels\"\nnothing: "
            "null\nanswer: \n  everything: 42\nlist: \n  - 1\n  - 0\n  - "
            "2\nobject: \n  currency: \"USD\"\n  value: \n    - 1\n    - 2\n   "
            " - 3\n    - 4\n    - 5\n...\n");
  }
  SECTION("Object with sub array/object with an embedded object create using "
          "initializer list.",
          "[YAML][Create][Complex]") {
    YAML yaml;
    yaml["pi"] = 3.141;
    yaml["happy"] = true;
    yaml["name"] = "Niels";
    yaml["nothing"] = nullptr;
    yaml["answer"]["everything"] = 42;
    yaml["list"] = {1, 0, 2};
    yaml["object"] = {{"currency", "USD"},
                      {"value", YNode{{"key1", 22}, {"key2", 99.899}}}};
    BufferDestination yamlDestination;
    REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
    REQUIRE(
        yamlDestination.toString() ==
        "---\npi: 3.141\nhappy: True\nname: \"Niels\"\nnothing: null\nanswer: "
        "\n  everything: 42\nlist: \n  - 1\n  - 0\n  - 2\nobject: \n  "
        "currency: \"USD\"\n  value: \n    key1: 22\n    key2: 99.899\n...\n");
  }
  //   SECTION("Array creation completely using a initializer list.",
  //   "[YAML][Create][Complex][Initializer")
  //   {
  //     YAML yaml = { 1, 2, 3, 4 };
  //     BufferDestination yamlDestination;
  //     REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
  //     REQUIRE(yamlDestination.toString() == R"([1,2,3,4])");
  //   }
  //   SECTION("Object creation completely using a initializer list.",
  //   "[YAML][Create][Complex][Initializer")
  //   {
  //     YAML yaml = { { "currency", "USD" }, { "value", 42.99 } };
  //     BufferDestination yamlDestination;
  //     REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
  //     REQUIRE(yamlDestination.toString() ==
  //     R"({"currency":"USD","value":42.99})");
  //   }
  //   SECTION("Object creation completely using a nested initializer list.",
  //   "[YAML][Create][Complex][Initializer")
  //   {
  //     // Note: For the moment has to explicitly uses YNode to create a
  //     // nested object/array
  //     YAML yaml = { { "pi", 3.141 },
  //       { "happy", true },
  //       { "name", "Niels" },
  //       { "nothing", nullptr },
  //       { "answer", YNode{ { "everything", 42 } } },
  //       { "list", YNode{ 1, 0, 2 } },
  //       { "object", YNode{ { "currency", "USD" }, { "value", 42.99 } } } };
  //     BufferDestination yamlDestination;
  //     REQUIRE_NOTHROW(yaml.stringify(yamlDestination));
  //     REQUIRE(
  //       yamlDestination.toString()
  //       ==
  //       R"({"pi":3.141,"happy":true,"name":"Niels","nothing":null,"answer":{"everything":42},"list":[1,0,2],"object":{"currency":"USD","value":42.99}})");
  //   }
}