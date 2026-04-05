#include "YAML_Lib_Tests.hpp"

// ============================================================================
// Stringify round-trip tests: parse YAML, stringify it and re-parse, then
// verify the structure is preserved.  These also guard against stringify
// regressions.
// ============================================================================

TEST_CASE("Check YAML stringify round-trip correctness.",
          "[YAML][Stringify][RoundTrip]") {
  const YAML yaml;

  // ---- Basic scalars ----

  SECTION("YAML round-trip: integer scalar.",
          "[YAML][Stringify][RoundTrip][Scalar]") {
    BufferSource source{"---\n42\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Number>(yaml2.document(0)));
    REQUIRE(NRef<Number>(yaml2.document(0)).value<int>() == 42);
  }

  SECTION("YAML round-trip: float scalar.",
          "[YAML][Stringify][RoundTrip][Scalar]") {
    BufferSource source{"---\n3.14\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Number>(yaml2.document(0)));
  }

  SECTION("YAML round-trip: boolean scalar.",
          "[YAML][Stringify][RoundTrip][Scalar]") {
    BufferSource source{"---\ntrue\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Boolean>(yaml2.document(0)));
    REQUIRE(NRef<Boolean>(yaml2.document(0)).value() == true);
  }

  SECTION("YAML round-trip: null scalar.",
          "[YAML][Stringify][RoundTrip][Scalar]") {
    BufferSource source{"---\nnull\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Null>(yaml2.document(0)));
  }

  // ---- Timestamp ----

  SECTION("YAML round-trip: timestamp value is preserved.",
          "[YAML][Stringify][RoundTrip][Timestamp]") {
    BufferSource source{"---\n2024-01-15\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Timestamp>(yaml2.document(0)));
    REQUIRE(NRef<Timestamp>(yaml2.document(0)).value() == "2024-01-15");
  }

  SECTION("YAML round-trip: datetime timestamp.",
          "[YAML][Stringify][RoundTrip][Timestamp]") {
    BufferSource source{"---\nts: 2024-06-15T12:00:00Z\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Timestamp>(yaml2.document(0)["ts"]));
    REQUIRE(NRef<Timestamp>(yaml2.document(0)["ts"]).value() ==
            "2024-06-15T12:00:00Z");
  }

  // ---- Dictionary ----

  SECTION("YAML round-trip: flat block dictionary.",
          "[YAML][Stringify][RoundTrip][Dictionary]") {
    BufferSource source{"---\na: 1\nb: hello\nc: true\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Dictionary>(yaml2.document(0)));
    REQUIRE(NRef<Number>(yaml2.document(0)["a"]).value<int>() == 1);
    REQUIRE(NRef<String>(yaml2.document(0)["b"]).value() == "hello");
    REQUIRE(NRef<Boolean>(yaml2.document(0)["c"]).value() == true);
  }

  SECTION("YAML round-trip: nested block dictionary.",
          "[YAML][Stringify][RoundTrip][Dictionary]") {
    BufferSource source{"---\nouter:\n  inner:\n    value: 99\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(NRef<Number>(yaml2.document(0)["outer"]["inner"]["value"])
                .value<int>() == 99);
  }

  // ---- Sequence ----

  SECTION("YAML round-trip: block sequence of strings.",
          "[YAML][Stringify][RoundTrip][Sequence]") {
    BufferSource source{"---\n- alpha\n- beta\n- gamma\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<Array>(yaml2.document(0)));
    REQUIRE(NRef<Array>(yaml2.document(0)).size() == 3);
    REQUIRE(NRef<String>(yaml2.document(0)[2]).value() == "gamma");
  }

  SECTION("YAML round-trip: sequence of dictionaries.",
          "[YAML][Stringify][RoundTrip][Sequence]") {
    BufferSource source{"---\n"
                        "- name: Alice\n  score: 95\n"
                        "- name: Bob\n  score: 87\n"
                        "...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(NRef<String>(yaml2.document(0)[0]["name"]).value() == "Alice");
    REQUIRE(NRef<Number>(yaml2.document(0)[1]["score"]).value<int>() == 87);
  }

  // ---- Multi-document stream ----

  SECTION("YAML round-trip: two-document stream.",
          "[YAML][Stringify][RoundTrip][MultiDoc]") {
    BufferSource source{"---\nfirst: 1\n...\n---\nsecond: 2\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 2);
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(yaml2.getNumberOfDocuments() == 2);
    REQUIRE(NRef<Number>(yaml2.document(0)["first"]).value<int>() == 1);
    REQUIRE(NRef<Number>(yaml2.document(1)["second"]).value<int>() == 2);
  }

  // ---- Mixed content ----

  SECTION("YAML round-trip: dict with Null, String, Number, Boolean, Array.",
          "[YAML][Stringify][RoundTrip][Mixed]") {
    BufferSource source{"---\n"
                        "name: Test\n"
                        "count: 7\n"
                        "active: false\n"
                        "tags: [a, b, c]\n"
                        "nothing: null\n"
                        "...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(NRef<String>(yaml2.document(0)["name"]).value() == "Test");
    REQUIRE(NRef<Number>(yaml2.document(0)["count"]).value<int>() == 7);
    REQUIRE(NRef<Boolean>(yaml2.document(0)["active"]).value() == false);
    REQUIRE(NRef<Array>(yaml2.document(0)["tags"]).size() == 3);
    REQUIRE(isA<Null>(yaml2.document(0)["nothing"]));
  }

  // ---- Quoted strings are preserved with quotes ----

  SECTION("YAML round-trip: double-quoted string retains quotes in output.",
          "[YAML][Stringify][RoundTrip][String]") {
    BufferSource source{"---\n\"hello world\"\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n\"hello world\"\n...\n");
  }

  SECTION("YAML round-trip: single-quoted string retains quotes in output.",
          "[YAML][Stringify][RoundTrip][String]") {
    BufferSource source{"---\n'single quoted'\n...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n'single quoted'\n...\n");
  }

  // ---- Numeric literal representations survive round-trip ----

  SECTION("YAML round-trip: hex integer parsed and re-stringified as decimal.",
          "[YAML][Stringify][RoundTrip][Numeric]") {
    BufferSource source{"---\nvalue: 0xFF\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    // Stringify converts to decimal representation (255)
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(NRef<Number>(yaml2.document(0)["value"]).value<int>() == 255);
  }
}
