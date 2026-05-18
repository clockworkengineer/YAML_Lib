#include "YAML_Lib_Tests.hpp"

#include <vector>

TEST_CASE("Parser rejects alias expansion resource attacks", "[YAML][Security][AliasLimit]") {
  ::YAML_Lib::Options options;
  options.max_alias_expansions = 4;
  options.max_parse_depth = 64;
  ::YAML_Lib::YAML yaml(options);

  const std::string maliciousYaml = R"(---
a: &a { foo: 1 }
root: [*a, *a, *a, *a, *a]
)";

  REQUIRE_THROWS_WITH(yaml.parse(BufferSource{maliciousYaml}), Catch::Matchers::ContainsSubstring("YAML alias expansion limit exceeded"));
}

TEST_CASE("Parser rejects large alias expansion graphs", "[YAML][Security][AliasLimit]") {
  ::YAML_Lib::Options options;
  options.max_alias_expansions = 8;
  options.max_parse_depth = 64;
  ::YAML_Lib::YAML yaml(options);

  const std::string maliciousYaml = R"(---
a: &a { foo: 1 }
root:
  - *a
  - *a
  - *a
  - *a
  - *a
  - *a
  - *a
  - *a
  - *a
)";

  REQUIRE_THROWS_WITH(yaml.parse(BufferSource{maliciousYaml}), Catch::Matchers::ContainsSubstring("YAML alias expansion limit exceeded"));
}

TEST_CASE("Parser rejects deeply nested YAML before stack exhaustion", "[YAML][Security][DepthLimit]") {
  ::YAML_Lib::Options options;
  options.max_parse_depth = 4;
  options.max_alias_expansions = 64;
  ::YAML_Lib::YAML yaml(options);

  std::string deepYaml = "---\n";
  for (int depth = 0; depth < 12; ++depth) {
    deepYaml += std::string(depth * 2, ' ') + "level" + std::to_string(depth) + ":\n";
  }
  deepYaml += std::string(12 * 2, ' ') + "value: nested\n";

  REQUIRE_THROWS_WITH(yaml.parse(BufferSource{deepYaml}), Catch::Matchers::ContainsSubstring("nesting depth limit"));
}

TEST_CASE("Parser rejects too many documents in untrusted input", "[YAML][Security][DocumentLimit]") {
  ::YAML_Lib::Options options;
  options.max_documents = 1;
  options.max_parse_depth = 64;
  options.max_alias_expansions = 64;
  ::YAML_Lib::YAML yaml(options);

  const std::string multiDocYaml = R"(---
value: first
---
value: second
)";

  REQUIRE_THROWS_WITH(yaml.parse(BufferSource{multiDocYaml}), Catch::Matchers::ContainsSubstring("document count exceeds configured limit"));
}

TEST_CASE("Parser handles malformed inputs robustly without crashing", "[YAML][Security][Fuzz]") {
  ::YAML_Lib::Options options;
  options.max_alias_expansions = 16;
  options.max_parse_depth = 64;
  ::YAML_Lib::YAML yaml(options);

  const std::vector<std::string> fuzzCases = {
    "---\n- [*x]\n",
    "---\na: &a [*a]\n",
    "---\n? [*a\n",
    "a: [1, 2, 3\n  - 4\n",
    "a: &a [\"LOL\"]\nb: [*a,*a]\nc: [*b,*b]\n",
    "a: &a \"value\"\nb: *a\nc: *b\n",
    "---\n- foo\n- bar\n- baz\n  - qux\n",
    "a: |\n  unclosed block string"
  };

  for (const auto &input : fuzzCases) {
    try {
      yaml.parse(BufferSource{input});
    } catch (...) {
      // The parser may throw on malformed data, but must not crash.
    }
  }
}
