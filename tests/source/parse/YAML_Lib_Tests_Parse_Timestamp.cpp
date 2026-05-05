#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML parsing of timestamps.", "[YAML][Parse][Timestamp]") {
  const YAML yaml;

  // ---- Date-only timestamps ----

  SECTION("YAML parse date-only timestamp.", "[YAML][Parse][Timestamp][Date]") {
    BufferSource source{"---\n2001-07-08\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
    REQUIRE(NRef<Timestamp>(yaml.document(0)).value() == "2001-07-08");
  }

  SECTION("YAML parse date-only timestamp as dictionary value.",
          "[YAML][Parse][Timestamp][Date]") {
    BufferSource source{"---\ndate: 2023-12-25\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Timestamp>(yaml.document(0)["date"]));
    REQUIRE(NRef<Timestamp>(yaml.document(0)["date"]).value() == "2023-12-25");
  }

  SECTION("YAML parse date-only timestamp in a sequence.",
          "[YAML][Parse][Timestamp][Date]") {
    BufferSource source{"---\n- 2020-01-01\n- 2021-06-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Array>(yaml.document(0)));
    REQUIRE(isA<Timestamp>(yaml.document(0)[0]));
    REQUIRE(NRef<Timestamp>(yaml.document(0)[0]).value() == "2020-01-01");
    REQUIRE(isA<Timestamp>(yaml.document(0)[1]));
    REQUIRE(NRef<Timestamp>(yaml.document(0)[1]).value() == "2021-06-15");
  }

  // ---- DateTime timestamps ----

  SECTION("YAML parse canonical datetime timestamp (ISO 8601 with T and Z).",
          "[YAML][Parse][Timestamp][DateTime]") {
    BufferSource source{"---\n2001-07-08T17:08:28Z\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
    REQUIRE(NRef<Timestamp>(yaml.document(0)).value() ==
            "2001-07-08T17:08:28Z");
  }

  SECTION("YAML parse datetime timestamp with space separator.",
          "[YAML][Parse][Timestamp][DateTime]") {
    BufferSource source{"---\nts: 2001-07-08 17:08:28\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Dictionary>(yaml.document(0)));
    REQUIRE(isA<Timestamp>(yaml.document(0)["ts"]));
    REQUIRE(NRef<Timestamp>(yaml.document(0)["ts"]).value() ==
            "2001-07-08 17:08:28");
  }

  SECTION("YAML parse datetime timestamp with timezone offset.",
          "[YAML][Parse][Timestamp][DateTime]") {
    BufferSource source{"---\nts: 2001-07-08T15:08:28+05:30\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)["ts"]));
    REQUIRE(NRef<Timestamp>(yaml.document(0)["ts"]).value() ==
            "2001-07-08T15:08:28+05:30");
  }

  // ---- !!timestamp tag ----

  SECTION("YAML parse value with !!timestamp tag.",
          "[YAML][Parse][Timestamp][Tag]") {
    BufferSource source{"---\n!!timestamp 2001-07-08\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
    REQUIRE(NRef<Timestamp>(yaml.document(0)).value() == "2001-07-08");
    REQUIRE(yaml.document(0).getTag() ==
            "tag:yaml.org,2002:timestamp");
  }

  // ---- Non-timestamps should not be parsed as Timestamp ----

  SECTION("YAML value that looks like date range is not a timestamp.",
          "[YAML][Parse][Timestamp][NonDate]") {
    // Contains text, not a pure date
    BufferSource source{"---\nnot-a-date\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(isA<Timestamp>(yaml.document(0)));
    REQUIRE(isA<String>(yaml.document(0)));
  }

  SECTION("YAML partial date is not a timestamp.",
          "[YAML][Parse][Timestamp][NonDate]") {
    // Only 3 digit year segment — not a valid date
    BufferSource source{"---\n200-07-08\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(isA<Timestamp>(yaml.document(0)));
  }

  SECTION("YAML integer is not parsed as timestamp.",
          "[YAML][Parse][Timestamp][NonDate]") {
    BufferSource source{"---\n2001\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(isA<Timestamp>(yaml.document(0)));
    REQUIRE(isA<Number>(yaml.document(0)));
  }

  // ---- Timestamp stringify ----

  SECTION("YAML timestamp stringifies back to its raw value.",
          "[YAML][Parse][Timestamp][Stringify]") {
    BufferSource source{"---\n2001-07-08\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("2001-07-08") != std::string::npos);
  }

  SECTION("YAML timestamp in dictionary stringifies correctly.",
          "[YAML][Parse][Timestamp][Stringify]") {
    BufferSource source{"---\ndate: 2023-12-25\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    BufferDestination dest;
    REQUIRE_NOTHROW(yaml.stringify(dest));
    REQUIRE(dest.toString().find("2023-12-25") != std::string::npos);
  }

  // ---- isA and NRef access ----

  SECTION("isA<Timestamp> returns true for timestamp node.",
          "[YAML][Parse][Timestamp][IsA]") {
    BufferSource source{"---\n2000-01-01\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
  }

  SECTION("isA<Timestamp> returns false for non-timestamp node.",
          "[YAML][Parse][Timestamp][IsA]") {
    BufferSource source{"---\nhello\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE_FALSE(isA<Timestamp>(yaml.document(0)));
  }

  // ---- toTm() / toTimeT() conversion (YAML_LIB_TIMESTAMP_PARSE=ON only) ----

#ifdef YAML_LIB_TIMESTAMP_PARSE
  SECTION("toTm() returns correct broken-down time for date-only timestamp.",
          "[YAML][Parse][Timestamp][toTm]") {
    BufferSource source{"---\n2024-03-15\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
    const std::tm t = NRef<Timestamp>(yaml.document(0)).toTm();
    REQUIRE(t.tm_year == 2024 - 1900);
    REQUIRE(t.tm_mon  == 3 - 1);   // 0-based
    REQUIRE(t.tm_mday == 15);
  }

  SECTION("toTm() returns correct time fields for datetime timestamp.",
          "[YAML][Parse][Timestamp][toTm]") {
    BufferSource source{"---\n2001-07-08T17:08:28Z\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
    const std::tm t = NRef<Timestamp>(yaml.document(0)).toTm();
    REQUIRE(t.tm_year == 2001 - 1900);
    REQUIRE(t.tm_mon  == 7 - 1);
    REQUIRE(t.tm_mday == 8);
    REQUIRE(t.tm_hour == 17);
    REQUIRE(t.tm_min  == 8);
    REQUIRE(t.tm_sec  == 28);
  }

  SECTION("toTimeT() returns a positive value for a valid timestamp.",
          "[YAML][Parse][Timestamp][toTimeT]") {
    BufferSource source{"---\n2024-01-01\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
    REQUIRE(isA<Timestamp>(yaml.document(0)));
    const std::time_t t = NRef<Timestamp>(yaml.document(0)).toTimeT();
    REQUIRE(t > 0);
  }
#endif // YAML_LIB_TIMESTAMP_PARSE
}
