#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check Node reference functions work.", "[YAML][Node][Reference]")
{
    const YAML yaml;
    SECTION("Number reference.", "[YAML][Node][Reference][Number]")
    {
        BufferSource yamlSource{ "45500" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<Number>(yaml.document(0)).value<int>() == 45500);
    }
    SECTION("String reference.", "[YAML][Node][Reference][String]")
    {
        BufferSource yamlSource{ R"("0123456789")" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<String>(yaml.document(0)).value() == "0123456789");
    }
    SECTION("Boolean reference.", "[YAML][Node][Reference][Boolean]")
    {
        BufferSource yamlSource{ R"(True)" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<Boolean>(yaml.document(0)).value() == true);
    }
    SECTION("Null reference.", "[YAML][Node][Reference][Null]")
    {
        BufferSource yamlSource{ R"(null)" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<Null>(yaml.document(0)).value() == nullptr);
    }
    SECTION("Array reference.", "[YAML][Node][Reference][Array]")
    {
        BufferSource yamlSource{ R"([777,9000,"apples"])" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<Array>(yaml.document(0)).size() == 3);
        REQUIRE(YRef<String>((yaml.document(0))[2]).value() == "apples");
    }
    SECTION("Dictionary reference.", "[YAML][Node][Reference][Dictionary]")
    {
        BufferSource yamlSource{ R"({"City":"Southampton","Population":500000 })" };
        yaml.parse(yamlSource);
        REQUIRE(YRef<Dictionary>(yaml.document(0)).size() == 2);
        REQUIRE(YRef<String>((yaml.document(0))["City"]).value() == "Southampton");
    }
}

TEST_CASE("Check Node reference function exceptions.", "[YAML][Node][Reference][Exception]")
{
    const YAML yaml;
    SECTION("String reference an number.", "[YAML][Node][Reference][String]")
    {
        BufferSource yamlSource{ "45500" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(YRef<String>(yaml.document(0)).value(), Node::Error);
        REQUIRE_THROWS_WITH(YRef<String>(yaml.document(0)).value(), "Node Error: Node not a string.");
    }
    SECTION("Number reference a string.", "[YAML][Node][Reference][Number][Exception]")
    {
        BufferSource yamlSource{ R"("0123456789")" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(YRef<Number>(yaml.document(0)).is<int>(), Node::Error);
        REQUIRE_THROWS_WITH(YRef<Number>(yaml.document(0)).is<int>(), "Node Error: Node not a number.");
    }
    SECTION("Null reference a boolean.", "[YAML][Node][Reference][Null][Exception]")
    {
        BufferSource yamlSource{ R"(true)" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(YRef<Null>(yaml.document(0)).value(), Node::Error);
        REQUIRE_THROWS_WITH(YRef<Null>(yaml.document(0)).value(), "Node Error: Node not a null.");
    }
    SECTION("Boolean reference a null.", "[YAML][Node][Reference][Boolean][Exception]")
    {
        BufferSource yamlSource{ R"(null)" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(YRef<Boolean>(yaml.document(0)).value(), Node::Error);
        REQUIRE_THROWS_WITH(YRef<Boolean>(yaml.document(0)).value(), "Node Error: Node not a boolean.");
    }
    SECTION("Dictionary reference an array.", "[YAML][Node][Reference][Dictionary][Exception]")
    {
        BufferSource yamlSource{ R"([777,9000,"apples"])" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(YRef<Dictionary>(yaml.document(0)).size(), Node::Error);
        REQUIRE_THROWS_WITH(YRef<Dictionary>(yaml.document(0)).size(), "Node Error: Node not an dictionary.");
    }
    SECTION("Array reference an object.", "[YAML][Node][Reference][Array][Exception]")
    {
        BufferSource yamlSource{ R"({"City":"Southampton","Population":500000 })" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(YRef<Array>(yaml.document(0)).size(), Node::Error);
        REQUIRE_THROWS_WITH(YRef<Array>(yaml.document(0)).size(), "Node Error: Node not an array.");
    }
}
