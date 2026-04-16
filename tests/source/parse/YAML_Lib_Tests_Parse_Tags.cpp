#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parsing of tags.", "[YAML][Parse][Tags]") {
  const YAML yaml;

  // ---- Standard !! tags ----

  SECTION("YAML parse !!str forces string type.", "[YAML][Parse][Tags][Str]") {
    BufferSource source{"---\n!!str 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "42");
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:str");
  }

  SECTION("YAML parse !!str on a boolean-looking value forces string.",
          "[YAML][Parse][Tags][StrBool]") {
    BufferSource source{"---\n!!str true\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "true");
  }

  SECTION("YAML parse !!int forces integer type.", "[YAML][Parse][Tags][Int]") {
    BufferSource source{"---\n!!int 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)));
    REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 42);
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:int");
  }

  SECTION("YAML parse !!float forces float type.",
          "[YAML][Parse][Tags][Float]") {
    BufferSource source{"---\n!!float 3.14\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Number>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() ==
            "tag:yaml.org,2002:float");
  }

  SECTION("YAML parse !!bool forces boolean type.",
          "[YAML][Parse][Tags][Bool]") {
    BufferSource source{"---\n!!bool true\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Boolean>(yaml.document(0)));
    REQUIRE(NRef<Boolean>(yaml.document(0)).value() == true);
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:bool");
  }

  SECTION("YAML parse !!null forces null type.", "[YAML][Parse][Tags][Null]") {
    BufferSource source{"---\n!!null ~\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Null>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:null");
  }

  SECTION("YAML parse !!seq on a YAML sequence.", "[YAML][Parse][Tags][Seq]") {
    BufferSource source{"---\n!!seq\n- one\n- two\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 2);
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:seq");
  }

  SECTION("YAML parse !!map on a YAML mapping.", "[YAML][Parse][Tags][Map]") {
    BufferSource source{"---\n!!map\nkey: value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:map");
  }

  // ---- Custom/local ! tags ----

  SECTION("YAML parse custom !tag on string value.",
          "[YAML][Parse][Tags][Custom]") {
    BufferSource source{"---\n!mytag some value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() == "!mytag");
  }

  SECTION("YAML parse !!str in a dictionary value.",
          "[YAML][Parse][Tags][DictValue]") {
    BufferSource source{"---\nid: !!str 007\nname: Bond\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["id"]));
    REQUIRE(NRef<String>(yaml.document(0)["id"]).value() == "007");
  }

  SECTION("YAML parse !!int with invalid value throws.",
          "[YAML][Parse][Tags][IntError]") {
    BufferSource source{"---\n!!int notanumber\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  SECTION("YAML parse !!bool with invalid value throws.",
          "[YAML][Parse][Tags][BoolError]") {
    BufferSource source{"---\n!!bool notabool\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  SECTION("YAML parse verbatim tag !<uri>.", "[YAML][Parse][Tags][Verbatim]") {
    BufferSource source{"---\n!<tag:example.com,2024:item> some value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() ==
            "!<tag:example.com,2024:item>");
  }

  SECTION("YAML parse verbatim-tagged mapping key with tagged scalar value.",
          "[YAML][Parse][Tags][Verbatim]") {
    BufferSource source{"!<tag:yaml.org,2002:str> foo :\n"
                        "  !<!bar> baz\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["foo"]));
    REQUIRE(NRef<String>(yaml.document(0)["foo"]).value() == "baz");
    REQUIRE(yaml.document(0)["foo"].getVariant().getTag() == "!<!bar>");
  }

  // ---- Named tag handle !ns!suffix ----

  SECTION("YAML named tag handle !m! expands to registered prefix.",
          "[YAML][Parse][Tags][NamedHandle]") {
    // %TAG !m! !my- maps !m!light -> !my-light
    BufferSource source{"%TAG !m! !my-\n---\n!m!light fluorescent\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() == "!my-light");
    REQUIRE(NRef<String>(yaml.document(0)).value() == "fluorescent");
  }

  SECTION("YAML named tag handle expands across two documents.",
          "[YAML][Parse][Tags][NamedHandle]") {
    BufferSource source{
        "%TAG !m! !my-\n--- # first\n!m!light fluorescent\n...\n"
        "%TAG !m! !my-\n--- # second\n!m!light green\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 2);
    REQUIRE(yaml.document(0).getVariant().getTag() == "!my-light");
    REQUIRE(NRef<String>(yaml.document(0)).value() == "fluorescent");
    REQUIRE(yaml.document(1).getVariant().getTag() == "!my-light");
    REQUIRE(NRef<String>(yaml.document(1)).value() == "green");
  }

  SECTION("YAML named tag handle with URI-style prefix.",
          "[YAML][Parse][Tags][NamedHandle]") {
    BufferSource source{"%TAG !e! tag:example.com,2024:\n---\n!e!widget foo\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() ==
            "tag:example.com,2024:widget");
  }

  SECTION("YAML unknown named handle falls back to verbatim.",
          "[YAML][Parse][Tags][NamedHandle]") {
    // !x!bar with no %TAG !x! registered - kept verbatim
    BufferSource source{"---\n!x!bar baz\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(yaml.document(0).getVariant().getTag() == "!x!bar");
  }

  SECTION("YAML named tag handle on dictionary value.",
          "[YAML][Parse][Tags][NamedHandle]") {
    BufferSource source{
        "%TAG !e! tag:example.com,\n---\ncolor: !e!rgb #ff0000\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)["color"]));
    REQUIRE(yaml.document(0)["color"].getVariant().getTag() ==
            "tag:example.com,rgb");
  }

  // ---- !!omap and !!pairs ----

  SECTION("YAML !!omap parses as a Dictionary with tag preserved.",
          "[YAML][Parse][Tags][OMap]") {
    BufferSource source{"---\n!!omap\n- a: 1\n- b: 2\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:omap");
  }

  SECTION("YAML !!omap preserves key insertion order.",
          "[YAML][Parse][Tags][OMap]") {
    BufferSource source{"---\n!!omap\n- z: last\n- a: first\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.document(0).getVariant().getTag() == "tag:yaml.org,2002:omap");
  }

  SECTION("YAML !!pairs parses as an Array with tag preserved.",
          "[YAML][Parse][Tags][Pairs]") {
    BufferSource source{"---\n!!pairs\n- key: value\n- key: another\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.document(0).getVariant().getTag() ==
            "tag:yaml.org,2002:pairs");
  }
}
