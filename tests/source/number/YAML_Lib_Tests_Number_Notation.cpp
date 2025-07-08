
#include "YAML_Lib_Tests.hpp"

// ====================================
// Node Number floating point notation
// ====================================
TEST_CASE("Check Node Number floating point notation.", "[YAML][Node][Number][Float][Precision]")
{
    const YAML yaml;
    SECTION("Floating point notation to normal.", "[YAML][Node][Number][Float][Notation]")
    {
        std::string expected{ R"({"latitude":39.068341,"longitude":-70.741615})" };
        BufferSource yamlSource{ expected };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!NRef<Number>(yaml["latitude"]).is<float>());
        REQUIRE_FALSE(!NRef<Number>(yaml["longitude"]).is<float>());
        BufferDestination yamlDestination;
        Number::setNotation(Number::numberNotation::normal);
        Number::setPrecision(6);
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "---\n\"latitude\": 39.0683\n\"longitude\": -70.7416\n...\n");
    }
    SECTION("Floating point notation to fixed.", "[YAML][Node][Number][Float][Notation]")
    {
        std::string expected{ R"({"latitude":39.068341,"longitude":-70.741615})" };
        BufferSource yamlSource{ expected };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!NRef<Number>(yaml["latitude"]).is<float>());
        REQUIRE_FALSE(!NRef<Number>(yaml["longitude"]).is<float>());
        BufferDestination yamlDestination;
        Number::setNotation(Number::numberNotation::fixed);
        yamlDestination.clear();
        yaml.stringify(yamlDestination);
        Number::setNotation(Number::numberNotation::normal);
        REQUIRE(yamlDestination.toString() == "---\n\"latitude\": 39.068340\n\"longitude\": -70.741615\n...\n");
    }
    SECTION("Floating point notation to scientific.", "[YAML][Node][Number][Float][Notation]")
    {
        std::string expected{ R"({"latitude":39.068341,"longitude":-70.741615})" };
        BufferSource yamlSource{ expected };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!NRef<Number>(yaml["latitude"]).is<float>());
        REQUIRE_FALSE(!NRef<Number>(yaml["longitude"]).is<float>());
        BufferDestination yamlDestination;
        Number::setNotation(Number::numberNotation::scientific);
        yamlDestination.clear();
        yaml.stringify(yamlDestination);
        Number::setNotation(Number::numberNotation::normal);
        REQUIRE(yamlDestination.toString() == "---\n\"latitude\": 3.906834e+01\n\"longitude\": -7.074162e+01\n...\n");
    }
}
