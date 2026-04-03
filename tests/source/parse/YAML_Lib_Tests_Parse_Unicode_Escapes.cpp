#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parsing of Unicode/escape sequences.",
          "[YAML][Parse][Escape][Unicode]") {
  const YAML yaml;

  // ---- YAML 1.2 single-character escape additions ----

  SECTION("YAML parse double quoted string with \\0 (null char, read-only).",
          "[YAML][Parse][Escape][Null]") {
    // \0 is a valid YAML 1.2 read escape, but the platform UTF converter
    // cannot round-trip null bytes, so parsing throws an Error.
    BufferSource source{"---\n\"before\\0after\"\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }

  SECTION("YAML parse double quoted string with \\a (bell).",
          "[YAML][Parse][Escape][Bell]") {
    BufferSource source{"---\n\"before\\aafter\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    const std::string expected = "before\aafter";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION("YAML parse double quoted string with \\v (vertical tab).",
          "[YAML][Parse][Escape][VTab]") {
    BufferSource source{"---\n\"before\\vafter\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    const std::string expected = "before\vafter";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION("YAML parse double quoted string with \\e (escape char).",
          "[YAML][Parse][Escape][Esc]") {
    BufferSource source{"---\n\"before\\eafter\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    const std::string expected = std::string("before") + '\x1b' + "after";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION("YAML parse double quoted string with \\/ (forward slash).",
          "[YAML][Parse][Escape][ForwardSlash]") {
    BufferSource source{"---\n\"path\\/to\\/file\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "path/to/file");
  }

  SECTION("YAML parse double quoted string with \\  (escaped space).",
          "[YAML][Parse][Escape][Space]") {
    BufferSource source{"---\n\"hello\\ world\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "hello world");
  }

  // ---- Unicode escape sequences ----

  SECTION("YAML parse double quoted string with \\u (4 hex digits).",
          "[YAML][Parse][Escape][U4]") {
    // \u0041 = 'A'
    BufferSource source{"---\n\"\\u0041BC\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "ABC");
  }

  SECTION("YAML parse double quoted string with \\x (2 hex digits).",
          "[YAML][Parse][Escape][X2]") {
    // \x41 = 'A'
    BufferSource source{"---\n\"\\x41BC\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "ABC");
  }

  SECTION("YAML parse double quoted string with \\U (8 hex digits, BMP).",
          "[YAML][Parse][Escape][U8BMP]") {
    // \U00000041 = 'A'
    BufferSource source{"---\n\"\\U00000041BC\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    REQUIRE(NRef<String>(yaml.document(0)).value() == "ABC");
  }

  SECTION("YAML parse double quoted string with \\U for SMP codepoint.",
          "[YAML][Parse][Escape][U8SMP]") {
    // \U0001F600 = emoji grinning face (U+1F600), UTF-8: F0 9F 98 80
    BufferSource source{"---\n\"\\U0001F600\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    // Check the UTF-8 sequence for U+1F600
    const std::string expected = "\xF0\x9F\x98\x80";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION("YAML parse double quoted string with \\N (next line U+0085).",
          "[YAML][Parse][Escape][NextLine]") {
    BufferSource source{"---\n\"before\\Nafter\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    // U+0085 in UTF-8 is 0xC2 0x85
    const std::string expected = "before\xC2\x85"
                                 "after";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION(
      "YAML parse double quoted string with \\_ (non-breaking space U+00A0).",
      "[YAML][Parse][Escape][NBSP]") {
    BufferSource source{"---\n\"before\\_after\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    // U+00A0 in UTF-8 is 0xC2 0xA0
    const std::string expected = "before\xC2\xA0"
                                 "after";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION("YAML parse double quoted string with \\L (line separator U+2028).",
          "[YAML][Parse][Escape][LineSep]") {
    BufferSource source{"---\n\"before\\Lafter\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    // U+2028 in UTF-8 is 0xE2 0x80 0xA8
    const std::string expected = "before\xE2\x80\xA8"
                                 "after";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION(
      "YAML parse double quoted string with \\P (paragraph separator U+2029).",
      "[YAML][Parse][Escape][ParaSep]") {
    BufferSource source{"---\n\"before\\Pafter\"\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<String>(yaml.document(0)));
    // U+2029 in UTF-8 is 0xE2 0x80 0xA9
    const std::string expected = "before\xE2\x80\xA9"
                                 "after";
    REQUIRE(NRef<String>(yaml.document(0)).value() == expected);
  }

  SECTION("YAML parse invalid \\U with too few hex digits throws.",
          "[YAML][Parse][Escape][U8Error]") {
    BufferSource source{"---\n\"\\U0041\"\n"};
    REQUIRE_THROWS(yaml.parse(source));
  }
}
