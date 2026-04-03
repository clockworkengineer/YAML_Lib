#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML anchor edge cases and binary tag.",
          "[YAML][Parse][Anchors]") {
  const YAML yaml;

  // ---- Undefined alias error handling ----

  SECTION("YAML parse undefined alias throws SyntaxError.",
          "[YAML][Parse][Anchors][UndefinedAlias]") {
    BufferSource source{"---\n*undefinedAlias\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML parse undefined alias in dictionary value throws SyntaxError.",
          "[YAML][Parse][Anchors][UndefinedAlias]") {
    BufferSource source{"---\nkey: *missing\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML parse undefined alias in sequence throws SyntaxError.",
          "[YAML][Parse][Anchors][UndefinedAlias]") {
    BufferSource source{"---\n- *unknown\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- Override (<<) with undefined alias ----

  SECTION("YAML override with undefined alias throws SyntaxError.",
          "[YAML][Parse][Anchors][UndefinedAlias]") {
    BufferSource source{"---\nbase:\n  <<: *missing-base\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- Valid anchor/alias still works ----

  SECTION("YAML parse valid anchor and alias does not throw.",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\nfoo: &anchor bar\nbaz: *anchor\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["baz"]).value() == "bar");
  }

  SECTION("YAML anchor defined before use resolves correctly.",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{
        "---\ndefaults: &defaults\n  adapter: postgres\n  encoding: "
        "unicode\ndevelopment:\n  <<: *defaults\n  database: myapp_dev\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Dictionary>(yaml.document(0)["development"]));
    REQUIRE(NRef<String>(yaml.document(0)["development"]["adapter"]).value() ==
            "postgres");
    REQUIRE(NRef<String>(yaml.document(0)["development"]["database"]).value() ==
            "myapp_dev");
  }

  // ---- !!binary tag ----

  SECTION("YAML !!binary tag preserves base64 string as String node.",
          "[YAML][Parse][Tags][Binary]") {
    BufferSource source{"---\n!!binary SGVsbG8gV29ybGQ=\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "SGVsbG8gV29ybGQ=");
    REQUIRE(yaml.document(0).getVariant().getTag() ==
            "tag:yaml.org,2002:binary");
  }

  SECTION("YAML !!binary tag in dictionary value.",
          "[YAML][Parse][Tags][Binary]") {
    BufferSource source{"---\ndata: !!binary AAEC\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["data"]));
    REQUIRE(NRef<String>(yaml.document(0)["data"]).value() == "AAEC");
    REQUIRE(yaml.document(0)["data"].getVariant().getTag() ==
            "tag:yaml.org,2002:binary");
  }
}
