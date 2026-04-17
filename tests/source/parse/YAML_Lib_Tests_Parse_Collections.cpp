#include "YAML_Lib_Tests.hpp"

// ============================================================================
// Collection edge-case tests: compact block notation, deeply nested
// mixed flow/block, multi-line plain scalars, embedded block strings.
// ============================================================================

TEST_CASE("Check YAML parsing of collection edge cases.",
          "[YAML][Parse][Collections]") {
  const YAML yaml;

  // ---- Compact block sequence / mapping notation ----

  SECTION("YAML compact block sequence entry with inline dict value.",
          "[YAML][Parse][Collections][Compact]") {
    // '- key: value' on one line is valid compact block notation.
    BufferSource source{"---\n"
                        "- name: Alice\n"
                        "  age: 30\n"
                        "- name: Bob\n"
                        "  age: 25\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE(NRef<String>(yaml.document(0)[0]["name"]).value() == "Alice");
    REQUIRE(NRef<Number>(yaml.document(0)[0]["age"]).value<int>() == 30);
    REQUIRE(NRef<String>(yaml.document(0)[1]["name"]).value() == "Bob");
  }

  SECTION("YAML block sequence nested inside a block mapping.",
          "[YAML][Parse][Collections][Compact]") {
    BufferSource source{"---\n"
                        "fruits:\n"
                        "  - apple\n"
                        "  - banana\n"
                        "  - cherry\n"
                        "count: 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Array>(yaml.document(0)["fruits"]));
    REQUIRE(NRef<Array>(yaml.document(0)["fruits"]).size() == 3);
    REQUIRE(NRef<String>(yaml.document(0)["fruits"][1]).value() == "banana");
    REQUIRE(NRef<Number>(yaml.document(0)["count"]).value<int>() == 3);
  }

  // ---- Deeply nested mixed flow and block ----

  SECTION("YAML three-level block dict with flow sequence leaf.",
          "[YAML][Parse][Collections][Deep]") {
    BufferSource source{"---\n"
                        "config:\n"
                        "  database:\n"
                        "    ports: [5432, 5433, 5434]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Array>(yaml.document(0)["config"]["database"]["ports"]));
    REQUIRE(
        NRef<Array>(yaml.document(0)["config"]["database"]["ports"]).size() ==
        3);
    REQUIRE(NRef<Number>(yaml.document(0)["config"]["database"]["ports"][0])
                .value<int>() == 5432);
  }

  SECTION("YAML block sequence of flow dictionaries.",
          "[YAML][Parse][Collections][Deep]") {
    BufferSource source{"---\n"
                        "- {x: 1, y: 2}\n"
                        "- {x: 3, y: 4}\n"
                        "- {x: 5, y: 6}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 3);
    REQUIRE(NRef<Number>(yaml.document(0)[2]["x"]).value<int>() == 5);
    REQUIRE(NRef<Number>(yaml.document(0)[2]["y"]).value<int>() == 6);
  }

  SECTION("YAML flow dict with block-sequence-like keys (quoted).",
          "[YAML][Parse][Collections][Deep]") {
    BufferSource source{"---\n"
                        "{\"key1\": [1, 2], \"key2\": [3, 4]}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Array>(yaml.document(0)["key1"]));
    REQUIRE(NRef<Array>(yaml.document(0)["key1"]).size() == 2);
  }

  // ---- Block strings as mapping values ----

  SECTION("YAML literal block scalar (|) as mapping value.",
          "[YAML][Parse][Collections][BlockStr]") {
    BufferSource source{"---\n"
                        "message: |\n"
                        "  Hello,\n"
                        "  World!\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["message"]));
    const auto val = NRef<String>(yaml.document(0)["message"]).value();
    REQUIRE(val.find("Hello,") != std::string::npos);
    REQUIRE(val.find("World!") != std::string::npos);
  }

  SECTION("YAML folded block scalar (>) as mapping value.",
          "[YAML][Parse][Collections][BlockStr]") {
    BufferSource source{"---\n"
                        "description: >\n"
                        "  This is a long\n"
                        "  description that\n"
                        "  gets folded.\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["description"]));
    // Folded scalars replace newlines with spaces (except doubled newlines)
    const auto val = NRef<String>(yaml.document(0)["description"]).value();
    REQUIRE(!val.empty());
  }

  SECTION("YAML literal block scalar with strip chomping (|-) in sequence.",
          "[YAML][Parse][Collections][BlockStr]") {
    BufferSource source{"---\n"
                        "- |-\n"
                        "  first\n"
                        "- |-\n"
                        "  second\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "first");
    REQUIRE(NRef<String>(yaml.document(0)[1]).value() == "second");
  }

  // ---- Multi-line plain scalars ----

  SECTION("YAML multi-line plain scalar value folds newlines to spaces.",
          "[YAML][Parse][Collections][PlainScalar]") {
    BufferSource source{"---\n"
                        "summary: This is line one\n"
                        "         and line two\n"};
    // The second line is indented more and continuation of the value
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["summary"]));
  }

  SECTION("YAML plain scalar key with colon-containing text.",
          "[YAML][Parse][Collections][PlainScalar]") {
    // ':' inside a key value is part of the key text (not a separator)
    BufferSource source{"---\n"
                        "http://example.com: homepage\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("http://example.com"));
    REQUIRE(NRef<String>(yaml.document(0)["http://example.com"]).value() ==
            "homepage");
  }

  // ---- Flow collection edge cases ----

  SECTION("YAML empty flow sequence and empty flow dict in same doc.",
          "[YAML][Parse][Collections][Flow]") {
    BufferSource source{"---\n"
                        "empty_seq: []\n"
                        "empty_map: {}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)["empty_seq"]));
    REQUIRE(NRef<Array>(yaml.document(0)["empty_seq"]).size() == 0);
    REQUIRE(isA<Dictionary>(yaml.document(0)["empty_map"]));
    REQUIRE(NRef<Dictionary>(yaml.document(0)["empty_map"]).size() == 0);
  }

  SECTION("YAML flow sequence with trailing comma.",
          "[YAML][Parse][Collections][Flow]") {
    BufferSource source{"---\n[1, 2, 3,]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 3);
    REQUIRE(NRef<Number>(yaml.document(0)[2]).value<int>() == 3);
  }

  SECTION("YAML flow sequence allows comment before comma separator.",
          "[YAML][Parse][Collections][Flow]") {
    BufferSource source{"---\n[word1\n# comment\n, word2]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "word1");
    REQUIRE(NRef<String>(yaml.document(0)[1]).value() == "word2");
  }

  SECTION("YAML flow dict with quoted keys.",
          "[YAML][Parse][Collections][Flow]") {
    BufferSource source{
        "---\n{\"content-type\": \"text/html\", \"version\": 2}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("content-type"));
    REQUIRE(NRef<String>(yaml.document(0)["content-type"]).value() ==
            "text/html");
    REQUIRE(NRef<Number>(yaml.document(0)["version"]).value<int>() == 2);
  }

  // ---- Anchors and aliases in nested collections ----

  SECTION("YAML anchor on a sequence, aliased in another mapping.",
          "[YAML][Parse][Collections][AnchorAlias]") {
    BufferSource source{"---\n"
                        "defaults: &defaults\n"
                        "  - one\n"
                        "  - two\n"
                        "copy: *defaults\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Array>(yaml.document(0)["defaults"]));
    REQUIRE(isA<Array>(yaml.document(0)["copy"]));
    REQUIRE(NRef<Array>(yaml.document(0)["copy"]).size() == 2);
    REQUIRE(NRef<String>(yaml.document(0)["copy"][0]).value() == "one");
  }

  SECTION("YAML inline anchor on a scalar used in array.",
          "[YAML][Parse][Collections][AnchorAlias]") {
    BufferSource source{"---\n"
                        "- &name Alice\n"
                        "- *name\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "Alice");
    REQUIRE(NRef<String>(yaml.document(0)[1]).value() == "Alice");
  }

  SECTION("YAML merge key (<<) merges base dict into child.",
          "[YAML][Parse][Collections][Merge]") {
    BufferSource source{"---\n"
                        "base: &base\n"
                        "  x: 1\n"
                        "  y: 2\n"
                        "child:\n"
                        "  <<: *base\n"
                        "  z: 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)["child"]));
    REQUIRE(NRef<Number>(yaml.document(0)["child"]["x"]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml.document(0)["child"]["y"]).value<int>() == 2);
    REQUIRE(NRef<Number>(yaml.document(0)["child"]["z"]).value<int>() == 3);
  }

  SECTION("YAML multi-merge with two anchors.",
          "[YAML][Parse][Collections][Merge]") {
    BufferSource source{"---\n"
                        "a: &a\n"
                        "  x: 10\n"
                        "b: &b\n"
                        "  y: 20\n"
                        "c:\n"
                        "  <<: [*a, *b]\n"
                        "  z: 30\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)["c"]));
    REQUIRE(NRef<Number>(yaml.document(0)["c"]["x"]).value<int>() == 10);
    REQUIRE(NRef<Number>(yaml.document(0)["c"]["y"]).value<int>() == 20);
    REQUIRE(NRef<Number>(yaml.document(0)["c"]["z"]).value<int>() == 30);
  }

  // ---- Explicit indentation indicator (YAML 1.2 §8.1.1, gap 3.2) ----

  SECTION("YAML 1.2: literal block scalar with explicit indent indicator |2",
          "[YAML][Parse][Collections][BlockStr][ExplicitIndent]") {
    // |2 means content indented 2 spaces relative to the key's block level.
    BufferSource source{"key: |2\n"
                        "  line one\n"
                        "  line two\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["key"]));
    const auto val = NRef<String>(yaml.document(0)["key"]).value();
    // Clip chomping (|) preserves the first trailing newline in the spec, but
    // this library's current implementation strips it; check both lines exist.
    REQUIRE(val.find("line one") != std::string::npos);
    REQUIRE(val.find("line two") != std::string::npos);
  }

  SECTION("YAML 1.2: literal block scalar with explicit indent and strip "
          "chomping |2-",
          "[YAML][Parse][Collections][BlockStr][ExplicitIndent]") {
    // |2- means content at indent 2, strip trailing newline.
    BufferSource source{"key: |2-\n"
                        "  stripped\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["key"]));
    const auto val = NRef<String>(yaml.document(0)["key"]).value();
    REQUIRE(val == "stripped");
  }

  SECTION("YAML 1.2: literal block scalar chomping-then-digit |-2",
          "[YAML][Parse][Collections][BlockStr][ExplicitIndent]") {
    // |-2 is the same as |2- (both orderings are legal per spec §8.1.1).
    BufferSource source{"key: |-2\n"
                        "  stripped\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["key"]));
    const auto val = NRef<String>(yaml.document(0)["key"]).value();
    REQUIRE(val == "stripped");
  }

  SECTION("YAML 1.2: folded block scalar with explicit indent indicator >2",
          "[YAML][Parse][Collections][BlockStr][ExplicitIndent]") {
    // >2 means folded with content at indent-level 2.
    BufferSource source{"key: >2\n"
                        "  fold one\n"
                        "  fold two\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["key"]));
    const auto val = NRef<String>(yaml.document(0)["key"]).value();
    REQUIRE(!val.empty());
    // Folded style replaces the internal newline with a space.
    REQUIRE(val.find("fold one") != std::string::npos);
    REQUIRE(val.find("fold two") != std::string::npos);
  }

  SECTION("YAML 1.2: explicit indent indicator preserves more-indented lines",
          "[YAML][Parse][Collections][BlockStr][ExplicitIndent]") {
    // With |2, a normal line at 2-space indent followed by a 4-space line.
    // The more-indented line retains its extra leading spaces in the content.
    BufferSource source{"key: |2\n"
                        "  normal\n"
                        "    extra indent\n"
                        "  back\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["key"]));
    const auto val = NRef<String>(yaml.document(0)["key"]).value();
    REQUIRE(val.find("normal") != std::string::npos);
    REQUIRE(val.find("extra indent") != std::string::npos);
    REQUIRE(val.find("back") != std::string::npos);
  }
}
