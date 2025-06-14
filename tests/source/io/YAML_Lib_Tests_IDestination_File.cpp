#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check IDestination (File) interface.",
          "[YAML][IDestination][File]") {
  SECTION("Create FileDestination.", "[YAML][IDestination][File][Construct]") {
    REQUIRE_NOTHROW(FileDestination(prefixTestDataPath(kGeneratedYAMLFile)));
  }
  SECTION("Create FileDestination and get source which should be empty.",
          "[YAML][IDestination][File][Construct]") {
    FileDestination source(prefixTestDataPath(kGeneratedYAMLFile));
    REQUIRE(source.size() == 0);
  }
  SECTION("Create FileDestination and add one character.",
          "[YAML][IDestination][File][Add]") {
    FileDestination source(prefixTestDataPath(kGeneratedYAMLFile));
    source.add('i');
    REQUIRE(source.size() == 1);
  }
  SECTION("Create FileDestination and add an integer string and check result.",
          "[YAML][IDestination][File][Add]") {
    FileDestination source(prefixTestDataPath(kGeneratedYAMLFile));
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", prefixTestDataPath(kGeneratedYAMLFile)));
  }
  SECTION("Create FileDestination, add to it, clear source and then add to it "
          "again and check result.",
          "[YAML][IDestination][File][Clear]") {
    FileDestination source(prefixTestDataPath(kGeneratedYAMLFile));
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", prefixTestDataPath(kGeneratedYAMLFile)));
    source.clear();
    REQUIRE(source.size() == 0);
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", prefixTestDataPath(kGeneratedYAMLFile)));
  }
  SECTION("Create FileDestination and and add content with linefeeds.",
          "[YAML][IDestination][File][Linefeed]") {
    FileDestination source(prefixTestDataPath(kGeneratedYAMLFile));
    source.add("65767\n");
    source.add("22222\n");
    source.add("33333\n");
    REQUIRE(source.last() == kLineFeed);
    REQUIRE(source.size() == 21);
    source.close();
    REQUIRE_FALSE(!compareFile("65767\r\n22222\r\n33333\r\n",
                               prefixTestDataPath(kGeneratedYAMLFile)));
  }
  SECTION("Create FileDestination, add to it, clear source and then add to it "
          "again and check result and testing last() along the way.",
          "[YAML][IDestination][File][Clear]") {
    FileDestination source(prefixTestDataPath(kGeneratedYAMLFile));
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", prefixTestDataPath(kGeneratedYAMLFile)));
    REQUIRE(source.last() == '7');
    source.clear();
    REQUIRE(source.last() == kNull);
    REQUIRE(source.size() == 0);
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", prefixTestDataPath(kGeneratedYAMLFile)));
    REQUIRE(source.last() == '7');
  }
}
