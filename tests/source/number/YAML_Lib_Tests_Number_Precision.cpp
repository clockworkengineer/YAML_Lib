#include "YAML_Lib_Tests.hpp"

// =====================================
// JNode Number floating point precision
// =====================================
TEST_CASE("Check JNode Number floating point precision.",
          "[YAML][JNode][Number][Float][Precision]") {
  const YAML yaml;
  SECTION("Floating point precision to 6 (default).",
          "[YAML][JNode][Number][Float][Precision]") {
    std::string expected{R"({"latitude":39.068341,"longitude":-70.741615})"};
    BufferSource yamlSource{expected};
    yaml.parse(yamlSource);
    REQUIRE_FALSE(!NRef<Number>(yaml["latitude"]).is<float>());
    REQUIRE_FALSE(!NRef<Number>(yaml["longitude"]).is<float>());
    BufferDestination yamlDestination;
    Number::setPrecision(6);
    yaml.stringify(yamlDestination);
    REQUIRE(yamlDestination.toString() ==
            "---\n\"latitude\": 39.0683\n\"longitude\": -70.7416\n...\n");
  }
  SECTION("Floating point precision to 7.",
          "[YAML][JNode][Number][Float][Precision]") {
    std::string expected{R"({"latitude":39.068341,"longitude":-70.741615})"};
    BufferSource yamlSource{expected};
    yaml.parse(yamlSource);
    REQUIRE_FALSE(!NRef<Number>(yaml["latitude"]).is<float>());
    REQUIRE_FALSE(!NRef<Number>(yaml["longitude"]).is<float>());
    BufferDestination yamlDestination;
    Number::setPrecision(7);
    yaml.stringify(yamlDestination);
    Number::setPrecision(6);
    REQUIRE(yamlDestination.toString() ==
            "---\n\"latitude\": 39.06834\n\"longitude\": -70.74162\n...\n");
  }
  SECTION("Floating point precision to 8.",
          "[YAML][JNode][Number][Float][Precision]") {
    std::string expected{R"({"latitude":39.068341,"longitude":-70.741615})"};
    BufferSource yamlSource{expected};
    yaml.parse(yamlSource);
    REQUIRE_FALSE(!NRef<Number>(yaml["latitude"]).is<float>());
    REQUIRE_FALSE(!NRef<Number>(yaml["longitude"]).is<float>());
    BufferDestination yamlDestination;
    Number::setPrecision(8);
    yaml.stringify(yamlDestination);
    Number::setPrecision(6);
    REQUIRE(yamlDestination.toString() ==
            "---\n\"latitude\": 39.06834\n\"longitude\": -70.741615\n...\n");
  }
//   SECTION("Floating point precision to maximum.",
//           "[YAML][JNode][Number][Float][Precision]") {
//     std::string expected{R"({"latitude":39.068341,"longitude":-70.741615})"};
//     BufferSource yamlSource{expected};
//     yaml.parse(yamlSource);
//     REQUIRE_FALSE(!NRef<Number>(yaml["latitude"]).is<float>());
//     REQUIRE_FALSE(!NRef<Number>(yaml["longitude"]).is<float>());
//     BufferDestination yamlDestination;
//     Number::setPrecision(std::numeric_limits<long double>::digits10 + 1);
//     yaml.stringify(yamlDestination);
//     Number::setPrecision(6);
//     if constexpr (std::numeric_limits<long double>::digits10 + 1 == 16) {
//       REQUIRE(yamlDestination.toString() ==
//               "---\n\"latitude\": 39.06834030151367188\n\"longitude\": "
//               "-70.74161529541015625\n...\n");
//     } else if constexpr (std::numeric_limits<long double>::digits10 + 1 == 19) {
//       REQUIRE(yamlDestination.toString() ==
//               "---\n\"latitude\": 39.06834030151367188\n\"longitude\": "
//               "-70.74161529541015625\n...\n");
//     } else if constexpr (std::numeric_limits<long double>::digits10 + 1 == 34) {
//       REQUIRE(yamlDestination.toString() ==
//               "---\n\"latitude\": 39.06834030151367187\n\"longitude\": "
//               "-70.74161529541015625\n...\n");
//     } else {
//       REQUIRE_FALSE(true);
//     }
//   }
}
