#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML Parse start document.", "[YAML][Parse][Start]") {
  const YAML yaml;
  SECTION("YAML Parse start document.", "[YAML][Parse][Start]") {
    BufferSource source {"---\n"};
    REQUIRE_NOTHROW(yaml.parse(source));
  }
}