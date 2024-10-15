#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of simple scalar types.",
          "[YAML][Parse][Scalar][String]") {
  const YAML yaml;
  SECTION("YAML parse a unquoted quoted string.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\ntest string.\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "test string.");
    REQUIRE(YRef<String>(yaml.document(0)[0]).getQuote() == '\0');
  }
  SECTION("YAML parse a double quoted string.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n\"test string.\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "test string.");
    REQUIRE(YRef<String>(yaml.document(0)[0]).getQuote() == '"');
  }
  SECTION("YAML parse a single quoted string.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n'test string.'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "test string.");
    REQUIRE(YRef<String>(yaml.document(0)[0]).getQuote() == '\'');
  }
  SECTION(
      "YAML parse a qouble quoted string with some common escape sequences in.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{
        "---\n\"test string.\\t.\\n.\\b.\\r.\\f.\\\\.\\\".\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "test string.\t.\n.\b.\r.\f.\\.\".");
  }
  SECTION(
      "YAML parse a single quoted string with some common escape sequences in.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n'test string.\\t.\\n.\\b.\\r.\\f.\\\\.\\\".'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "test string.\\t.\\n.\\b.\\r.\\f.\\\\.\\\".");
  }
  // Single quoted strings have no escape translation
  SECTION("YAML parse an single quoted string with an escape sequence.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n 'test string \\n. ' \n"};
    REQUIRE_NOTHROW(yaml.parse(source));

    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "test string \\n. ");
  }
  // Single quoted strings '' -> '
  SECTION("YAML parse an single quoted string with escaped single quotes.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n 'test string ''.''. ' \n"};
    REQUIRE_NOTHROW(yaml.parse(source));

    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "test string '.'. ");
  }
  SECTION("YAML parse an unquoted string with that terminated by EOF.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n test string."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() == "test string.");
  }
  SECTION("YAML parse a string block folded ('\n' -> ' ') scalar.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "this is not a normal string it spans more than one line see?");
  }
  SECTION("YAML parse a string block literal scalar.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "this is not a normal string it\nspans more than\none line\nsee?");
  }
  SECTION("YAML parse a plain string block.", "[YAML][Parse][Scalar][String]") {
    BufferSource source{
        "---\n  Mark McGwire\'s\n  year was crippled\n  by a knee injury."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "Mark McGwire's year was crippled by a knee injury.");
  }
  SECTION(
      "YAML parse a folded string block that is terminated to key value pair.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?\nfoo: True\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("bar"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("foo"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["bar"]));
    REQUIRE(YRef<String>(yaml.document(0)[0]["bar"]).value() ==
            "this is not a normal string it spans more than one line see?");
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]["foo"]));
    REQUIRE_FALSE(!YRef<Boolean>(yaml.document(0)[0]["foo"]).value());
  }
  SECTION("YAML parse a folder string block that terminates early.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\nsee?"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "IParser Error: Invalid key 'see?' specified.");
  }
  SECTION(
      "YAML parse a literal string block that is terminated to key value pair.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?\nfoo: True\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("bar"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("foo"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["bar"]));
    REQUIRE(YRef<String>(yaml.document(0)[0]["bar"]).value() ==
            "this is not a normal string it\nspans more than\none line\nsee?");
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]["foo"]));
    REQUIRE_FALSE(!YRef<Boolean>(yaml.document(0)[0]["foo"]).value());
  }
  SECTION("YAML parse a literal string block that terminates early.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\nsee?"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "IParser Error: Invalid key 'see?' specified.");
  }
  SECTION("YAML parse regular multi-line string.",
          "[YAML][Stringify][literals]") {
    BufferSource source{
        "---\n Sammy Sosa completed another\n fine season with great "
        "stats.\n   63 Home Runs\n   0.288 Batting Average\n What a year!"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "Sammy Sosa completed another fine season with great stats. 63 "
            "Home Runs 0.288 Batting Average What a year!");
  }
  SECTION("YAML parse block string folded newlines preserved for indented and"
          "blank lines.",
          "[YAML][Stringify][Folded]") {
    BufferSource source{
        "--- >\n Sammy Sosa completed another\n fine season with great "
        "stats.\n\n   63 Home Runs\n   0.288 Batting Average\n\n What a year!"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<String>(yaml.document(0)[0]).value() ==
            "Sammy Sosa completed another fine season with great stats.\n\n   "
            "63 Home Runs\n   0.288 Batting Average\n\nWhat a year!");
  }
  SECTION("YAML parse block string example with folded newlines preserved for "
          "indented and"
          "blank lines.",
          "[YAML][Stringify][folded]") {
    BufferSource source{
        "example: >\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<String>(yaml.document(0)[0]["example"]).value() ==
            "Several lines of text, with some \" quotes of various \'types\', "
            "and also a blank line:\n\nand some text with \n    extra "
            "indentation\non the next line, plus another line at the end.\n");
  }
  SECTION("YAML parse block string example with folded newlines preserved for "
          "indented and"
          "blank lines plus last newline stripped.",
          "[YAML][Stringify][folded]") {
    BufferSource source{
        "example: >-\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<String>(yaml.document(0)[0]["example"]).value() ==
            "Several lines of text, with some \" quotes of various \'types\', "
            "and also a blank line:\n\nand some text with \n    extra "
            "indentation\non the next line, plus another line at the end.");
  }
  SECTION("YAML parse block string example with folded newlines preserved for "
          "indented and"
          "blank lines plus kepp all trailing newlines.",
          "[YAML][Stringify][folded]") {
    BufferSource source{
        "example: >+\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        YRef<String>(yaml.document(0)[0]["example"]).value() ==
        "Several lines of text, with some \" quotes of various \'types\', "
        "and also a blank line:\n\nand some text with \n    extra "
        "indentation\non the next line, plus another line at the end.\n\n\n");
  }
  SECTION("YAML parse block string literal newlines preserved.",
          "[YAML][Stringify][Literal]") {
    BufferSource source{
        "example: |\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        YRef<String>(yaml.document(0)[0]["example"]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.\n");
  }
  SECTION("YAML parse block string literal newlines preserved and last newline "
          "stripped.",
          "[YAML][Stringify][Literal]") {
    BufferSource source{
        "example: |-\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        YRef<String>(yaml.document(0)[0]["example"]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.");
  }
  SECTION("YAML parse block string literal newlines preserved and plus keep "
          "all trailing newlines.",
          "[YAML][Stringify][Literal]") {
    BufferSource source{
        "example: |+\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        YRef<String>(yaml.document(0)[0]["example"]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.\n\n\n");
  }
  SECTION("YAML parse block single quoted flow scalar",
          "[YAML][Stringify][Flow Scalar]") {
    BufferSource source{
        "example: \'Several lines of text,\n  containing \'\'single "
        "quotes\'\'. Escapes (like \\n) don\'\'t do anything.\n  \n  Newlines "
        "can be added by leaving a blank line.\n    Leading whitespace on "
        "lines is ignored.\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(YRef<String>(yaml.document(0)[0]["example"]).value() ==
            "Several lines of text, containing \'single quotes\'. Escapes "
            "(like \\n) don\'t do anything.\nNewlines can be added by "
            "leaving a blank line. Leading whitespace on lines is ignored.");
  }
  //   SECTION("YAML parse block double quoted flow scalar",
  //           "[YAML][Stringify][Flow Scalar]") {
  //     BufferSource source{
  //         "example: \"Several lines of text,\n  containing \\\"double "
  //         "quotes\\\". Escapes (like \\\\n) work.\\nIn addition,\n  newlines
  //         can " "be escaped to prevent them from being converted to a
  //         space.\n  \n  " "Newlines can also be added by leaving a blank
  //         line.\\n\n    Leading " "whitespace on lines is ignored.\""};
  //     REQUIRE_NOTHROW(yaml.parse(source));
  //     REQUIRE(YRef<String>(yaml.document(0)[0]["example"]).value() == "");
  //   }
}