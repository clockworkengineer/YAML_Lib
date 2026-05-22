//
// Program: YAML_Error_Handling_Demo
//
// Description: Demonstrates best-practice error handling patterns when using
// YAML_Lib: catching SyntaxError for parse failures, guarding against missing
// keys or wrong types, and validating YAML structure before use.
//
// Dependencies: C++20, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;

// ---------------------------------------------------------------------------
// Helper: try to parse source and report any SyntaxError.
// ---------------------------------------------------------------------------
static void tryParse(const std::string &label, const std::string &yamlText) {
  yl::YAML yaml;
  std::cout << "--- Trying: " << label;
  try {
    yl::BufferSource source{yamlText};
    yaml.parse(source);
    std::cout << "  OK: parsed " << yaml.getNumberOfDocuments()
              << " document(s).";
  } catch (const yl::SyntaxError &ex) {
    std::cerr << "  SyntaxError: " << ex.what();
  } catch (const std::exception &ex) {
    std::cerr << "  Unexpected error: " << ex.what();
  }
}

// ---------------------------------------------------------------------------
// Helper: demonstrate safe key access via contains() before indexing.
// ---------------------------------------------------------------------------
static void safeKeyAccess() {
  yl::YAML yaml;
  yl::BufferSource source{"---\n"
                          "username: alice\n"
                          "role: admin\n"};
  yaml.parse(source);

  const auto &doc = yaml.document(0);

  // Safe: check before accessing
  if (yl::NRef<yl::Dictionary>(doc).contains("username")) {
    std::cout << "  username = "
              << yl::NRef<yl::String>(doc["username"]).value();
  }

  // The key 'email' is not present — contains() prevents a crash
  if (!yl::NRef<yl::Dictionary>(doc).contains("email")) {
    std::cerr << "  'email' key not found — skipping.";
  }
}

// ---------------------------------------------------------------------------
// Helper: demonstrate type guard with isA<T> before NRef<T>.
// ---------------------------------------------------------------------------
static void safeTypeAccess() {
  yl::YAML yaml;
  yl::BufferSource source{"---\n"
                          "count: 42\n"
                          "name: demo\n"
                          "active: true\n"};
  yaml.parse(source);

  const auto &doc = yaml.document(0);

  // Safely read a Number
  if (yl::isA<yl::Number>(doc["count"])) {
    std::cout << "  count = "
              << yl::NRef<yl::Number>(doc["count"]).value<int>();
  }

  // Safely read a String
  if (yl::isA<yl::String>(doc["name"])) {
    std::cout << "  name = " << yl::NRef<yl::String>(doc["name"]).value();
  }

  // Safely read a Boolean
  if (yl::isA<yl::Boolean>(doc["active"])) {
    std::cout << "  active = "
              << (yl::NRef<yl::Boolean>(doc["active"]).value() ? "yes" : "no");
  }
}

// ---------------------------------------------------------------------------
// Helper: demonstrate handling unsupported YAML version directive.
// ---------------------------------------------------------------------------
static void badYAMLVersion() {
  std::cout << "--- Unsupported %YAML major version:";
  tryParse("YAML 2.0 document", "%YAML 2.0\n---\nvalue: 42\n");
}

// ---------------------------------------------------------------------------
// Helper: demonstrate error from undefined alias.
// ---------------------------------------------------------------------------
static void undefinedAlias() {
  std::cout << "--- Undefined alias reference:";
  tryParse("*missing_anchor", "---\nkey: *missing_anchor\n");
}

// ---------------------------------------------------------------------------
// Helper: demonstrate error from duplicate key.
// ---------------------------------------------------------------------------
static void duplicateKey() {
  std::cout << "--- Duplicate dictionary key:";
  tryParse("duplicate key block", "---\nfoo: 1\nbar: 2\nfoo: 3\n");
}

// ---------------------------------------------------------------------------
// Helper: demonstrate error from tab indentation.
// ---------------------------------------------------------------------------
static void tabIndentation() {
  std::cout << "--- Tab used in block indentation:";
  tryParse("tab indent", "---\nparent:\n\tchild: value\n");
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  try {
        std::cout << "YAML_Error_Handling_Demo started ...";
    std::cout << yl::YAML::version();

    // 1. Safe parse with error reporting
    std::cout << "=== Parse error demonstrations ===";
    badYAMLVersion();
    undefinedAlias();
    duplicateKey();
    tabIndentation();

    // 2. Valid parse: safe key and type guards
    std::cout << "=== Safe access demonstrations ===";
    safeKeyAccess();
    safeTypeAccess();

    // 3. Validate against expected structure
    std::cout << "=== Structure validation ===";
    {
      yl::YAML yaml;
      yl::BufferSource source{"---\n"
                              "server:\n"
                              "  host: localhost\n"
                              "  port: 8080\n"};
      yaml.parse(source);
      const auto &doc = yaml.document(0);
      if (!yl::isA<yl::Dictionary>(doc)) {
        throw std::runtime_error("Expected top-level mapping.");
      }
      if (!yl::NRef<yl::Dictionary>(doc).contains("server")) {
        throw std::runtime_error("Missing 'server' key.");
      }
      if (!yl::isA<yl::Dictionary>(doc["server"])) {
        throw std::runtime_error("'server' must be a mapping.");
      }
      const int port = yl::NRef<yl::Number>(doc["server"]["port"]).value<int>();
      std::cout << "  Server port validated: " << port;
    }

  } catch (const std::exception &ex) {
    std::cerr << "Fatal error: " << ex.what();
  }
  std::cout << "YAML_Error_Handling_Demo exited.";
  exit(EXIT_SUCCESS);
}
