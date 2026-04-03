#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML spec compliance edge cases.", "[YAML][Parse][Spec]") {
  const YAML yaml;

  // ---- Empty flow collections ----

  SECTION("YAML parse empty inline array produces empty Array node.",
          "[YAML][Parse][Spec][Flow]") {
    BufferSource source{"---\n[]\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 0);
  }

  SECTION("YAML parse empty inline dictionary produces empty Dictionary node.",
          "[YAML][Parse][Spec][Flow]") {
    BufferSource source{"---\n{ }\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).size() == 0);
  }

  // ---- Deeply nested flow collections ----

  SECTION("YAML parse deeply nested inline dictionary.",
          "[YAML][Parse][Spec][Flow]") {
    BufferSource source{"---\n{a: {b: {c: 42}}}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Dictionary>(yaml.document(0)["a"]));
    REQUIRE(isA<Dictionary>(yaml.document(0)["a"]["b"]));
    REQUIRE(isA<Number>(yaml.document(0)["a"]["b"]["c"]));
    REQUIRE(NRef<Number>(yaml.document(0)["a"]["b"]["c"]).value<int>() == 42);
  }

  SECTION("YAML parse inline array with inline dictionary elements.",
          "[YAML][Parse][Spec][Flow]") {
    BufferSource source{"---\n[{a: 1}, {b: 2}]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE(isA<Dictionary>(yaml.document(0)[1]));
    REQUIRE(NRef<Number>(yaml.document(0)[0]["a"]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml.document(0)[1]["b"]).value<int>() == 2);
  }

  SECTION("YAML parse mixed nested flow: dict with array values.",
          "[YAML][Parse][Spec][Flow]") {
    BufferSource source{"---\n{first: [1, 2], second: [3, 4]}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Array>(yaml.document(0)["first"]));
    REQUIRE(isA<Array>(yaml.document(0)["second"]));
    REQUIRE(NRef<Array>(yaml.document(0)["first"]).size() == 2);
    REQUIRE(NRef<Array>(yaml.document(0)["second"]).size() == 2);
  }

  // ---- Null key behavior ----

  SECTION("YAML parse null as dictionary key gives empty-string key.",
          "[YAML][Parse][Spec][NullKey]") {
    BufferSource source{"---\nnull: 42\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains(""));
    REQUIRE(NRef<Number>(yaml.document(0)[""]).value<int>() == 42);
  }

  SECTION("YAML parse ~ as dictionary key gives empty-string key.",
          "[YAML][Parse][Spec][NullKey]") {
    BufferSource source{"---\n~: hello\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains(""));
    REQUIRE(NRef<String>(yaml.document(0)[""]).value() == "hello");
  }

  // ---- Comments do not affect parsed values ----

  SECTION("YAML comment at end of value line does not alter value.",
          "[YAML][Parse][Spec][Comment]") {
    BufferSource source{"---\nkey: value # this is a comment\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["key"]).value() == "value");
  }

  SECTION("YAML comment on separate line between key-value pairs.",
          "[YAML][Parse][Spec][Comment]") {
    BufferSource source{"---\nfoo: 1\n# comment line\nbar: 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Number>(yaml.document(0)["foo"]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml.document(0)["bar"]).value<int>() == 2);
  }

  // ---- Multi-document stream edge cases ----

  SECTION("YAML stream with three documents.",
          "[YAML][Parse][Spec][MultiDoc]") {
    BufferSource source{
        "---\nfoo: 1\n...\n---\nfoo: 2\n...\n---\nfoo: 3\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 3);
    REQUIRE(NRef<Number>(yaml.document(0)["foo"]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml.document(1)["foo"]).value<int>() == 2);
    REQUIRE(NRef<Number>(yaml.document(2)["foo"]).value<int>() == 3);
  }

  SECTION("YAML stream with consecutive --- starts a new document.",
          "[YAML][Parse][Spec][MultiDoc]") {
    BufferSource source{"---\nvalue: first\n---\nvalue: second\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 2);
    REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "first");
    REQUIRE(NRef<String>(yaml.document(1)["value"]).value() == "second");
  }

  // ---- Block sequence as mapping value ----

  SECTION("YAML block sequence as a mapping value.",
          "[YAML][Parse][Spec][Block]") {
    BufferSource source{"---\nitems:\n  - one\n  - two\n  - three\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Array>(yaml.document(0)["items"]));
    REQUIRE(NRef<Array>(yaml.document(0)["items"]).size() == 3);
    REQUIRE(NRef<String>(yaml.document(0)["items"][0]).value() == "one");
    REQUIRE(NRef<String>(yaml.document(0)["items"][2]).value() == "three");
  }

  SECTION("YAML nested block mapping.", "[YAML][Parse][Spec][Block]") {
    BufferSource source{"---\nouter:\n  inner:\n    deep: value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Dictionary>(yaml.document(0)["outer"]));
    REQUIRE(isA<Dictionary>(yaml.document(0)["outer"]["inner"]));
    REQUIRE(NRef<String>(yaml.document(0)["outer"]["inner"]["deep"]).value() ==
            "value");
  }
}
