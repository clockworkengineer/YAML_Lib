#include "YAML_Lib_Tests.hpp"
#include <filesystem>

#ifdef YAML_LIB_FILE_IO
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
  SECTION("Check that fromFile() rejects missing or unreadable files.", "[YAML][FromFile][Error]") {
    const auto missingFile = std::filesystem::temp_directory_path() / "yaml_lib_missing_file_that_should_not_exist.yaml";
    REQUIRE(!std::filesystem::exists(missingFile));
    REQUIRE_THROWS_AS(YAML::fromFile(missingFile.string()), YAML_Lib::Error);
  }
  SECTION("Check that fromFile() rejects unsupported BOM files.", "[YAML][FromFile][Error][BOM]") {
    const auto tempFile = std::filesystem::temp_directory_path() /
        ("yaml_lib_unsupported_bom_" + generateRandomFileName() + ".yaml");
    std::ofstream out(tempFile, std::ios::binary);
    out.put(static_cast<char>(0x2B));
    out.put(static_cast<char>(0x2F));
    out.put(static_cast<char>(0x76));
    out.put(static_cast<char>(0x38));
    out << "---\nkey: value\n";
    out.close();
    REQUIRE_THROWS_AS(YAML::fromFile(tempFile.string()), YAML_Lib::Error);
    std::filesystem::remove(tempFile);
  }
  SECTION("Check that fromFile() rejects truncated UTF16 input.", "[YAML][FromFile][Error][UTF16]") {
    const auto tempFile = std::filesystem::temp_directory_path() /
        ("yaml_lib_truncated_utf16_" + generateRandomFileName() + ".yaml");
    std::ofstream out(tempFile, std::ios::binary);
    out.put(static_cast<char>(0xFE));
    out.put(static_cast<char>(0xFF));
    out.put(static_cast<char>(0x00));
    out.close();
    REQUIRE_THROWS_AS(YAML::fromFile(tempFile.string()), YAML_Lib::Error);
    std::filesystem::remove(tempFile);
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
#endif // YAML_LIB_FILE_IO
