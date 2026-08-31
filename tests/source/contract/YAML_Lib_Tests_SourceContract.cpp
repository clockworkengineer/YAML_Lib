#include "YAML_Lib_Tests.hpp"
#include "implementation/io/YAML_Sources.hpp"

TEST_CASE("ISource contract testing across implementations.", "[YAML][Contract][ISource]") {
  SECTION("BufferSource contract adherence") {
    BufferSource source("key: value");
    REQUIRE(source.more());
    REQUIRE(source.current() == 'k');
    REQUIRE(source.position() == 0);

    source.next();
    REQUIRE(source.current() == 'e');
    REQUIRE(source.position() == 1);

    source.reset();
    REQUIRE(source.current() == 'k');
    REQUIRE(source.position() == 0);
  }

  SECTION("SpanSource contract adherence") {
    const std::string text = "abc";
    SpanSource source(std::span<const char>(text.data(), text.size()));
    REQUIRE(source.more());
    REQUIRE(source.current() == 'a');
    source.next();
    source.next();
    source.next();
    REQUIRE_FALSE(source.more());
  }
}
