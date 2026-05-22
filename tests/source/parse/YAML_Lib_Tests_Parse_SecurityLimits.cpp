#include "YAML_Lib_Tests.hpp"

using namespace YAML_Lib;

TEST_CASE("Parser honors max_documents in parse()", "[YAML][Parse][Security][max_documents]") {
  Options options;
  options.max_documents = 1;

  YAML yaml(options);
  BufferSource src("---\nfoo: 1\n---\nbar: 2\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}

TEST_CASE("Parser honors max_parse_depth in nested structures", "[YAML][Parse][Security][max_parse_depth]") {
  Options options;
  options.max_parse_depth = 2;

  YAML yaml(options);
  BufferSource src("---\nroot: [ [ [1] ] ]\n");

  REQUIRE_THROWS_AS(yaml.parse(src), IParser::Error);
}

TEST_CASE("Parser honors max_alias_expansions for anchor chains", "[YAML][Parse][Security][max_alias_expansions]") {
  Options options;
  options.max_alias_expansions = 2;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "c: &a3 { foo: bar }\n"
      "b: &a2 { foo: *a3 }\n"
      "a: &a1 { foo: *a2 }\n"
      "root: *a1\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}

TEST_CASE("Parser rejects recursive alias loops", "[YAML][Parse][Security][alias_loop]") {
  Options options;
  options.max_alias_expansions = 100;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "a: &a1 { foo: *a2 }\n"
      "b: &a2 { foo: *a1 }\n"
      "root: *a1\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}

TEST_CASE("Parser rejects exponential alias expansion graphs", "[YAML][Parse][Security][alias_exponential]") {
  Options options;
  options.max_alias_expansions = 4;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "a1: &a1 [1, 1]\n"
      "a2: &a2 [*a1, *a1]\n"
      "a3: &a3 [*a2, *a2]\n"
      "root: *a3\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}

TEST_CASE("Parser honors repeated alias expansions up to the limit", "[YAML][Parse][Security][alias_repeat]") {
  Options options;
  options.max_alias_expansions = 3;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "a: &a { foo: bar }\n"
      "root: [*a, *a, *a]\n");

  REQUIRE_NOTHROW(yaml.parse(src));
}

TEST_CASE("Parser rejects repeated alias expansions beyond the limit", "[YAML][Parse][Security][alias_repeat]") {
  Options options;
  options.max_alias_expansions = 2;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "a: &a { foo: bar }\n"
      "root: [*a, *a, *a]\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}

TEST_CASE("Parser rejects alias-based denial-of-service in flow context", "[YAML][Parse][Security][alias_dos_flow]") {
  Options options;
  options.max_alias_expansions = 3;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "a: &a [1, 2]\n"
      "root: [*a, *a, *a, *a]\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}

TEST_CASE("Parser rejects alias-based denial-of-service in block context", "[YAML][Parse][Security][alias_dos_block]") {
  Options options;
  options.max_alias_expansions = 2;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "a: &a\n"
      "  - 1\n"
      "  - 2\n"
      "root:\n"
      "  - *a\n"
      "  - *a\n"
      "  - *a\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}

TEST_CASE("Parser rejects alias table size limit", "[YAML][Parse][Security][alias_table_limit]") {
  Options options;
  options.max_aliases = 2;

  YAML yaml(options);
  BufferSource src(
      "---\n"
      "a: &a 1\n"
      "b: &b 2\n"
      "c: &c 3\n"
      "root: *a\n");

  REQUIRE_THROWS_AS(yaml.parse(src), SyntaxError);
}
