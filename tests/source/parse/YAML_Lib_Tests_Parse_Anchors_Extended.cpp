#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML anchor edge cases and binary tag.",
          "[YAML][Parse][Anchors]") {
  const YAML yaml;

  // ---- Multi-alias merge key: <<: [*a, *b] ----

  SECTION("YAML <<: [*a, *b] merges both aliases into one mapping.",
          "[YAML][Parse][Anchors][MultiMerge]") {
    BufferSource source{"---\n"
                        "base: &base\n"
                        "  x: 1\n"
                        "  y: 2\n"
                        "ext: &ext\n"
                        "  z: 3\n"
                        "combined:\n"
                        "  <<: [*base, *ext]\n"
                        "  w: 4\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)["combined"]));
    REQUIRE(NRef<Number>(yaml.document(0)["combined"]["x"]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml.document(0)["combined"]["y"]).value<int>() == 2);
    REQUIRE(NRef<Number>(yaml.document(0)["combined"]["z"]).value<int>() == 3);
    REQUIRE(NRef<Number>(yaml.document(0)["combined"]["w"]).value<int>() == 4);
  }

  SECTION("YAML <<: [*a, *b] first alias has priority on conflicting keys.",
          "[YAML][Parse][Anchors][MultiMerge]") {
    BufferSource source{"---\n"
                        "a: &a\n"
                        "  color: red\n"
                        "  size: large\n"
                        "b: &b\n"
                        "  color: blue\n"
                        "  weight: heavy\n"
                        "merged:\n"
                        "  <<: [*a, *b]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    // *a's color: red takes priority over *b's color: blue
    REQUIRE(NRef<String>(yaml.document(0)["merged"]["color"]).value() == "red");
    REQUIRE(NRef<String>(yaml.document(0)["merged"]["size"]).value() ==
            "large");
    REQUIRE(NRef<String>(yaml.document(0)["merged"]["weight"]).value() ==
            "heavy");
  }

  SECTION("YAML <<: [*a, *b] explicit local keys override all merged keys.",
          "[YAML][Parse][Anchors][MultiMerge]") {
    BufferSource source{"---\n"
                        "a: &a\n"
                        "  x: 10\n"
                        "b: &b\n"
                        "  x: 20\n"
                        "  y: 30\n"
                        "result:\n"
                        "  <<: [*a, *b]\n"
                        "  x: 99\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    // Local x: 99 overrides both *a's x: 10 and *b's x: 20
    REQUIRE(NRef<Number>(yaml.document(0)["result"]["x"]).value<int>() == 99);
    REQUIRE(NRef<Number>(yaml.document(0)["result"]["y"]).value<int>() == 30);
  }

  // ---- Nested anchors (inner anchor accessible via its own alias) ----

  SECTION("YAML inner anchor defined inside outer anchor is accessible.",
          "[YAML][Parse][Anchors][Nested]") {
    BufferSource source{"---\n"
                        "outer: &outer\n"
                        "  inner: &inner\n"
                        "    value: 42\n"
                        "copy_inner: *inner\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)["copy_inner"]));
    REQUIRE(
        NRef<Number>(yaml.document(0)["copy_inner"]["value"]).value<int>() ==
        42);
  }

  SECTION("YAML anchor of a sequence resolves to array node.",
          "[YAML][Parse][Anchors][Sequence]") {
    BufferSource source{"---\n"
                        "items: &items\n"
                        "  - one\n"
                        "  - two\n"
                        "  - three\n"
                        "copy: *items\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)["copy"]));
    REQUIRE(NRef<Array>(yaml.document(0)["copy"]).size() == 3);
    REQUIRE(NRef<String>(yaml.document(0)["copy"][0]).value() == "one");
    REQUIRE(NRef<String>(yaml.document(0)["copy"][2]).value() == "three");
  }

  SECTION("YAML alias used multiple times creates independent copies.",
          "[YAML][Parse][Anchors][Reuse]") {
    BufferSource source{"---\n"
                        "template: &tmpl\n"
                        "  x: 1\n"
                        "  y: 2\n"
                        "first: *tmpl\n"
                        "second: *tmpl\n"
                        "third: *tmpl\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<Number>(yaml.document(0)["first"]["x"]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml.document(0)["second"]["y"]).value<int>() == 2);
    REQUIRE(NRef<Number>(yaml.document(0)["third"]["x"]).value<int>() == 1);
  }

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

  SECTION("YAML mapping with anchor on document start line throws.",
          "[YAML][Parse][Anchors][Invalid]") {
    BufferSource source{"--- &anchor a: b\n"};
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

  SECTION(
      "YAML anchor with standalone tag token captures following block value.",
      "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n&a4 !!map\n&a5 !!str key5: value4\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["key5"]).value() == "value4");
  }

  SECTION("YAML anchor on its own line captures same-indentation tagged node.",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n&a1\n!!str\nscalar1\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "scalar1");
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:str");
  }

  SECTION("YAML !!map with tagged anchored key parses.",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n!!map\n!!str &a10 key10: value9\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["key10"]).value() == "value9");
  }

  SECTION("YAML stream preserves anchor and tag parsing across documents.",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n"
                        "&a1\n"
                        "!!str\n"
                        "scalar1\n"
                        "---\n"
                        "!!str\n"
                        "&a2\n"
                        "scalar2\n"
                        "---\n"
                        "&a3\n"
                        "!!str scalar3\n"
                        "---\n"
                        "&a4 !!map\n"
                        "&a5 !!str key5: value4\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 4);
    REQUIRE(isA<Dictionary>(yaml.document(3)));
    REQUIRE(NRef<String>(yaml.document(3)["key5"]).value() == "value4");
  }

  SECTION("YAML !!str followed by anchored scalar on next line parses.",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n!!str\n&a2\nscalar2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "scalar2");
  }

  SECTION(
      "YAML tag followed by anchor and scalar parses for core tag coercion.",
      "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n- &a !!str a\n- !!int &c 4\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "a");
    REQUIRE(yaml.document(0)[0].getVariant().getTag() ==
            "tag:yaml.org,2002:str");
    REQUIRE(isA<Number>(yaml.document(0)[1]));
    REQUIRE(NRef<Number>(yaml.document(0)[1]).value<int>() == 4);
    REQUIRE(yaml.document(0)[1].getVariant().getTag() ==
            "tag:yaml.org,2002:int");
  }

  SECTION("YAML anchor followed by tagged scalar on next line parses.",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n&a3\n!!str scalar3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "scalar3");
  }

  SECTION("YAML tags on empty scalars parse correctly (FH7J).",
          "[YAML][Parse][Anchors][Valid]") {
    BufferSource source{"---\n"
                        "- !!str\n"
                        "-\n"
                        "  !!null : a\n"
                        "  b: !!str\n"
                        "- !!str : !!null\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)[0]));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "");
    REQUIRE(isA<Dictionary>(yaml.document(0)[1]));
    REQUIRE(NRef<String>(yaml.document(0)[1]["b"]).value() == "");
    REQUIRE(isA<Dictionary>(yaml.document(0)[2]));
    REQUIRE(isA<Null>(yaml.document(0)[2][""]));
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
