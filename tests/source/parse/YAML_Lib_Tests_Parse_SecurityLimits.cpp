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
