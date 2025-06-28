#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check IDestination (File) interface.",
          "[YAML][IDestination][File]") {
  SECTION("Create FileDestination.", "[YAML][IDestination][File][Construct]") {
    std::string testFileName{generateRandomFileName()};
    REQUIRE_NOTHROW(FileDestination(testFileName));
    std::filesystem::remove(testFileName);
  }
  SECTION("Create FileDestination and get source which should be empty.",
          "[YAML][IDestination][File][Construct]") {
    std::string testFileName{generateRandomFileName()};
    FileDestination source(testFileName);
    source.close();
    REQUIRE(source.size() == 0);
    std::filesystem::remove(testFileName);
  }
  SECTION("Create FileDestination and add one character.",
          "[YAML][IDestination][File][Add]") {
    std::string testFileName{generateRandomFileName()};
    FileDestination source(testFileName);
    source.add('i');
    source.close();
    REQUIRE(source.size() == 1);
    std::filesystem::remove(testFileName);
  }
  SECTION("Create FileDestination and add an integer string and check result.",
          "[YAML][IDestination][File][Add]") {
    std::string testFileName{generateRandomFileName()};
    FileDestination source(testFileName);
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", testFileName));
    std::filesystem::remove(testFileName);
  }
  SECTION("Create FileDestination, add to it, clear source and then add to it "
          "again and check result.",
          "[YAML][IDestination][File][Clear]") {
    std::string testFileName{generateRandomFileName()};
    FileDestination source(testFileName);
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", testFileName));
    source.clear();
    REQUIRE(source.size() == 0);
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", testFileName));
    std::filesystem::remove(testFileName);
  }
  SECTION("Create FileDestination and and add content with linefeeds.",
          "[YAML][IDestination][File][Linefeed]") {
    std::string testFileName{generateRandomFileName()};
    FileDestination source(testFileName);
    source.add("65767\n");
    source.add("22222\n");
    source.add("33333\n");
    REQUIRE(source.last() == kLineFeed);
    REQUIRE(source.size() == 21);
    source.close();
    REQUIRE_FALSE(!compareFile("65767\r\n22222\r\n33333\r\n", testFileName));
    std::filesystem::remove(testFileName);
  }
  SECTION("Create FileDestination, add to it, clear source and then add to it "
          "again and check result and testing last() along the way.",
          "[YAML][IDestination][File][Clear]") {
    std::string testFileName{generateRandomFileName()};
    FileDestination source(testFileName);
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", testFileName));
    REQUIRE(source.last() == '7');
    source.clear();
    REQUIRE(source.last() == kNull);
    REQUIRE(source.size() == 0);
    source.add("65767");
    REQUIRE(source.size() == 5);
    source.close();
    REQUIRE_FALSE(!compareFile("65767", testFileName));
    REQUIRE(source.last() == '7');
    std::filesystem::remove(testFileName);
  }
}
