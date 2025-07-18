#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of simple scalar types.",
          "[YAML][Parse][Scalar][String]") {
  const YAML yaml;
  SECTION("YAML parse a unquoted quoted string.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\ntest string.\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "test string.");
    REQUIRE(NRef<String>(yaml.document(0)).getQuote() == kNull);
  }
  SECTION("YAML parse a double quoted string.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n\"test string.\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "test string.");
    REQUIRE(NRef<String>(yaml.document(0)).getQuote() == kDoubleQuote);
  }
  SECTION("YAML parse a single quoted string.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n'test string.'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "test string.");
    REQUIRE(NRef<String>(yaml.document(0)).getQuote() == kApostrophe);
  }
  SECTION(
      "YAML parse a double quoted string with some common escape sequences in.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{
        "---\n\"test string.\\t.\\n.\\b.\\r.\\f.\\\\.\\\".\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "test string.\t.\n.\b.\r.\f.\\.\".");
  }
  SECTION(
      "YAML parse a single quoted string with some common escape sequences in.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n'test string.\\t.\\n.\\b.\\r.\\f.\\\\.\\\".'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "test string.\\t.\\n.\\b.\\r.\\f.\\\\.\\\".");
  }
  // Single quoted strings have no escape translation
  SECTION("YAML parse an single quoted string with an escape sequence.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n 'test string \\n. ' \n"};
    REQUIRE_NOTHROW(yaml.parse(source));

    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "test string \\n. ");
  }
  // Single quoted strings '' -> '
  SECTION("YAML parse an single quoted string with escaped single quotes.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n 'test string ''.''. ' \n"};
    REQUIRE_NOTHROW(yaml.parse(source));

    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "test string '.'. ");
  }
  SECTION("YAML parse an unquoted string with that terminated by EOF.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n test string."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "test string.");
  }
  SECTION("Parse Array of plain flow scalars with blank line at end.",
          "[YAML][Parse][Examples][File]") {
    BufferSource yamlSource{
        "---\n- Mark Joseph\n- James Stephen\n- Ken Griffey\n\n"};
    REQUIRE_NOTHROW(yaml.parse(yamlSource));
    REQUIRE_FALSE(!isA<Array>(yaml.document(0)));
    REQUIRE(NRef<Array>(yaml.document(0)).size() == 3);
    REQUIRE(NRef<String>(yaml.document(0)[0]).value() == "Mark Joseph");
    REQUIRE(NRef<String>(yaml.document(0)[1]).value() == "James Stephen");
    REQUIRE(NRef<String>(yaml.document(0)[2]).value() == "Ken Griffey");
  }
  SECTION("YAML parse a string block folded (kLineFeed -> ' ') scalar.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "this is not a normal string it spans more than one line see?");
  }
  SECTION("YAML parse a string block literal scalar.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\n |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "this is not a normal string it\nspans more than\none line\nsee?");
  }
  SECTION("YAML parse a plain string block.", "[YAML][Parse][Scalar][String]") {
    BufferSource source{
        "---\n  Mark McGwire\'s\n  year was crippled\n  by a knee injury."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "Mark McGwire's year was crippled by a knee injury.");
  }
  SECTION(
      "YAML parse a folded string block that is terminated to key value pair.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?\nfoo: True\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!NRef<Dictionary>(yaml.document(0)).contains("bar"));
    REQUIRE_FALSE(!NRef<Dictionary>(yaml.document(0)).contains("foo"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)["bar"]));
    REQUIRE(NRef<String>(yaml.document(0)["bar"]).value() ==
            "this is not a normal string it spans more than one line see?");
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)["foo"]));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)["foo"]).value());
  }
  SECTION("YAML parse a folder string block that terminates early.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\nsee?"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 6 Column: 1]: Missing "
                        "key/value pair from indentation level.");
  }
  SECTION(
      "YAML parse a literal string block that is terminated to key value pair.",
      "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?\nfoo: True\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!NRef<Dictionary>(yaml.document(0)).contains("bar"));
    REQUIRE_FALSE(!NRef<Dictionary>(yaml.document(0)).contains("foo"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)["bar"]));
    REQUIRE(NRef<String>(yaml.document(0)["bar"]).value() ==
            "this is not a normal string it\nspans more than\none line\nsee?");
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)["foo"]));
    REQUIRE_FALSE(!NRef<Boolean>(yaml.document(0)["foo"]).value());
  }
  SECTION("YAML parse a literal string block that terminates early.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\nbar: |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\nsee?"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Syntax Error [Line: 6 Column: 1]: Missing "
                        "key/value pair from indentation level.");
  }
  SECTION("YAML parse regular multi-line string.", "[YAML][Parse][literals]") {
    BufferSource source{
        "---\n Sammy Sosa completed another\n fine season with great "
        "stats.\n   63 Home Runs\n   0.288 Batting Average\n What a year!"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "Sammy Sosa completed another fine season with great stats. 63 "
            "Home Runs 0.288 Batting Average What a year!");
  }
  SECTION("YAML parse block string folded newlines preserved for indented and"
          "blank lines.",
          "[YAML][Parse][Folded]") {
    BufferSource source{
        "--- >\n Sammy Sosa completed another\n fine season with great "
        "stats.\n\n   63 Home Runs\n   0.288 Batting Average\n\n What a year!"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)).value() ==
            "Sammy Sosa completed another fine season with great stats.\n\n   "
            "63 Home Runs\n   0.288 Batting Average\n\nWhat a year!");
  }
  SECTION("YAML parse block string example with folded newlines preserved for "
          "indented and"
          "blank lines.",
          "[YAML][Parse][folded]") {
    BufferSource source{
        "example: >\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)["example"]).value() ==
            "Several lines of text, with some \" quotes of various \'types\', "
            "and also a blank line:\n\nand some text with \n    extra "
            "indentation\non the next line, plus another line at the end.\n");
  }
  SECTION("YAML parse block string example with folded newlines preserved for "
          "indented and"
          "blank lines plus last newline stripped.",
          "[YAML][Parse][folded]") {
    BufferSource source{
        "example: >-\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)["example"]).value() ==
            "Several lines of text, with some \" quotes of various \'types\', "
            "and also a blank line:\n\nand some text with \n    extra "
            "indentation\non the next line, plus another line at the end.");
  }
  SECTION("YAML parse block string example with folded newlines preserved for "
          "indented and"
          "blank lines plus kepp all trailing newlines.",
          "[YAML][Parse][folded]") {
    BufferSource source{
        "example: >+\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        NRef<String>(yaml.document(0)["example"]).value() ==
        "Several lines of text, with some \" quotes of various \'types\', "
        "and also a blank line:\n\nand some text with \n    extra "
        "indentation\non the next line, plus another line at the end.\n\n\n");
  }
  SECTION("YAML parse block string literal newlines preserved.",
          "[YAML][Parse][Literal]") {
    BufferSource source{
        "example: |\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        NRef<String>(yaml.document(0)["example"]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.\n");
  }
  SECTION("YAML parse block string literal newlines preserved and last newline "
          "stripped.",
          "[YAML][Parse][Literal]") {
    BufferSource source{
        "example: |-\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        NRef<String>(yaml.document(0)["example"]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.");
  }
  SECTION("YAML parse block string literal newlines preserved and plus keep "
          "all trailing newlines.",
          "[YAML][Parse][Literal]") {
    BufferSource source{
        "example: |+\n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n\n\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        NRef<String>(yaml.document(0)["example"]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.\n\n\n");
  }
  SECTION("YAML parse single quoted flow scalar",
          "[YAML][Parse][Flow Scalar]") {
    BufferSource source{
        "example: \'Several lines of text,\n  containing \'\'single "
        "quotes\'\'. Escapes (like \\n) don\'\'t do anything.\n  \n  Newlines "
        "can be added by leaving a blank line.\n    Leading whitespace on "
        "lines is ignored.\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)["example"]).value() ==
            "Several lines of text, containing \'single quotes\'. Escapes "
            "(like \\n) don\'t do anything.\nNewlines can be added by "
            "leaving a blank line. Leading whitespace on lines is ignored.");
  }
  SECTION("YAML parse double quoted flow scalar",
          "[YAML][Parse][Flow Scalar]") {
    BufferSource source{
        "example: \"Several lines of text,\n  containing \\\"double "
        "quotes\\\". Escapes (like \\\\n) work.\\nIn addition,\n  newlines"
        " can be escaped to prevent them from being converted to a"
        " space.\n  \n  "
        "Newlines can also be added by leaving a blank"
        " line.\n    Leading "
        "whitespace on lines is ignored.\""};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)["example"]).value() ==
            "Several lines of text, containing \"double quotes\". Escapes "
            "(like \\n) work.\nIn addition, newlines can be escaped to prevent "
            "them from being converted to a space.\nNewlines can also be added "
            "by leaving a blank line. Leading whitespace on lines is ignored.");
  }
  SECTION("YAML parse plain flow scalar", "[YAML][Parse][Flow Scalar]") {
    BufferSource source{
        "example: Several lines of text,\n  with some \"quotes\" of various "
        "\'types\'.\n  Escapes (like \\n) don\'t do anything.\n  \n  Newlines "
        "can be added by leaving a blank line.\n    Additional leading "
        "whitespace is ignored."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)["example"]).value() ==
            "Several lines of text, with some \"quotes\" of various \'types\'. "
            "Escapes (like \\n) don\'t do anything.\nNewlines can be added by "
            "leaving a blank line. Additional leading whitespace is ignored.");
  }
  SECTION("YAML parse various quoted scalars",
          "[YAML][Parse][Quoted Scalars]") {
    BufferSource source{
        "unicode: \"Sosa did fine.\\u263A\"\ncontrol: "
        "\"\\b1998\\t1999\\t2000\\n\"\nhexesc:  \"\\x13\\x10 is "
        "\\r\\n\"\n\nsingle: \'\"Howdy!\" he cried.\'\nquoted: \' # not a "
        "\'\'comment\'\'.\'\ntie-fighter: \'|\\-*-/|\'"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(NRef<String>(yaml.document(0)["unicode"]).value() ==
            "Sosa did fine.☺");
    REQUIRE(NRef<String>(yaml.document(0)["control"]).value() ==
            "\b1998\t1999\t2000\n");
    REQUIRE(NRef<String>(yaml.document(0)["hexesc"]).value() ==
            "\x13\x10 is \r\n");
    REQUIRE(NRef<String>(yaml.document(0)["single"]).value() ==
            "\"Howdy!\" he cried.");
    REQUIRE(NRef<String>(yaml.document(0)["quoted"]).value() ==
            " # not a 'comment'.");
    REQUIRE(NRef<String>(yaml.document(0)["tie-fighter"]).value() ==
            "|\\-*-/|");
  }
  SECTION("YAML parse array of block block strings literal newlines preserved.",
          "[YAML][Parse][Literal]") {
    BufferSource source{
        "- | \n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n"
        "- | \n  Several lines of text,\n  with some \" quotes"
        " of various 'types',\n  and also a blank line:\n\n  and some text "
        "with\n    extra indentation\n  on the next line,\n  plus another line "
        "at the end.\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(
        NRef<String>(yaml.document(0)[0]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.");
    REQUIRE(
        NRef<String>(yaml.document(0)[1]).value() ==
        "Several lines of text,\nwith some \" quotes of various "
        "\'types\',\nand also a blank line:\n\nand some text with\n    extra "
        "indentation\non the next line,\nplus another line at the end.");
  }
  SECTION("YAML parse a plain string block terminated by indentation level.",
          "[YAML][Parse][Scalar][String]") {
    BufferSource source{"---\ntest1:\n   Mark McGwire\'s\n   year was "
                        "crippled\n   by a knee injury.\ntest2: Mark Twain\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)["test1"]).value() ==
            "Mark McGwire's year was crippled by a knee injury.");
    REQUIRE(NRef<String>(yaml.document(0)["test2"]).value() == "Mark Twain");
  }
}