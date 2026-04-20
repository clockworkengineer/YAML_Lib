#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML stringify.", "[YAML][Stringify]") {
  const YAML yaml;
  SECTION("YAML Stringify document a start.", "[YAML][Stringify][Start]") {
    BufferSource source{"---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n...\n");
  }
  SECTION("YAML Stringify document with just an end.",
          "[YAML][Stringify][End]") {
    BufferSource source{"...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n...\n");
  }
  SECTION("YAML Stringify document with comments.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"...\n# comment 1\n# comment 2\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n...\n");
  }
  SECTION("YAML Stringify document one with integer.",
          "[YAML][Stringify][Integer]") {
    BufferSource source{"---\n65000"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n65000\n...\n");
  }
  SECTION("YAML Stringify document one string.", "[YAML][Stringify][String]") {
    BufferSource source{"---\n\" test string \""};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n\" test string \"\n...\n");
  }
  SECTION("YAML Stringify document one boolean.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nTrue"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nTrue\n...\n");
  }
  SECTION("YAML Stringify document dictionary key value pair.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\ndoe: 'a deer, a female deer'\n...\n");
  }
  SECTION("YAML Stringify document dictionary two key value pairs.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml,
                "---\ndoe: 'a deer, a female deer'\nray: 'a drop of golden "
                "sun'\n...\n");
  }
  SECTION("YAML Parse array with one string elements and stringify.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - \"One\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n- \"One\"\n...\n");
  }
  SECTION("YAML Parse array with multiple string elements and stringify.",
          "[YAML][Parse][Array]") {
    BufferSource source{
        "---\n  - \"One\"\n  - \"Two\"\n  - \"Three\"\n  - \"Four\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n- \"One\"\n- \"Two\"\n"
                      "- \"Three\"\n- \"Four\"\n...\n");
  }
  SECTION("YAML Stringify document with comments before start.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"# comment 1\n---\n# comment 2\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    compareYAML(yaml, "---\n...\n");
  }
  SECTION("YAML Stringify sequence of double quoted strings.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{
        "- \"Mark McGwire\"\n- \"Sammy Sosa\"\n- \"Ken Griffey\""};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    compareYAML(
        yaml,
        "---\n- \"Mark McGwire\"\n- \"Sammy Sosa\"\n- \"Ken Griffey\"\n...\n");
  }
  SECTION("YAML Stringify sequence of single quoted strings.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"- 'Mark McGwire'\n- 'Sammy Sosa'\n- 'Ken Griffey'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    compareYAML(
        yaml, "---\n- 'Mark McGwire'\n- 'Sammy Sosa'\n- 'Ken Griffey'\n...\n");
  }
  SECTION("YAML Stringify sequence of unquoted strings.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"- Mark McGwire\n- Sammy Sosa\n- Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    compareYAML(yaml,
                "---\n- Mark McGwire\n- Sammy Sosa\n- Ken Griffey\n...\n");
  }
  SECTION("YAML Stringify nested array in dictionary.",
          "[YAML][Stringify][Comples]") {
    BufferSource source{"---\nhr:\n  - Mark McGwire\n  - Sammy Sosa\nrbi:\n  - "
                        "Sammy Sosa\n  - Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml,
                "---\nhr: \n  - Mark McGwire\n  - Sammy Sosa\nrbi: \n  - Sammy "
                "Sosa\n  - Ken Griffey\n...\n");
  }
  SECTION("YAML Stringify block/piped strings.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{
        "---\nname:  Mark McGwire\naccomplishment: >\n  Mark set a major "
        "league\n  home run record in 1998.\nstats: |\n  65 Home Runs\n  "
        "0.278 "
        "Batting Average"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(
        yaml, "---\nname: Mark McGwire\naccomplishment: >\n  Mark set a major "
              "league home run record in 1998.\nstats: |\n  65 Home Runs\n  "
              "0.278 Batting Average\n...\n");
  }
  //
  // It would be better to end with \n...\n instead of ....\n
  //
  SECTION("YAML Stringify plain literals newlines treated as space.",
          "[YAML][Stringify][literals]") {
    BufferSource source{
        "---\n  Mark McGwire\'s\n  year was crippled\n  by a knee injury."};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(
        yaml,
        "---\nMark McGwire\'s year was crippled by a knee injury.\n...\n");
  }
  SECTION("YAML Stringify literal string preserves newlines.",
          "[YAML][Stringify][literals]") {
    BufferSource source{"--- |\n  \\//||\\/||\n  // ||  ||__\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "\\//||\\/||\n// ||  ||__");
    compareYAML(yaml, "---|\n\\//||\\/||\n// ||  ||__\n...\n");
  }
  SECTION(
      "YAML Stringify folded newlines preserved for indented and blank lines.",
      "[YAML][Stringify][literals]") {
    BufferSource source{
        "--- >\n Sammy Sosa completed another\n fine season with great "
        "stats.\n\n   63 Home Runs\n   0.288 Batting Average\n\n What a year!"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(
        yaml,
        "--->\nSammy Sosa completed another fine season with great stats.\n\n "
        "  63 Home Runs\n   0.288 Batting Average\n\nWhat a year!\n...\n");
  }
  SECTION("YAML stringify various quoted scalars",
          "[YAML][Parse][Quoted Scalars]") {
    BufferSource source{
        "unicode: \"Sosa did fine.\\u263A\"\ncontrol: "
        "\"\\b1998\\t1999\\t2000\\n\"\nhexesc:  \"\\x13\\x10 is "
        "\\r\\n\"\n\nsingle: \'\"Howdy!\" he cried.\'\nquoted: \' # not a "
        "\'\'comment\'\'.\'\ntie-fighter: \'|\\-*-/|\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(
        yaml, "---\nunicode: \"Sosa did fine.\\u263A\"\ncontrol: "
              "\"\\b1998\\t1999\\t2000\\n\"\nhexesc: \"\\u0013\\u0010 is "
              "\\r\\n\"\nsingle: \'\"Howdy!\" he cried.\'\nquoted: \' # not a "
              "\'\'comment\'\'.\'\ntie-fighter: \'|\\-*-/|\'\n...\n");
  }
  SECTION("YAML parse dictionary with null for value entry and  stringify back",
          "[YAML][Parse][Null]") {
    BufferSource source{"null:\nbooleans: [ true, false ]\nstring: \'012345\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\n\"\": null\nbooleans: \n  - true\n "
                      " - false\nstring: \'012345\'\n...\n");
  }
  SECTION("YAML parse dictionaries in two documents and stringify back.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"---\ntime: 20:03:20\nplayer: Sammy Sosa\naction: "
                        "strike (miss)\n\n---\ntime: 20:03:47\nplayer: Sammy "
                        "Sosa\naction: grandslam\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml,
                "---\ntime: 20:03:20\nplayer: Sammy Sosa\naction: strike "
                "(miss)\n...\n---\ntime: 20:03:47\nplayer: Sammy Sosa\naction: "
                "grandslam\n...\n");
  }

  SECTION("YAML parse dictionaries indented YAML and stringify back",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"spec:\n  replicas: 3             \n  template:\n    "
                        "spec:\n      containers:\n        - name: api\n       "
                        "   image: my-app/api-server:latest"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml,
                "---\nspec: \n  replicas: 3\n  template: \n    spec: \n      "
                "containers: \n        - name: api\n          image: "
                "my-app/api-server:latest\n...\n");
  }

  SECTION("YAML parse array with nested dictionaries and stringify.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{"-\n  name: Mark Joseph\n  hr: 87\n  avg: 0.278\n-\n  "
                        "name: James Stephen\n  hr: 63\n  avg: 0.288"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(
        yaml, "---\n- name: Mark Joseph\n  hr: 87\n  avg: 0.278\n- name: James "
              "Stephen\n  hr: 63\n  avg: 0.288\n...\n");
  }

  SECTION("YAML parse dictionary with nested array and stringify.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{
        "---\nhr: # 1998 hr ranking\n  - Mark McGwire\n  - Sammy Sosa\nrbi:\n  "
        "# 1998 rbi ranking\n  - Sammy Sosa\n  - Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml,
                "---\nhr: \n  - Mark McGwire\n  - Sammy Sosa\nrbi: \n  - Sammy "
                "Sosa\n  - Ken Griffey\n...\n");
  }
  SECTION(
      "YAML parse dictionary with nested array plus comments and stringify.",
      "[YAML][Stringify][Comments]") {
    BufferSource source{"key: #comment 1\n   - value line 1\n   #comment 2\n   "
                        "- value line 2\n   #comment 3\n   - value line 3"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nkey: \n  - value line 1\n  - value "
                      "line 2\n  - value line 3\n...\n");
  }
  SECTION("YAML parse an array of character art (one) and  stringify.",
          "[YAML][Stringify][Piped]") {
    BufferSource source{"---\n- |\n \\//||\\/||\n // ||  ||__\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 1);
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() ==
            "\\//||\\/||\n// ||  ||__");
    compareYAML(yaml, "---\n- |\n  \\//||\\/||\n  // ||  ||__\n...\n");
  }
  SECTION("YAML parse a boolean (True) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nTrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nTrue\n...\n");
  }
  SECTION("YAML parse a boolean (False) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nFalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nFalse\n...\n");
  }
  SECTION("YAML parse a boolean (On) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nOn\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nOn\n...\n");
  }
  SECTION("YAML parse a boolean (Off) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nOff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nOff\n...\n");
  }
  SECTION("YAML parse a boolean (Yes) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nYes\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nYes\n...\n");
  }
  SECTION("YAML parse a boolean (No) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nNo\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    compareYAML(yaml, "---\nNo\n...\n");
  }
}

TEST_CASE("Check YAML stringify preserves explicit tags (gap 3.6).",
          "[YAML][Stringify][Tags]") {
  const YAML yaml;

  SECTION("Stringify round-trip: !!str forces string interpretation.",
          "[YAML][Stringify][Tags][Str]") {
    BufferSource source{"---\n!!str 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("!!str 42") != std::string::npos);
  }

  SECTION("Stringify round-trip: !!int tag preserved.",
          "[YAML][Stringify][Tags][Int]") {
    BufferSource source{"---\n!!int 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("!!int 42") != std::string::npos);
  }

  SECTION("Stringify round-trip: !!bool tag preserved.",
          "[YAML][Stringify][Tags][Bool]") {
    BufferSource source{"---\n!!bool true\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("!!bool true") != std::string::npos);
  }

  SECTION("Stringify round-trip: !!null tag preserved.",
          "[YAML][Stringify][Tags][Null]") {
    // ~  parses as Null; Null::toString() returns "null", so the stringify
    // output is "!!null null" (tag + canonical null form).
    BufferSource source{"---\n!!null ~\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("!!null null") != std::string::npos);
  }

  SECTION("Stringify round-trip: !!float tag preserved.",
          "[YAML][Stringify][Tags][Float]") {
    BufferSource source{"---\n!!float 3.14\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("!!float 3.14") != std::string::npos);
  }

  SECTION("Stringify round-trip: custom !tag preserved.",
          "[YAML][Stringify][Tags][Custom]") {
    BufferSource source{"---\n!mytag some value\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("!mytag some value") != std::string::npos);
  }

  SECTION("Stringify round-trip: !!str in dictionary value preserved.",
          "[YAML][Stringify][Tags][DictValue]") {
    BufferSource source{"---\nid: !!str 007\nname: Bond\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    const auto out = dest.toString();
    REQUIRE(out.find("id: !!str 007") != std::string::npos);
    REQUIRE(out.find("name: Bond") != std::string::npos);
  }

  SECTION("Stringify round-trip: !!str in array element preserved.",
          "[YAML][Stringify][Tags][ArrayElem]") {
    BufferSource source{"---\n- !!str 42\n- !!int 99\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    const auto out = dest.toString();
    REQUIRE(out.find("- !!str 42") != std::string::npos);
    REQUIRE(out.find("- !!int 99") != std::string::npos);
  }

  SECTION("Stringify round-trip: tagged value re-parses with correct type.",
          "[YAML][Stringify][Tags][RoundTrip]") {
    // !!str 42 should round-trip: re-parse produces a String node.
    BufferSource source{"---\n!!str 42\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    const YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(isA<String>(yaml2.document(0)));
    REQUIRE(NRef<String>(yaml2.document(0)).value() == "42");
    REQUIRE(yaml2.document(0).getVariant().getTag() == "tag:yaml.org,2002:str");
  }

  SECTION("Stringify round-trip: !!timestamp tag preserved.",
          "[YAML][Stringify][Tags][Timestamp]") {
    BufferSource source{"---\n!!timestamp 2001-07-08\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("!!timestamp 2001-07-08") !=
            std::string::npos);
  }
}

TEST_CASE("Check YAML stringify emits correct block scalar style (gap 3.7).",
          "[YAML][Stringify][BlockScalar]") {
  const YAML yaml;

  SECTION("Folded block string (>) stringifies as '>'.",
          "[YAML][Stringify][BlockScalar][Folded]") {
    BufferSource source{"---\ntext: >\n  hello world\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("text: >") != std::string::npos);
  }

  SECTION("Literal block string (|) stringifies as '|'.",
          "[YAML][Stringify][BlockScalar][Literal]") {
    BufferSource source{"---\ntext: |\n  hello\n  world\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("text: |") != std::string::npos);
  }

  SECTION("Folded block round-trip preserves string value.",
          "[YAML][Stringify][BlockScalar][RoundTrip]") {
    BufferSource source{"---\ntext: >\n  first line\n  second line\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    const auto originalValue = NRef<String>(yaml.document(0)["text"]).value();
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    const YAML yaml2;
    BufferSource reparsed{dest.toString()};
    REQUIRE_NOTHROW(yaml2.parse(reparsed));
    REQUIRE(NRef<String>(yaml2.document(0)["text"]).value() == originalValue);
  }
}