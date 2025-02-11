#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check use of YNode indexing operators.", "[YAML][YNode][Index]")
{
    const YAML yaml;
    SECTION("Parse dictionary and check its components using indexing.", "[YAML][YNode][Index]")
    {
        BufferSource yamlSource{ R"({"City":"Southampton","Population":500000})" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<String>((yaml.document(0))["City"]).value() == "Southampton");
        REQUIRE(YRef<Number>((yaml.document(0))["Population"]).value<int>() == 500000);
    }
    SECTION("Parse array and check its components using indexing.", "[YAML][YNode][Index]")
    {
        BufferSource yamlSource{ R"([777,9000,"apples"])" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<Number>((yaml.document(0))[0]).value<int>() == 777);
        REQUIRE(YRef<Number>((yaml.document(0))[1]).value<int>() == 9000);
        REQUIRE(YRef<String>((yaml.document(0))[2]).value() == "apples");
    }
    SECTION("Parse array with embedded dictionary and check its components using indexing.", "[YAML][YNode][Index]")
    {
        BufferSource yamlSource{ R"([777,{"City":"Southampton","Population":500000},"apples"])" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<Number>((yaml.document(0))[0]).value<int>() == 777);
        REQUIRE(YRef<String>((yaml.document(0))[1]["City"]).value() == "Southampton");
        REQUIRE(YRef<Number>((yaml.document(0))[1]["Population"]).value<int>() == 500000);
        REQUIRE(YRef<String>((yaml.document(0))[2]).value() == "apples");
    }
    SECTION("Parse dictionary and check an invalid key generates exception.", "[YAML][YNode][Index]")
    {
        BufferSource yamlSource{ R"({"City":"Southampton","Population":500000})" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(isA<Dictionary>(yaml.document(0)["Cityy"]), Dictionary::Error);
        REQUIRE_THROWS_WITH(isA<Dictionary>(yaml.document(0)["Cityy"]), "Dictionary Error: Invalid key used to access dictionary.");
    }
    SECTION("Parse array and check an invalid index generates exception.", "[YAML][YNode][Index]")
    {
        BufferSource yamlSource{ R"([777,9000,"apples"])" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(isA<Array>(yaml.document(0)[3]), YNode::Error);
        REQUIRE_THROWS_WITH(isA<Array>(yaml.document(0)[3]), "YNode Error: Invalid index used to access array.");
    }
}
