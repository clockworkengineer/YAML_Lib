#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parsing of simple types.", "[YAML][Parse][String]") {
  const YAML yaml;
  SECTION("YAML Parse a single quoted string.", "[YAML][Parse][String]") {
    BufferSource source{"---\n'test string.'\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).value() ==
            "test string.");
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).getQuote() == '\'');
  }
  SECTION("YAML Parse a double quoted string.", "[YAML][Parse][String]") {
    BufferSource source{"---\n\"test string.\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).value() ==
            "test string.");
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).getQuote() == '"');
  }
  SECTION("YAML Parse a qouted string with some escape sequences in.",
          "[YAML][Parse][String]") {
    BufferSource source{
        "---\n\"test string.\\t.\\n.\\b.\\r.\\f.\\\\.\\\".\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).value() ==
            "test string.\t.\n.\b.\r.\f.\\.\".");
  }
  SECTION("YAML Parse an unquoted string.", "[YAML][Parse][String]") {
    BufferSource source{"---\n test string. \n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).value() ==
            "test string. ");
  }
  SECTION("YAML Parse an unquoted string with an escape sequence.",
          "[YAML][Parse][String]") {
    BufferSource source{"---\n test string \\n. \n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).value() ==
            "test string \\n. ");
  }
  // Single quoted strings have no escape translation
  SECTION("YAML Parse an single quoted string with an escape sequence.",
          "[YAML][Parse][String]") {
    BufferSource source{"---\n 'test string \\n. ' \n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).value() ==
            "test string \\n. ");
  }
  SECTION("YAML Parse an unquoted string with that terminated by EOF.",
          "[YAML][Parse][String]") {
    BufferSource source{"---\n test string."};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]).value() ==
            "test string.");
  }
  SECTION("YAML Parse a string block.", "[YAML][Parse][String]") {
    BufferSource source{"---\nbar: >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["bar"]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]["bar"]).value() ==
            "this is not a normal string it spans more than one line see?");
  }
  SECTION("YAML Parse a string block that is terminated to key value pair.",
          "[YAML][Parse][String]") {
    BufferSource source{"---\nbar: >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?\nfoo: True\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("bar"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("foo"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["bar"]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]["bar"]).value() ==
            "this is not a normal string it spans more than one line see?");
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]["foo"]));
    REQUIRE_FALSE(!YRef<Boolean>(yaml.document(0)[0]["foo"]).value());
  }
  SECTION("YAML Parse a string block that terminates early.",
          "[YAML][Parse][String]") {
    BufferSource source{"---\nbar: >\n  this is not a normal string it\n  "
                        "spans more than\n  one line\nsee?"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Error: Invalid key 'see?' specified.");
  }
  SECTION("YAML Parse a piped string block.", "[YAML][Parse][String]") {
    BufferSource source{"---\nbar: |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!isA<Dictionary>(yaml.document(0)[0]));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["bar"]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]["bar"]).value() ==
            "this is not a normal string it\nspans more than\none line\nsee?");
  }
  SECTION(
      "YAML Parse a piped string block that is terminated to key value pair.",
      "[YAML][Parse][String]") {
    BufferSource source{"---\nbar: |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\n  see?\nfoo: True\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(yaml.getNumberOfDocuments() == 1);
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("bar"));
    REQUIRE_FALSE(!YRef<Dictionary>(yaml.document(0)[0]).contains("foo"));
    REQUIRE_FALSE(!isA<String>(yaml.document(0)[0]["bar"]));
    REQUIRE(YAML_Lib::YRef<String>(yaml.document(0)[0]["bar"]).value() ==
            "this is not a normal string it\nspans more than\none line\nsee?");
    REQUIRE_FALSE(!isA<Boolean>(yaml.document(0)[0]["foo"]));
    REQUIRE_FALSE(!YRef<Boolean>(yaml.document(0)[0]["foo"]).value());
  }
  SECTION("YAML Parse a piped string block that terminates early.",
          "[YAML][Parse][String]") {
    BufferSource source{"---\nbar: |\n  this is not a normal string it\n  "
                        "spans more than\n  one line\nsee?"};
    REQUIRE_THROWS_WITH(yaml.parse(source),
                        "YAML Error: Invalid key 'see?' specified.");
  }
}