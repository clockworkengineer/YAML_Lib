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
    REQUIRE_FALSE(!isA<Anchor>(yaml.document(0)[0]["hr"][1]));
    REQUIRE(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).toString() ==
            "Sammy Sosa");
    REQUIRE_FALSE(
        !isA<String>(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).value()));
    REQUIRE(YRef<String>(YRef<Anchor>(yaml.document(0)[0]["hr"][1]).value())
                .value() == "Sammy Sosa");
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["rbi"][0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]["rbi"][0]).value() ==
            "Sammy Sosa");
    REQUIRE(YRef<String>(yaml.document(0)[0]["rbi"][1]).value() ==
            "Ken Griffey");
  }
  SECTION("YAML parse array with one complex anchor (example 1).",
          "[YAML][Parse][Anchors]") {
    BufferSource source{
        "version: \"3.9\"\n\nservices:\n  production-db: "
        "&database-definition\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment:\n      "
        "MYSQL_ROOT_PASSWORD: somewordpress\n      MYSQL_DATABASE: wordpress\n "
        "     MYSQL_USER: wordpress\n      MYSQL_PASSWORD: wordpress\n  "
        "test-db: *database-definition\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\nversion: \"3.9\"\nservices: \n  production-db: \n    image: "
            "mysql:5.7\n    volumes: \n      - db_data:/var/lib/mysql\n    "
            "restart: always\n    environment: \n      MYSQL_ROOT_PASSWORD: "
            "somewordpress\n      MYSQL_DATABASE: wordpress\n      MYSQL_USER: "
            "wordpress\n      MYSQL_PASSWORD: wordpress\n  test-db: \n    "
            "image: mysql:5.7\n    volumes: \n      - db_data:/var/lib/mysql\n "
            "   restart: always\n    environment: \n      MYSQL_ROOT_PASSWORD: "
            "somewordpress\n      MYSQL_DATABASE: wordpress\n      MYSQL_USER: "
            "wordpress\n      MYSQL_PASSWORD: wordpress\n...\n");
  }
  SECTION("YAML parse array with one complex anchor (example 2).",
          "[YAML][Parse][Anchors]") {
    BufferSource source{
        "definitions: \n  steps:\n    - step: &build-test\n        name: Build "
        "and test\n        script:\n          - mvn package\n        "
        "artifacts:\n          - target/**\n\npipelines:\n  branches:\n    "
        "develop:\n      - step: *build-test\n    master:\n      - step: "
        "*build-test\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\ndefinitions: \n  steps: \n    - step: \n        name: Build "
            "and test\n        script: \n          - mvn package\n        "
            "artifacts: \n          - target/**\npipelines: \n  branches: \n   "
            " develop: \n      - step: \n          name: Build and test\n      "
            "    script: \n            - mvn package\n          artifacts: \n  "
            "          - target/**\n    master: \n      - step: \n          "
            "name: Build and test\n          script: \n            - mvn "
            "package\n          artifacts: \n            - target/**\n...\n");
  }
  SECTION("YAML parse array with one complex anchor and overrides (example 1).",
          "[YAML][Parse][Anchors]") {
    BufferSource source{
        "\nversion: \"3.9\"\n\nservices:\n  production-db: "
        "&database-definition\n    image: mysql:5.7\n    volumes:\n      - "
        "db_data:/var/lib/mysql\n    restart: always\n    environment: "
        "&environment-definition\n      MYSQL_ROOT_PASSWORD: somewordpress\n   "
        "   MYSQL_DATABASE: wordpress\n      MYSQL_USER: wordpress\n      "
        "MYSQL_PASSWORD: production-password\n  test-db:\n    <<: "
        "*database-definition\n    environment:\n      <<: "
        "*environment-definition\n      MYSQL_PASSWORD: test-password"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(
        destination.toString() ==
        "---\nversion: \"3.9\"\nservices: \n  production-db: \n    image: "
        "mysql:5.7\n    volumes: \n      - db_data:/var/lib/mysql\n    "
        "restart: always\n    environment: \n      MYSQL_ROOT_PASSWORD: "
        "somewordpress\n      MYSQL_DATABASE: wordpress\n      MYSQL_USER: "
        "wordpress\n      MYSQL_PASSWORD: production-password\n  test-db: \n   "
        " image: mysql:5.7\n    volumes: \n      - db_data:/var/lib/mysql\n    "
        "restart: always\n    environment: \n      MYSQL_ROOT_PASSWORD: "
        "somewordpress\n      MYSQL_DATABASE: wordpress\n      MYSQL_USER: "
        "wordpress\n      MYSQL_PASSWORD: test-password\n...\n");
  }
  //   SECTION("YAML parse array with one complex anchor and overrides (example
  //   2).",
  //           "[YAML][Parse][Anchors]") {
  //     BufferSource source{
  //         "version: \"3.9\"\n\nservices:\n  production-db: "
  //         "&database-definition\n    image: mysql:5.7\n    volumes:\n      -
  //         " "db_data:/var/lib/mysql\n    restart: always\n    environment: "
  //         "&environment-definition\n      MYSQL_ROOT_PASSWORD:
  //         somewordpress\n   " "   MYSQL_DATABASE: wordpress\n MYSQL_USER:
  //         wordpress\n      " "MYSQL_PASSWORD: production-password\n
  //         test-db:\n    <<: "
  //         "*database-definition\n    environment:\n      <<: "
  //         "*environment-definition\n      MYSQL_PASSWORD: test-password\n "
  //         "MYSQL_EXTRA: test"};
  //     REQUIRE_NOTHROW(yaml.parse(source));
  //     BufferDestination destination;
  //     REQUIRE_NOTHROW(yaml.stringify(destination));
  //     REQUIRE(destination.toString() == "");
  //   }
}