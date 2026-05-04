#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parsing of directives.", "[YAML][Parse][Directives]") {
  const YAML yaml;

  // ---- %YAML directives ----

  SECTION("YAML parse %YAML 1.2 directive before document.",
          "[YAML][Parse][Directives][YAML12]") {
    BufferSource source{"%YAML 1.2\n---\nvalue: 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(isA<Dictionary>(yaml.document(0)));
  }

  SECTION("YAML parse %YAML 1.1 directive (older minor version).",
          "[YAML][Parse][Directives][YAML11]") {
    BufferSource source{"%YAML 1.1\n---\nvalue: 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }

  SECTION("YAML parse %YAML directive with unsupported major version throws.",
          "[YAML][Parse][Directives][YAMLBadMajor]") {
    BufferSource source{"%YAML 2.0\n---\nvalue: 42\n"};
    REQUIRE_THROWS_WITH(yaml.parse(source), Catch::Matchers::ContainsSubstring(
                                                "unsupported major version"));
  }

  SECTION("YAML parse %YAML directive with bad format throws.",
          "[YAML][Parse][Directives][YAMLBadFormat]") {
    BufferSource source{"%YAML 1\n---\nvalue: 42\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  SECTION("YAML parse %YAML directive with extra content after version throws.",
          "[YAML][Parse][Directives][YAMLExtraContent]") {
    BufferSource source{"%YAML 1.2 foo\n---\nvalue: 42\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION(
      "YAML parse %YAML directive with numeric extra content after version.",
      "[YAML][Parse][Directives][YAMLNumericExtraContent]") {
    BufferSource source{"%YAML 1.1 1.2\n---\nvalue: 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }

  SECTION("YAML bare %YAML directive with no document throws.",
          "[YAML][Parse][Directives][YAMLNoDocument]") {
    BufferSource source{"%YAML 1.2\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML %YAML directive followed only by document end throws.",
          "[YAML][Parse][Directives][YAMLNoDocument]") {
    BufferSource source{"%YAML 1.2\n...\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  SECTION("YAML parse %YAML directive after document start is invalid content.",
          "[YAML][Parse][Directives][YAMLAfterDoc]") {
    // Inside a document, % starts a plain scalar not a directive.
    // The parser should throw or handle gracefully (not crash).
    BufferSource source{"---\n%YAML 1.2\nvalue: 42\n"};
    // % inside document content is parsed as a plain scalar which fails
    // since "value: 42" causes a syntax error after it
    REQUIRE_THROWS(yaml.parse(source));
  }

  SECTION("YAML block mapping cannot start on same line as document start.",
          "[YAML][Parse][Directives]") {
    BufferSource source{"--- key1: value1\n    key2: value2\n"};
    REQUIRE_THROWS_AS(yaml.parse(source), SyntaxError);
  }

  // ---- %TAG directives ----

  SECTION("YAML parse %TAG directive maps handle to prefix.",
          "[YAML][Parse][Directives][Tag]") {
    // After %TAG, !!str should expand to "tag:example.com,2024:str"
    BufferSource source{"%TAG !! tag:example.com,2024:\n---\n!!str hello\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(isA<String>(yaml.document(0)));
    // Tag was expanded using the custom prefix
    REQUIRE(yaml.document(0).getTag() ==
            "tag:example.com,2024:str");
  }

  SECTION("YAML parse %TAG directive with ! handle for local tags.",
          "[YAML][Parse][Directives][TagLocal]") {
    BufferSource source{
        "%TAG ! tag:example.com,2024:\n---\n!item some value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(yaml.document(0).getTag() ==
            "tag:example.com,2024:item");
  }

  SECTION("YAML parse tagged mapping key with anchor prefix.",
          "[YAML][Parse][Directives][TagAnchorKey]") {
    BufferSource source{"!!str &a1 \"foo\":\n"
                        "  !!str bar\n"
                        "&a2 baz: *a1\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["foo"]).value() == "bar");
    REQUIRE(NRef<String>(yaml.document(0)["baz"]).value() == "foo");
  }

  SECTION("YAML parse multiple directives before a document.",
          "[YAML][Parse][Directives][Multiple]") {
    BufferSource source{
        "%YAML 1.2\n%TAG ! tag:example.com,2024:\n---\nname: test\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(isA<Dictionary>(yaml.document(0)));
  }

  SECTION("YAML parse unknown directive is silently ignored.",
          "[YAML][Parse][Directives][Unknown]") {
    BufferSource source{"%UNKNOWN some-value\n---\nname: test\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }

  // ---- %YAML + content round-trip ----

  SECTION("YAML parse document with %YAML directive and complex content.",
          "[YAML][Parse][Directives][Complex]") {
    BufferSource source{"%YAML 1.2\n---\nfoo:\n  - one\n  - two\nbar: true\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Array>(yaml.document(0)["foo"]));
    REQUIRE(NRef<Array>(yaml.document(0)["foo"]).size() == 2);
    REQUIRE(isA<Boolean>(yaml.document(0)["bar"]));
  }

  // ---- Multiple named %TAG handles in one document stream ----

  SECTION("YAML two distinct named handles expand independently.",
          "[YAML][Parse][Directives][MultiHandle]") {
    // %TAG !e! and %TAG !m! define two different handle-to-prefix mappings.
    // Both must be registered and both tags must expand correctly.
    BufferSource source{"%TAG !e! tag:example.com,2024:\n"
                        "%TAG !m! !my-\n"
                        "---\n"
                        "a: !e!type one\n"
                        "b: !m!color blue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["a"]));
    REQUIRE(yaml.document(0)["a"].getTag() ==
            "tag:example.com,2024:type");
    REQUIRE(isA<String>(yaml.document(0)["b"]));
    REQUIRE(yaml.document(0)["b"].getTag() == "!my-color");
  }

  SECTION("YAML %TAG handle resets between documents.",
          "[YAML][Parse][Directives][HandleReset]") {
    // A %TAG defined for one document does not carry over to the next;
    // the second document must re-declare its own %TAG to use the handle.
    BufferSource source{"%TAG !m! !my-\n"
                        "--- # doc 1\n"
                        "!m!light fluorescent\n"
                        "...\n"
                        "%TAG !m! !other-\n"
                        "--- # doc 2\n"
                        "!m!light warm\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 2);
    REQUIRE(yaml.document(0).getTag() == "!my-light");
    REQUIRE(yaml.document(1).getTag() == "!other-light");
  }

  SECTION("YAML %TAG !! handle remaps the secondary handle prefix.",
          "[YAML][Parse][Directives][SecondaryHandle]") {
    // %TAG !! prefix overrides the default secondary tag handle
    BufferSource source{"%TAG !! tag:custom.org,2024:\n"
                        "---\n"
                        "!!mytype some value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(yaml.document(0).getTag() ==
            "tag:custom.org,2024:mytype");
  }

  SECTION("YAML remapped !!int stays a tagged string instead of core integer "
          "coercion.",
          "[YAML][Parse][Directives][SecondaryHandle]") {
    BufferSource source{"%TAG !! tag:example.com,2000:app/\n"
                        "---\n"
                        "!!int 1 - 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "1 - 3");
    REQUIRE(yaml.document(0).getTag() ==
            "tag:example.com,2000:app/int");
  }
}
