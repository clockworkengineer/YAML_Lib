#include "YAML_Lib_Tests.hpp"

TEST_CASE("Checks for fromFile() api.", "[YAML][FromFile]") {
  SECTION("Check that fromFile() works with UTF8.", "[YAML][FromFile][UTF8]") {
    std::string testFile{prefixTestDataPath("testfile033.yaml")};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  SECTION("Check that fromFile() works with UTF8BOM.",
          "[YAML][FromFile][UTF8BOM]") {
    std::string testFile{prefixTestDataPath("testfile034.yaml")};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  SECTION("Check that fromFile() works with UTF16BE.",
          "[YAML][FromFile][UTF16BE]") {
    std::string testFile{prefixTestDataPath("testfile035.yaml")};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  SECTION("Check that fromFile() works with UTF16LE.",
          "[YAML][FromFile][UTF16LE]") {
    std::string testFile{prefixTestDataPath("testfile036.yaml")};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  // SECTION("Check that fromFile() works with UTF32BE.",
  // "[YAML][FromFile][UTF32BE]")
  // {
  //     std::string testFile{ prefixPath("testfile025.yaml") };
  //     std::string expected{ R"([true  , "Out of time",  7.89043e+18,
  //     true])" }; REQUIRE_THROWS_WITH(
  //         YAML::fromFile(testFile), "YAML Error: Unsupported YAML file
  //         format (Byte Order Mark) encountered.");
  // }
  // SECTION("Check that fromFile() works with UTF32LE.",
  // "[YAML][FromFile][UTF32LE]")
  // {
  //     std::string testFile{ prefixPath("testfile026.yaml") };
  //     std::string expected{ R"([true  , "Out of time",  7.89043e+18,
  //     true])" }; REQUIRE_THROWS_WITH(
  //         YAML::fromFile(testFile), "YAML Error: Unsupported YAML file
  //         format (Byte Order Mark) encountered.");
  // }
}
