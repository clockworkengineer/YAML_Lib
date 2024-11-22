#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of Anchors.", "[YAML][Parse][Anchors]") {
  const YAML yaml;
  SECTION("YAML parse array with one simple anchor.",
          "[YAML][Parse][Anchors]") {
    BufferSource source{
        "----\nhr:\n  - Mark McGwire\n  # Following node labeled SS\n  - &SS "
        "Sammy Sosa\nrbi:\n  - *SS # Subsequent occurance\n  - Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("hr"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("rbi"));
    REQUIRE(YRef<String>(yaml.document(0)[0]["hr"][0]).value() ==
            "Mark McGwire");
    //     REQUIRE_FALSE(!isA<Anchor>(yaml.document(0)[0]["hr"][1]));
    //     REQUIRE(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).toString() ==
    //             "Sammy Sosa");
    //     REQUIRE_FALSE(
    //         !isA<String>(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).value()));
    //     REQUIRE(YRef<String>(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).value())
    //                 .value() == "Sammy Sosa");
    //     REQUIRE_FALSE(!isA<Alias>(yaml.document(0)[0]["rbi"][0]));
    //     REQUIRE(YRef<String>(YRef<Alias>(yaml.document(0)[0]["rbi"][0]).value())
    //                 .value() == "Sammy Sosa");
    //     REQUIRE(YRef<Alias>(yaml.document(0)[0]["rbi"][0]).toString() ==
    //             "Sammy Sosa");
    //     REQUIRE(YRef<Alias>(yaml.document(0)[0]["rbi"][0]).getName() ==
    //     "SS");
    REQUIRE(YRef<String>(yaml.document(0)[0]["rbi"][1]).value() ==
            "Ken Griffey");
  }
  SECTION("YAML parse array with one complex anchor.",
          "[YAML][Parse][Anchors]") {
    BufferSource source{
        "version: \"3.9\"\n\nservices:\n  production-db: "
        "&database-definition\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n  "
        "MYSQL_ROOT_PASSWORD: somewordpress\n  MYSQL_DATABASE: wordpress\n  "
        "MYSQL_USER: wordpress\n  MYSQL_PASSWORD: wordpress\n   \n  test-db: "
        "*database-definition\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\nversion: \"3.9\"\nservices: \n  production-db: \n    image: "
            "mysql:5.7\n    volumes: \n      - db_data:/var/lib/mysql\n    "
            "restart: always\n    environment: null\n  MYSQL_ROOT_PASSWORD: "
            "somewordpress\n  MYSQL_DATABASE: wordpress\n  MYSQL_USER: "
            "wordpress\n  MYSQL_PASSWORD: wordpress\n  test-db: \n    image: "
            "mysql:5.7\n    volumes: \n      - db_data:/var/lib/mysql\n    "
            "restart: always\n    environment: null\n...\n");
  }
}