#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YAML creation.", "[YAML][Create]") {
  SECTION("Create YAML.",
          "[YAML][Create][Constructor]") {
    REQUIRE_NOTHROW(YAML());
  }
}