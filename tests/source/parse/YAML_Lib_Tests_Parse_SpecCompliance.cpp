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

  // ---- Numeric literal representations ----

  SECTION("YAML hexadecimal integer literal is parsed as Number.",
          "[YAML][Parse][Spec][Numeric]") {
    BufferSource source{"---\nvalue: 0xFF\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)["value"]));
    REQUIRE(NRef<Number>(yaml.document(0)["value"]).value<int>() == 255);
  }

  SECTION("YAML octal integer literal is parsed as Number.",
          "[YAML][Parse][Spec][Numeric]") {
    BufferSource source{"---\nvalue: 0o17\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)["value"]));
    REQUIRE(NRef<Number>(yaml.document(0)["value"]).value<int>() == 15);
  }

  SECTION("YAML positive infinity float literal is parsed as Number.",
          "[YAML][Parse][Spec][Numeric]") {
    BufferSource source{"---\nvalue: .inf\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)["value"]));
  }

  SECTION("YAML negative infinity float literal is parsed as Number.",
          "[YAML][Parse][Spec][Numeric]") {
    BufferSource source{"---\nvalue: -.inf\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)["value"]));
  }

  SECTION("YAML not-a-number float literal is parsed as Number.",
          "[YAML][Parse][Spec][Numeric]") {
    BufferSource source{"---\nvalue: .nan\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)["value"]));
  }

  // ---- Boolean representations ----

  SECTION("YAML 'true' and 'false' literals are parsed as Boolean.",
          "[YAML][Parse][Spec][Boolean]") {
    BufferSource source{"---\na: true\nb: false\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Boolean>(yaml.document(0)["a"]));
    REQUIRE(isA<Boolean>(yaml.document(0)["b"]));
    REQUIRE(NRef<Boolean>(yaml.document(0)["a"]).value() == true);
    REQUIRE(NRef<Boolean>(yaml.document(0)["b"]).value() == false);
  }

  SECTION("YAML 1.1 'yes' and 'no' literals are parsed as Boolean.",
          "[YAML][Parse][Spec][Boolean]") {
    // YAML 1.1 compat: yes/no treated as booleans when %YAML 1.1 declared.
    BufferSource source{"%YAML 1.1\n---\na: Yes\nb: No\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Boolean>(yaml.document(0)["a"]));
    REQUIRE(isA<Boolean>(yaml.document(0)["b"]));
    REQUIRE(NRef<Boolean>(yaml.document(0)["a"]).value() == true);
    REQUIRE(NRef<Boolean>(yaml.document(0)["b"]).value() == false);
  }

  // ---- Null representations ----

  SECTION("YAML '~' is parsed as Null.", "[YAML][Parse][Spec][Null]") {
    BufferSource source{"---\nvalue: ~\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Null>(yaml.document(0)["value"]));
  }

  SECTION("YAML empty value after key colon is parsed as Null.",
          "[YAML][Parse][Spec][Null]") {
    BufferSource source{"---\nvalue:\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Null>(yaml.document(0)["value"]));
  }

  // ---- Deeply nested block collections ----

  SECTION("YAML five levels deep nested block dictionaries.",
          "[YAML][Parse][Spec][Block][Deep]") {
    BufferSource source{"---\n"
                        "l1:\n"
                        "  l2:\n"
                        "    l3:\n"
                        "      l4:\n"
                        "        l5: leaf\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        NRef<String>(yaml.document(0)["l1"]["l2"]["l3"]["l4"]["l5"]).value() ==
        "leaf");
  }

  SECTION("YAML block sequence of block sequences (nested arrays).",
          "[YAML][Parse][Spec][Block][Deep]") {
    BufferSource source{"---\n"
                        "matrix:\n"
                        "  - - 1\n"
                        "    - 2\n"
                        "  - - 3\n"
                        "    - 4\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)["matrix"]));
    REQUIRE(NRef<Array>(yaml.document(0)["matrix"]).size() == 2);
    REQUIRE(isA<Array>(yaml.document(0)["matrix"][0]));
    REQUIRE(NRef<Number>(yaml.document(0)["matrix"][0][0]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml.document(0)["matrix"][1][1]).value<int>() == 4);
  }

  // ---- Mixed flow and block styles ----

  SECTION("YAML flow sequence as the value of a block mapping key.",
          "[YAML][Parse][Spec][Mixed]") {
    BufferSource source{"---\ncolors: [red, green, blue]\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)["colors"]));
    REQUIRE(NRef<Array>(yaml.document(0)["colors"]).size() == 3);
    REQUIRE(NRef<String>(yaml.document(0)["colors"][0]).value() == "red");
  }

  SECTION("YAML flow mapping as element of a block sequence.",
          "[YAML][Parse][Spec][Mixed]") {
    BufferSource source{
        "---\n- {name: Alice, age: 30}\n- {name: Bob, age: 25}\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(NRef<String>(yaml.document(0)[0]["name"]).value() == "Alice");
    REQUIRE(NRef<Number>(yaml.document(0)[1]["age"]).value<int>() == 25);
  }

  // ---- Type coercion with explicit tags ----

  SECTION("YAML !!str tag forces numeric string to remain a string.",
          "[YAML][Parse][Spec][Tags]") {
    BufferSource source{"---\nvalue: !!str 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["value"]));
    REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "42");
  }

  SECTION("YAML !!int tag coerces string to integer.",
          "[YAML][Parse][Spec][Tags]") {
    BufferSource source{"---\nvalue: !!int \"99\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)["value"]));
    REQUIRE(NRef<Number>(yaml.document(0)["value"]).value<int>() == 99);
  }

  SECTION("YAML !!bool tag coerces bare true to boolean true.",
          "[YAML][Parse][Spec][Tags]") {
    BufferSource source{"---\nflag: !!bool true\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Boolean>(yaml.document(0)["flag"]));
    REQUIRE(NRef<Boolean>(yaml.document(0)["flag"]).value() == true);
  }

  SECTION("YAML !!null tag on bare null value produces Null node.",
          "[YAML][Parse][Spec][Tags]") {
    BufferSource source{"---\nnothing: !!null ~\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Null>(yaml.document(0)["nothing"]));
  }

  // ---- String edge cases ----

  SECTION("YAML single-quoted string that contains double single quotes.",
          "[YAML][Parse][Spec][String]") {
    BufferSource source{"---\nvalue: 'it''s a test'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["value"]));
    REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "it's a test");
  }

  SECTION("YAML double-quoted string with escape sequence.",
          "[YAML][Parse][Spec][String]") {
    BufferSource source{"---\nvalue: \"line1\\nline2\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)["value"]));
    REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "line1\nline2");
  }

  // ---- Empty document ----

  SECTION("YAML empty document (just --- and ...) parses without error.",
          "[YAML][Parse][Spec][Empty]") {
    // An empty document has no content: the document gets no entries added.
    // We just verify it doesn't crash and yields one document.
    BufferSource source{"---\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
  }

  // ---- Explicit block mapping key (? indicator) ----

  SECTION("YAML explicit ? key with null value produces null entry.",
          "[YAML][Parse][Spec][ExplicitKey]") {
    // ? key with no ': value' gives key -> null (used in YAML sets)
    BufferSource source{"---\n? Mark McGwire\n? Sammy Sosa\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("Mark McGwire"));
    REQUIRE(isA<Null>(yaml.document(0)["Mark McGwire"]));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("Sammy Sosa"));
  }

  SECTION("YAML explicit ? key then : value on separate line.",
          "[YAML][Parse][Spec][ExplicitKey]") {
    // Spec Example 8.18 style: ? a\n: b
    BufferSource source{"---\n? a\n: b\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("a"));
    REQUIRE(NRef<String>(yaml.document(0)["a"]).value() == "b");
  }

  SECTION("YAML explicit ? key then : value inline (no space after colon).",
          "[YAML][Parse][Spec][ExplicitKey]") {
    BufferSource source{"---\n? foo\n: bar\n? baz\n: qux\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["foo"]).value() == "bar");
    REQUIRE(NRef<String>(yaml.document(0)["baz"]).value() == "qux");
  }

  SECTION("YAML !!set uses ? keys with implicit null values.",
          "[YAML][Parse][Spec][ExplicitKey]") {
    // Spec Example 2.25: Unordered Sets represented as mappings with null vals
    BufferSource source{
        "--- !!set\n? Mark McGwire\n? Sammy Sosa\n? Ken Griff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<Dictionary>(yaml.document(0)).size() == 3);
    REQUIRE(isA<Null>(yaml.document(0)["Mark McGwire"]));
    REQUIRE(isA<Null>(yaml.document(0)["Sammy Sosa"]));
    REQUIRE(isA<Null>(yaml.document(0)["Ken Griff"]));
  }
}
