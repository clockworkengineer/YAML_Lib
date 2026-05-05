#include "YAML_Lib_Tests.hpp"

using namespace YAML_Lib;

TEST_CASE("Check YAML schema validation.", "[YAML][Schema][Validate]") {

  // -------------------------------------------------------------------------
  // Constexpr schema defined once — lives in ROM in real embedded builds.
  // -------------------------------------------------------------------------
  static constexpr FieldSchema kDeviceFields[] = {
      {"host",   NodeType::String,  true},
      {"port",   NodeType::Number,  true},
      {"debug",  NodeType::Boolean, false},
      {"name",   NodeType::String,  false},
  };
  static constexpr Schema kDeviceSchema{kDeviceFields, 4};

  SECTION("Valid document with all fields passes validation.",
          "[YAML][Schema][Validate][Pass]") {
    const YAML yaml;
    BufferSource src{
        "---\nhost: example.com\nport: 8080\ndebug: true\nname: device1\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kDeviceSchema);
    REQUIRE(errors.empty());
  }

  SECTION("Document with only required fields passes validation.",
          "[YAML][Schema][Validate][Pass]") {
    const YAML yaml;
    BufferSource src{"---\nhost: example.com\nport: 8080\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kDeviceSchema);
    REQUIRE(errors.empty());
  }

  SECTION("Missing required field produces one error.",
          "[YAML][Schema][Validate][Fail]") {
    const YAML yaml;
    // 'port' is required but absent
    BufferSource src{"---\nhost: example.com\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kDeviceSchema);
    REQUIRE(errors.size() == 1);
    REQUIRE(std::string_view{errors[0].key} == "port");
    REQUIRE(std::string_view{errors[0].message} == "required key missing");
  }

  SECTION("Both required fields missing produces two errors.",
          "[YAML][Schema][Validate][Fail]") {
    const YAML yaml;
    BufferSource src{"---\ndebug: false\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kDeviceSchema);
    REQUIRE(errors.size() == 2);
  }

  SECTION("Wrong type for a field produces one error.",
          "[YAML][Schema][Validate][Fail]") {
    const YAML yaml;
    // 'port' should be a Number but is given as a String (quoted)
    BufferSource src{"---\nhost: example.com\nport: \"not-a-number\"\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kDeviceSchema);
    REQUIRE(errors.size() == 1);
    REQUIRE(std::string_view{errors[0].key} == "port");
    REQUIRE(std::string_view{errors[0].message} == "value has unexpected type");
  }

  SECTION("Non-dictionary document produces document-level error.",
          "[YAML][Schema][Validate][Fail]") {
    const YAML yaml;
    BufferSource src{"---\n- item1\n- item2\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kDeviceSchema);
    REQUIRE(errors.size() == 1);
    REQUIRE(std::string_view{errors[0].key} == "");
    REQUIRE(std::string_view{errors[0].message} == "Document is not a Dictionary");
  }

  SECTION("Extra keys not in schema are silently allowed.",
          "[YAML][Schema][Validate][Pass]") {
    const YAML yaml;
    BufferSource src{
        "---\nhost: example.com\nport: 8080\nextra_key: some_value\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kDeviceSchema);
    REQUIRE(errors.empty());
  }

  SECTION("NodeType::Any wildcard passes for any value type.",
          "[YAML][Schema][Validate][Any]") {
    static constexpr FieldSchema kAnyFields[] = {
        {"value", NodeType::Any, true},
    };
    static constexpr Schema kAnySchema{kAnyFields, 1};

    const YAML yaml;
    BufferSource src{"---\nvalue: 42\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kAnySchema);
    REQUIRE(errors.empty());
  }

  SECTION("Schema validates nested Array type correctly.",
          "[YAML][Schema][Validate][Types]") {
    static constexpr FieldSchema kFields[] = {
        {"items", NodeType::Array, true},
    };
    static constexpr Schema kSchema{kFields, 1};

    const YAML yaml;
    BufferSource src{"---\nitems:\n  - a\n  - b\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kSchema);
    REQUIRE(errors.empty());
  }

  SECTION("Schema detects Array field given wrong type.",
          "[YAML][Schema][Validate][Types]") {
    static constexpr FieldSchema kFields[] = {
        {"items", NodeType::Array, true},
    };
    static constexpr Schema kSchema{kFields, 1};

    const YAML yaml;
    BufferSource src{"---\nitems: not_an_array\n"};
    REQUIRE_NOTHROW(yaml.parse(src));
    const auto errors = validateAgainst(yaml.document(0), kSchema);
    REQUIRE(errors.size() == 1);
    REQUIRE(std::string_view{errors[0].key} == "items");
  }
}
