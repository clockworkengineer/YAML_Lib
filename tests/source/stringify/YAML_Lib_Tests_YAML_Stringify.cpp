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
    REQUIRE(destination.toString() == "---\n65000...\n");
  }
  SECTION("YAML Stringify document one string.", "[YAML][Stringify][String]") {
    BufferSource source{"---\n\" test string \""};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n\" test string \"...\n");
  }
  SECTION("YAML Stringify document one boolean.",
          "[YAML][Stringify][Boolean]") {
    BufferSource source{"---\nTrue"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\nTrue...\n");
  }
  SECTION("YAML Stringify document dictionary key value pair.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{"---\n doe: 'a deer, a female deer'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n doe: 'a deer, a female deer'\n...\n");
  }
  SECTION("YAML Stringify document dictionary two key value pairs.",
          "[YAML][Stringify][Dictionary]") {
    BufferSource source{
        "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden sun'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() ==
            "---\n doe: 'a deer, a female deer'\n ray: 'a drop of golden "
            "sun'\n...\n");
  }
  SECTION("YAML Parse array with one string elements and restringify.",
          "[YAML][Parse][Array]") {
    BufferSource source{"---\n   - \"One\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n   - \"One\"\n...\n");
  }
  SECTION("YAML Parse array with multiple string elements and restringify.",
          "[YAML][Parse][Array]") {
    BufferSource source{
        "---\n  - \"One\"\n  - \"Two\"\n  - \"Three\"\n  - \"Four\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination destination;
    REQUIRE_NOTHROW(yaml.stringify(destination));
    REQUIRE(destination.toString() == "---\n  - \"One\"\n  - \"Two\"\n  "
                                      "- \"Three\"\n  - \"Four\"\n...\n");
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
//   SECTION("YAML Stringify plain literals newlines treatd as space.",
//           "[YAML][Stringify][literals]") {
//     BufferSource source{
//         "---\n  Mark McGwire\'s\n  year was crippled\n  by a knee injury."};
//     REQUIRE_NOTHROW(yaml.parse(source));
//     BufferDestination destination;
//     REQUIRE_NOTHROW(yaml.stringify(destination));
//     REQUIRE(destination.toString() ==
//             "---\nMark McGwire\'s year was crippled by a knee injury.\n...\n");
//   }
  //   SECTION("YAML Stringify literals perserves newlines.",
  //   "[YAML][Stringify][literals]") {
  //     BufferSource source{"--- |\n  \\//||\\/||\n  // ||  ||__"};
  //     REQUIRE_NOTHROW(yaml.parse(source));
  //     BufferDestination destination;
  //     REQUIRE_NOTHROW(yaml.stringify(destination));
  //     REQUIRE(destination.toString() == "--- |\n\\//||\\/||\n// ||
  //     ||__\n...");
  //   }
}