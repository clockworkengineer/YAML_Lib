#include "YAML_Lib_Tests.hpp"

// ============================================================================
// YAML test-suite integration
// ============================================================================
// A curated selection of cases from the YAML test-suite
// (tests/yaml-test-suite/yaml-test-suite/src/).  Each SECTION contains the
// raw YAML drawn directly from the suite file plus the expected outcome
// (REQUIRE_NOTHROW for valid YAML, REQUIRE_THROWS for fail:true cases).
// ============================================================================

TEST_CASE("YAML test-suite — valid documents parse without error.",
          "[YAML][TestSuite][Valid]") {
  const YAML yaml;

  // 229Q — Spec Example 2.4. Sequence of Mappings
  SECTION("229Q: sequence of mappings.", "[YAML][TestSuite][Valid]") {
    BufferSource source{"-\n"
                        "  name: Mark McGwire\n"
                        "  hr:   65\n"
                        "  avg:  0.278\n"
                        "-\n"
                        "  name: Sammy Sosa\n"
                        "  hr:   63\n"
                        "  avg:  0.288\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(NRef<String>(yaml.document(0)[0]["name"]).value() ==
            "Mark McGwire");
    REQUIRE(NRef<Number>(yaml.document(0)[0]["hr"]).value<int>() == 65);
  }

  // 2AUY — Tags in Block Sequence
  SECTION("2AUY: !!str and !!int tags in block sequence.",
          "[YAML][TestSuite][Valid]") {
    BufferSource source{"- !!str a\n"
                        "- b\n"
                        "- !!int 42\n"
                        "- d\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)[0]));
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "a");
    REQUIRE(isA<String>(yaml.document(0)[1]));
    REQUIRE(isA<Number>(yaml.document(0)[2]));
    REQUIRE(NRef<Number>(yaml.document(0)[2]).value<int>() == 42);
  }

  // 4GC6 — Spec Example 7.7. Single Quoted Characters
  SECTION("4GC6: single-quoted string with embedded single quote.",
          "[YAML][TestSuite][Valid]") {
    BufferSource source{"'here''s to \"quotes\"'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "here's to \"quotes\"");
  }

  // 5C5M — Spec Example 7.15. Flow Mappings (trailing comma)
  SECTION("5C5M: flow mappings with trailing comma.",
          "[YAML][TestSuite][Valid]") {
    BufferSource source{"- { one : two , three: four , }\n"
                        "- {five: six,seven : eight}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE(NRef<String>(yaml.document(0)[0]["one"]).value() == "two");
    REQUIRE(NRef<String>(yaml.document(0)[0]["three"]).value() == "four");
  }

  // 5TYM — Spec Example 6.21. Local Tag Prefix (!m! handle)
  SECTION("5TYM: local tag prefix !m! expands to !my-.",
          "[YAML][TestSuite][Valid]") {
    // Single-document form: %TAG !m! registers !m! as a named handle.
    BufferSource source{"%TAG !m! !my-\n"
                        "---\n"
                        "!m!light fluorescent\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(yaml.document(0).getVariant().getTag() == "!my-light");
    REQUIRE(NRef<String>(yaml.document(0)).value() == "fluorescent");
  }

  // 6LVF — Spec Example 6.13. Reserved Directives (ignored)
  SECTION("6LVF: unknown %FOO directive is silently ignored.",
          "[YAML][TestSuite][Valid]") {
    // The parser ignores unknown directives.  The following --- on its own
    // line then starts a document with a quoted string value.
    BufferSource source{"%FOO  bar baz # Should be ignored\n"
                        "              # with a warning.\n"
                        "---\n"
                        "\"foo\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "foo");
  }

  // 26DV — Alias and mapping (whitespace)
  SECTION("26DV: alias in mapping.", "[YAML][TestSuite][Valid]") {
    BufferSource source{"---\n"
                        "- &a\n"
                        "  key: value\n"
                        "- *a\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE(isA<Dictionary>(yaml.document(0)[1]));
  }

  // 2EBW — Keys with special characters
  SECTION("2EBW: keys with question-mark and colon as plain scalar prefixes.",
          "[YAML][TestSuite][Valid]") {
    // "?foo" (no space after ?) is a plain scalar key, NOT an explicit key.
    // ":foo" and "-foo" are similarly treated as plain scalar keys.
    BufferSource source{"?foo: safe question mark\n"
                        ":foo: safe colon\n"
                        "-foo: safe dash\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("?foo"));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains(":foo"));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("-foo"));
    REQUIRE(NRef<String>(yaml.document(0)["?foo"]).value() ==
            "safe question mark");
  }

  // 2XXW — Spec Example 2.25. Unordered Sets (explicit ? keys)
  SECTION("2XXW: explicit ? keys with null value (YAML set).",
          "[YAML][TestSuite][Valid]") {
    BufferSource source{"--- !!set\n"
                        "? Mark McGwire\n"
                        "? Sammy Sosa\n"
                        "? Ken Griff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).size() == 3);
    REQUIRE(isA<Null>(yaml.document(0)["Mark McGwire"]));
  }

  // 35KP — Tags on root objects + explicit ? key: value
  SECTION("35KP: explicit ? a : b with !!map tag.",
          "[YAML][TestSuite][Valid]") {
    BufferSource source{"--- !!map\n"
                        "? a\n"
                        ": b\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("a"));
    REQUIRE(NRef<String>(yaml.document(0)["a"]).value() == "b");
  }

  // S4JQ — Spec Example 6.28. Non-Specific Tags
  SECTION("S4JQ: non-specific ! tag on a scalar.", "[YAML][TestSuite][Valid]") {
    BufferSource source{"- \"12\"\n"
                        "- 12\n"
                        "- ! 12\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 3);
    // First is a quoted string "12"
    REQUIRE(isA<String>(yaml.document(0)[0]));
    // Second is the integer 12
    REQUIRE(isA<Number>(yaml.document(0)[1]));
  }

  // P94K — Spec Example 6.11. Multi-Line Comments
  SECTION("P94K: multi-line comment between key and value.",
          "[YAML][TestSuite][Valid]") {
    BufferSource source{"key:    # Comment\n"
                        "        # lines\n"
                        "  value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["key"]).value() == "value");
  }

  // 3RLN — Leading tab in double-quoted string (not block indentation)
  SECTION("3RLN: tab inside double-quoted string is not a block indent error.",
          "[YAML][TestSuite][Valid]") {
    BufferSource source{"\"1 leading\\n\t\\ttab\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
  }
}

TEST_CASE("YAML test-suite — invalid documents throw on parse.",
          "[YAML][TestSuite][Invalid]") {
  const YAML yaml;

  // 236B — Invalid value after mapping block
  SECTION("236B: invalid content after block mapping value throws.",
          "[YAML][TestSuite][Invalid]") {
    BufferSource source{"foo:\n"
                        "  bar\n"
                        "invalid\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  // 6JTT — Unclosed flow sequence
  SECTION("6JTT: unclosed flow sequence throws.",
          "[YAML][TestSuite][Invalid]") {
    BufferSource source{"---\n[ [ a, b, c ]\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  // 2CMS — error mapping
  SECTION("2CMS: mapping indentation error throws.",
          "[YAML][TestSuite][Invalid]") {
    BufferSource source{"- foo: bar\n"
                        " baz: bat\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }
}
