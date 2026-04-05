#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML syntax error detection.",
          "[YAML][Parse][ErrorHandling]") {
  const YAML yaml;

  // ---- Tab indentation ----

  SECTION("YAML tab as top-level block indentation throws SyntaxError.",
          "[YAML][Parse][ErrorHandling][TabIndent]") {
    // Tab character used as indentation for a block mapping value
    BufferSource source{"---\nkey:\n\tvalue\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML tab as first indentation character throws SyntaxError.",
          "[YAML][Parse][ErrorHandling][TabIndent]") {
    // Top-level key indented with a tab
    BufferSource source{"---\n\tkey: value\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML tab mixed with spaces in block indentation throws SyntaxError.",
          "[YAML][Parse][ErrorHandling][TabIndent]") {
    // Space then tab in indentation zone
    BufferSource source{"---\nparent:\n  child:\n   \tdeep: value\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- Unterminated flow collections ----

  SECTION("YAML unterminated inline array throws.",
          "[YAML][Parse][ErrorHandling][Unterminated]") {
    BufferSource source{"---\n[1, 2, 3\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  SECTION("YAML unterminated inline dictionary throws.",
          "[YAML][Parse][ErrorHandling][Unterminated]") {
    BufferSource source{"---\n{a: 1, b: 2\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  // ---- Duplicate keys ----

  SECTION("YAML duplicate keys in block mapping throw SyntaxError.",
          "[YAML][Parse][ErrorHandling][DuplicateKey]") {
    BufferSource source{"---\nkey: one\nkey: two\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML duplicate keys in inline dictionary throw SyntaxError.",
          "[YAML][Parse][ErrorHandling][DuplicateKey]") {
    BufferSource source{"---\n{x: 1, x: 2}\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- Undefined aliases ----

  SECTION("YAML alias used before anchor is defined throws SyntaxError.",
          "[YAML][Parse][ErrorHandling][UndefinedAlias]") {
    BufferSource source{"---\nbaz: *notYetDefined\nfoo: &notYetDefined bar\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- Invalid directives ----

  SECTION("YAML unsupported major version in %YAML directive throws.",
          "[YAML][Parse][ErrorHandling][Directive]") {
    BufferSource source{"%YAML 3.0\n---\nvalue: 1\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- Multi-merge-key sequence with non-mapping element ----

  SECTION("YAML <<: [*seq, *map] where *seq is a sequence throws.",
          "[YAML][Parse][ErrorHandling][MultiMerge]") {
    BufferSource source{"---\n"
                        "seq: &seq\n"
                        "  - one\n"
                        "  - two\n"
                        "map: &map\n"
                        "  x: 1\n"
                        "result:\n"
                        "  <<: [*seq, *map]\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  // ---- Mapping indentation errors ----

  SECTION("YAML mapping key at wrong indentation level throws.",
          "[YAML][Parse][ErrorHandling][Indentation]") {
    // A key indented MORE than peers but not a child of those peers
    BufferSource source{"---\n"
                        "level1: value1\n"
                        "    badkey: value2\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  // ---- Inline collection never closed ----

  SECTION("YAML unclosed nested flow sequence throws.",
          "[YAML][Parse][ErrorHandling][Unterminated]") {
    BufferSource source{"---\n[[1, 2], [3, 4\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  SECTION("YAML unclosed flow dict inside sequence throws.",
          "[YAML][Parse][ErrorHandling][Unterminated]") {
    BufferSource source{"---\n[{a: 1}, {b: 2\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  // ---- Invalid key characters ----

  SECTION("YAML block mapping duplicate key throws SyntaxError.",
          "[YAML][Parse][ErrorHandling][DuplicateKey]") {
    BufferSource source{"---\n"
                        "fruit: apple\n"
                        "color: red\n"
                        "fruit: orange\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML inline dict duplicate key throws SyntaxError.",
          "[YAML][Parse][ErrorHandling][DuplicateKey]") {
    BufferSource source{"---\n{name: Alice, age: 30, name: Bob}\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- SyntaxError carries a message ----

  SECTION("YAML SyntaxError from unsupported YAML major version has message.",
          "[YAML][Parse][ErrorHandling][ErrorMessage]") {
    BufferSource source{"%YAML 2.0\n---\nvalue: 1\n"};
    try {
      yaml.parse(source);
      FAIL("Expected SyntaxError was not thrown.");
    } catch (const SyntaxError &ex) {
      // The message should mention "major version" or "unsupported"
      REQUIRE(std::string{ex.what()}.find("major version") !=
              std::string::npos);
    }
  }

  SECTION("YAML SyntaxError from undefined alias has message.",
          "[YAML][Parse][ErrorHandling][ErrorMessage]") {
    BufferSource source{"---\n*undefined_ref\n"};
    try {
      yaml.parse(source);
      FAIL("Expected SyntaxError was not thrown.");
    } catch (const SyntaxError &ex) {
      REQUIRE(!std::string{ex.what()}.empty());
    }
  }
}
