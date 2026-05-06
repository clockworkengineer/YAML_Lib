#include "YAML_Lib_Tests.hpp"

TEST_CASE("YAML::Options enables strict boolean parsing", "[YAML][Options][Parse]") {
  ::YAML_Lib::Options options;
  options.strictBooleans = true;

  ::YAML_Lib::YAML yaml(options);
  ::YAML_Lib::BufferSource src{"---\nvalue: yes\n"};

  REQUIRE_NOTHROW(yaml.parse(src));
  REQUIRE(::YAML_Lib::isA<::YAML_Lib::String>(yaml.document(0)["value"]));
  REQUIRE(::YAML_Lib::NRef<::YAML_Lib::String>(yaml.document(0)["value"]).value() == "yes");

  ::YAML_Lib::YAML::setStrictBooleans(false);
}

TEST_CASE("YAML root numeric operator[] grows array without exception", "[YAML][Options][Index]") {
  ::YAML_Lib::YAML yaml;
  yaml[2] = 42;

  REQUIRE(::YAML_Lib::isA<::YAML_Lib::Array>(yaml.document(0)));
  REQUIRE(::YAML_Lib::NRef<::YAML_Lib::Array>(yaml.document(0)).size() == 3);
  REQUIRE(::YAML_Lib::isA<::YAML_Lib::Number>(yaml.document(0)[2]));
  REQUIRE(::YAML_Lib::NRef<::YAML_Lib::Number>(yaml.document(0)[2]).value<int>() == 42);
}

TEST_CASE("YAML root string operator[] creates a dictionary entry safely", "[YAML][Options][Index]") {
  ::YAML_Lib::YAML yaml;
  yaml["name"] = "Alice";

  REQUIRE(::YAML_Lib::isA<::YAML_Lib::Dictionary>(yaml.document(0)));
  REQUIRE(::YAML_Lib::isA<::YAML_Lib::String>(yaml.document(0)["name"]));
  REQUIRE(::YAML_Lib::NRef<::YAML_Lib::String>(yaml.document(0)["name"]).value() == "Alice");
}
