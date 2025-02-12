
#include "YAML_Lib_Tests.hpp"

// ====================================
// YNode Number floating point notation
// ====================================
TEST_CASE("Check YNode Number floating point notation.", "[YAML][YNode][Number][Float][Precision]")
{
    const YAML yaml;
    SECTION("Floating point notation to normal.", "[YAML][YNode][Number][Float][Notation]")
    {
        std::string expected{ R"({"latitude":39.068341,"longitude":-70.741615})" };
        BufferSource yamlSource{ expected };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml["latitude"]).is<float>());
        REQUIRE_FALSE(!YRef<Number>(yaml["longitude"]).is<float>());
        BufferDestination yamlDestination;
        Number::setNotation(Number::numberNotation::normal);
        Number::setPrecision(6);
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "---\n\"latitude\": 39.0683\n\"longitude\": -70.7416\n...\n");
    }
    SECTION("Floating point notation to fixed.", "[YAML][YNode][Number][Float][Notation]")
    {
        std::string expected{ R"({"latitude":39.068341,"longitude":-70.741615})" };
        BufferSource yamlSource{ expected };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml["latitude"]).is<float>());
        REQUIRE_FALSE(!YRef<Number>(yaml["longitude"]).is<float>());
        BufferDestination yamlDestination;
        Number::setNotation(Number::numberNotation::fixed);
        yamlDestination.clear();
        yaml.stringify(yamlDestination);
        Number::setNotation(Number::numberNotation::normal);
        REQUIRE(yamlDestination.toString() == "---\n\"latitude\": 39.068340\n\"longitude\": -70.741615\n...\n");
    }
    SECTION("Floating point notation to scientific.", "[YAML][YNode][Number][Float][Notation]")
    {
        std::string expected{ R"({"latitude":39.068341,"longitude":-70.741615})" };
        BufferSource yamlSource{ expected };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml["latitude"]).is<float>());
        REQUIRE_FALSE(!YRef<Number>(yaml["longitude"]).is<float>());
        BufferDestination yamlDestination;
        Number::setNotation(Number::numberNotation::scientific);
        yamlDestination.clear();
        yaml.stringify(yamlDestination);
        Number::setNotation(Number::numberNotation::normal);
        REQUIRE(yamlDestination.toString() == "---\n\"latitude\": 3.906834e+01\n\"longitude\": -7.074162e+01\n...\n");
    }
}
