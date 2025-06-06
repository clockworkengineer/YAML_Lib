#include "YAML_Lib_Tests.hpp"

TEST_CASE("Checks for toFile() api.", "[YAML][ToFile]") {
  SECTION("Check that toFile() works with UTF8.", "[YAML][ToFile][UTF8]") {
    std::string testFile{prefixTestDataPath(kGeneratedYAMLFile)};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    YAML::toFile(testFile, expected);
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  SECTION("Check that toFile() works with UTF8BOM.",
          "[YAML][ToFile][UTF8BOM]") {
    std::string testFile{prefixTestDataPath(kGeneratedYAMLFile)};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    YAML::toFile(testFile, expected, YAML::Format::utf8BOM);
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  SECTION("Check that toFile() works with UTF16BE.",
          "[YAML][ToFile][UTF16BE]") {
    std::string testFile{prefixTestDataPath(kGeneratedYAMLFile)};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    YAML::toFile(testFile, expected, YAML::Format::utf16BE);
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  SECTION("Check that toFile() works with UTF16BLE.",
          "[YAML][ToFile][UTF16LE]") {
    std::string testFile{prefixTestDataPath(kGeneratedYAMLFile)};
    std::string expected{
        "version: \"3.9\"\n\nservices:\n  production-db:\n    image: "
        "mysql:5.7\n    volumes:\n      - db_data:/var/lib/mysql\n    restart: "
        "always\n    environment:\n  MYSQL_ROOT_PASSWORD: somewordpress\n  "
        "MYSQL_DATABASE: wordpress\n  MYSQL_USER: wordpress\n  MYSQL_PASSWORD: "
        "wordpress\n  test-db:\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n    "
        "MYSQL_ROOT_PASSWORD: somewordpress\n    MYSQL_DATABASE: wordpress\n   "
        " MYSQL_USER: wordpress\n    MYSQL_PASSWORD: wordpress\n"};
    YAML::toFile(testFile, expected, YAML::Format::utf16LE);
    REQUIRE(YAML::fromFile(testFile) == expected);
  }
  // SECTION("Check that toFile() works with UTF32BE.",
  // "[YAML][ToFile][UTF32BE]")
  // {
  //     std::string testFile{ prefixPath(kGeneratedYAMLFile) };
  //     std::string expected{ R"([true  , "Out of time",  7.89043e+18, true])"
  //     }; REQUIRE_THROWS_WITH(YAML::toFile(testFile, expected,
  //     YAML::Format::utf32BE),
  //                         "YAML Error: Unsupported YAML file format (Byte
  //                         Order Mark) specified.");
  // }
  // SECTION("Check that toFile() works with UTF32LE.",
  // "[YAML][ToFile][UTF32LE]")
  // {
  //     std::string testFile{ prefixPath(kGeneratedYAMLFile) };
  //     std::string expected{ R"([true  , "Out of time",  7.89043e+18, true])"
  //     }; REQUIRE_THROWS_WITH(YAML::toFile(testFile, expected,
  //     YAML::Format::utf32LE),
  //                         "YAML Error: Unsupported YAML file format (Byte
  //                         Order Mark) specified.");
  // }
}
