#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML stringify.", "[YAML][Stringify]") {
  const YAML yaml;
  SECTION("YAML Stringify document a start.", "[YAML][Stringify][Start]") {
    BufferSource source{"---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n...\n");
  }
  SECTION("YAML Stringify document with just an end.",
          "[YAML][Stringify][End]") {
    BufferSource source{"...\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n...\n");
  }
  SECTION("YAML Stringify document with comments.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"...\n# comment 1\n# comment 2\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n...\n# comment 1\n# comment 2\n# comment 3\n");
  }
  SECTION("YAML Stringify document one with integer.",
          "[YAML][Stringify][Integer]") {
    BufferSource source{"---\n65000"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n65000\n...\n");
  }
  SECTION("YAML Stringify document one string.", "[YAML][Stringify][String]") {
    BufferSource source{"---\n\" test string \""};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\" test string \"\n...\n");
  }
  SECTION("YAML Stringify document one boolean.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nTrue"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nTrue\n...\n");
  }
  SECTION("YAML Stringify document dictionary key value pair.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\ndoe: 'a deer, a female deer'\n...\n");
  }
  SECTION("YAML Stringify document dictionary two key value pairs.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\ndoe: 'a deer, a female deer'\nray: 'a drop of golden "
            "sun'\n...\n");
  }
  SECTION("YAML Parse array with one string elements and restringify.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - \"One\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n- \"One\"\n...\n");
  }
  SECTION("YAML Parse array with multiple string elements and restringify.",
          "[YAML][Parse][Array]") {
    BufferSource source{
        "---\n  - \"One\"\n  - \"Two\"\n  - \"Three\"\n  - \"Four\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n- \"One\"\n- \"Two\"\n"
                                      "- \"Three\"\n- \"Four\"\n...\n");
  }
  SECTION("YAML Stringify document with comments before start.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"# comment 1\n---\n# comment 2\n# comment 3\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(destination.toString() ==
            "# comment 1\n---\n# comment 2\n# comment 3\n...\n");
  }
  SECTION("YAML Stringify sequence of double quoted strings.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{
        "- \"Mark McGwire\"\n- \"Sammy Sosa\"\n- \"Ken Griffey\""};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(
        destination.toString() ==
        "---\n- \"Mark McGwire\"\n- \"Sammy Sosa\"\n- \"Ken Griffey\"\n...\n");
  }
  SECTION("YAML Stringify sequence of single quoted strings.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"- 'Mark McGwire'\n- 'Sammy Sosa'\n- 'Ken Griffey'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(destination.toString() ==
            "---\n- 'Mark McGwire'\n- 'Sammy Sosa'\n- 'Ken Griffey'\n...\n");
  }
  SECTION("YAML Stringify sequence of unquoted strings.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"- Mark McGwire\n- Sammy Sosa\n- Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE(destination.toString() ==
            "---\n- Mark McGwire\n- Sammy Sosa\n- Ken Griffey\n...\n");
  }
  SECTION("YAML Stringify nested array in dictionary.",
          "[YAML][Stringify][Comples]") {
    BufferSource source{"---\nhr:\n  - Mark McGwire\n  - Sammy Sosa\nrbi:\n  - "
                        "Sammy Sosa\n  - Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
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
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\nname: Mark McGwire\naccomplishment: |\n  Mark set a major "
            "league home run record in 1998.\nstats: |\n  65 Home Runs\n  "
            "0.278 Batting Average\n...\n");
  }
  //
  // It would be better to end with \n...\n instead of ....\n
  //
  SECTION("YAML Stringify plain literals newlines treatd as space.",
          "[YAML][Stringify][literals]") {
    BufferSource source{
        "---\n  Mark McGwire\'s\n  year was crippled\n  by a knee injury."};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\nMark McGwire\'s year was crippled by a knee injury.\n...\n");
  }
  SECTION("YAML Stringify literal string perserves newlines.",
          "[YAML][Stringify][literals]") {
    BufferSource source{"--- |\n  \\//||\\/||\n  // ||  ||__\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "\\//||\\/||\n// ||  ||__");
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---|\n\\//||\\/||\n// ||  ||__\n...\n");
  }
  SECTION(
      "YAML Stringify folded newlines preserved for indented and blank lines.",
      "[YAML][Stringify][literals]") {
    BufferSource source{
        "--- >\n Sammy Sosa completed another\n fine season with great "
        "stats.\n\n   63 Home Runs\n   0.288 Batting Average\n\n What a year!"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(
        destination.toString() ==
        "---|\nSammy Sosa completed another fine season with great stats.\n\n "
        "  63 Home Runs\n   0.288 Batting Average\n\nWhat a year!\n...\n");
  }
  SECTION("YAML stringiy various qoted scalars",
          "[YAML][Parse][Quoted Scalars]") {
    BufferSource source{
        "unicode: \"Sosa did fine.\\u263A\"\ncontrol: "
        "\"\\b1998\\t1999\\t2000\\n\"\nhexesc:  \"\\x13\\x10 is "
        "\\r\\n\"\n\nsingle: \'\"Howdy!\" he cried.\'\nquoted: \' # not a "
        "\'\'comment\'\'.\'\ntie-fighter: \'|\\-*-/|\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\nunicode: \"Sosa did fine.\\u263A\"\ncontrol: "
            "\"\\b1998\\t1999\\t2000\\n\"\nhexesc: \"\\u0013\\u0010 is "
            "\\r\\n\"\nsingle: \'\"Howdy!\" he cried.\'\nquoted: \' # not a "
            "\'comment\'.\'\ntie-fighter: \'|\\-*-/|\'\n...\n");
  }
  SECTION("YAML parse dictionary with null for value entry and  stringify back",
          "[YAML][Parse][Null]") {
    BufferSource source{"null:\nbooleans: [ true, false ]\nstring: \'012345\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nnull: null\nbooleans: \n  - true\n "
                                      " - false\nstring: \'012345\'\n...\n");
  }
  SECTION("YAML parse dictionaries in two documents and stringify back.",
          "[YAML][Stringify][Comments]") {
    BufferSource source{"---\ntime: 20:03:20\nplayer: Sammy Sosa\naction: "
                        "strike (miss)\n\n---\ntime: 20:03:47\nplayer: Sammy "
                        "Sosa\naction: grandslam\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
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
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\nspec: \n  replicas: 3\n  template: \n    spec: \n      "
            "containers: \n        - name: api\n          image: "
            "my-app/api-server:latest\n...\n");
  }

  SECTION("YAML parse array with nested dictionaries and stringify.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{"-\n  name: Mark Joseph\n  hr: 87\n  avg: 0.278\n-\n  "
                        "name: James Stephen\n  hr: 63\n  avg: 0.288"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n- name: Mark Joseph\n  hr: 87\n  avg: 0.278\n- name: James "
            "Stephen\n  hr: 63\n  avg: 0.288\n...\n");
  }

  SECTION("YAML parse dictionary with nested array and stringify.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{
        "---\nhr: # 1998 hr ranking\n  - Mark McGwire\n  - Sammy Sosa\nrbi:\n  "
        "# 1998 rbi ranking\n  - Sammy Sosa\n  - Ken Griffey"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\nhr: \n  - Mark McGwire\n  - Sammy Sosa\nrbi: \n  - Sammy "
            "Sosa\n  - Ken Griffey\n...\n");
  }
  SECTION(
      "YAML parse dictionary with nested array plus comments and stringify.",
      "[YAML][Stringify][Comments]") {
    BufferSource source{"key: #comment 1\n   - value line 1\n   #comment 2\n   "
                        "- value line 2\n   #comment 3\n   - value line 3"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nkey: \n  - value line 1\n  - value "
                                      "line 2\n  - value line 3\n...\n");
  }
  SECTION("YAML parse an array of character art (one) and  stringify.",
          "[YAML][Stringify][Piped]") {
    BufferSource source{"---\n- |\n \\//||\\/||\n // ||  ||__\n..."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)[0]));
    REQUIRE(YRef<Array>(yaml.document(0)[0]).size() == 1);
    REQUIRE(YRef<String>(yaml.document(0)[0][0]).value() ==
            "\\//||\\/||\n// ||  ||__");
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n- |\n  \\//||\\/||\n  // ||  ||__\n...\n");
  }
  SECTION("YAML parse a boolean (True) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nTrue\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nTrue\n...\n");
  }
  SECTION("YAML parse a boolean (False) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nFalse\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nFalse\n...\n");
  }
  SECTION("YAML parse a boolean (On) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nOn\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nOn\n...\n");
  }
  SECTION("YAML parse a boolean (Off) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nOff\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nOff\n...\n");
  }
  SECTION("YAML parse a boolean (Yes) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nYes\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nYes\n...\n");
  }
  SECTION("YAML parse a boolean (No) and stringify.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nNo\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nNo\n...\n");
  }
}