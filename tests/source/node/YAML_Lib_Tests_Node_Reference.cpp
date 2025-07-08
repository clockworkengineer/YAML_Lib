#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check Node reference functions work.", "[YAML][Node][Reference]")
{
    const YAML yaml;
    SECTION("Number reference.", "[YAML][Node][Reference][Number]")
    {
        BufferSource yamlSource{ "45500" };
        yaml.parse(yamlSource);
        REQUIRE(NRef<Number>(yaml.document(0)).value<int>() == 45500);
    }
    SECTION("String reference.", "[YAML][Node][Reference][String]")
    {
        BufferSource yamlSource{ R"("0123456789")" };
        yaml.parse(yamlSource);
        REQUIRE(NRef<String>(yaml.document(0)).value() == "0123456789");
    }
    SECTION("Boolean reference.", "[YAML][Node][Reference][Boolean]")
    {
        BufferSource yamlSource{ R"(True)" };
        yaml.parse(yamlSource);
        REQUIRE(NRef<Boolean>(yaml.document(0)).value() == true);
    }
    SECTION("Null reference.", "[YAML][Node][Reference][Null]")
    {
        BufferSource yamlSource{ R"(null)" };
        yaml.parse(yamlSource);
        REQUIRE(NRef<Null>(yaml.document(0)).value() == nullptr);
    }
    SECTION("Array reference.", "[YAML][Node][Reference][Array]")
    {
        BufferSource yamlSource{ R"([777,9000,"apples"])" };
        yaml.parse(yamlSource);
        REQUIRE(NRef<Array>(yaml.document(0)).size() == 3);
        REQUIRE(NRef<String>((yaml.document(0))[2]).value() == "apples");
    }
    SECTION("Dictionary reference.", "[YAML][Node][Reference][Dictionary]")
    {
        BufferSource yamlSource{ R"({"City":"Southampton","Population":500000 })" };
        yaml.parse(yamlSource);
        REQUIRE(NRef<Dictionary>(yaml.document(0)).size() == 2);
        REQUIRE(NRef<String>((yaml.document(0))["City"]).value() == "Southampton");
    }
}

TEST_CASE("Check Node reference function exceptions.", "[YAML][Node][Reference][Exception]")
{
    const YAML yaml;
    SECTION("String reference an number.", "[YAML][Node][Reference][String]")
    {
        BufferSource yamlSource{ "45500" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(NRef<String>(yaml.document(0)).value(), Node::Error);
        REQUIRE_THROWS_WITH(NRef<String>(yaml.document(0)).value(), "Node Error: Node not a string.");
    }
    SECTION("Number reference a string.", "[YAML][Node][Reference][Number][Exception]")
    {
        BufferSource yamlSource{ R"("0123456789")" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(NRef<Number>(yaml.document(0)).is<int>(), Node::Error);
        REQUIRE_THROWS_WITH(NRef<Number>(yaml.document(0)).is<int>(), "Node Error: Node not a number.");
    }
    SECTION("Null reference a boolean.", "[YAML][Node][Reference][Null][Exception]")
    {
        BufferSource yamlSource{ R"(true)" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(NRef<Null>(yaml.document(0)).value(), Node::Error);
        REQUIRE_THROWS_WITH(NRef<Null>(yaml.document(0)).value(), "Node Error: Node not a null.");
    }
    SECTION("Boolean reference a null.", "[YAML][Node][Reference][Boolean][Exception]")
    {
        BufferSource yamlSource{ R"(null)" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(NRef<Boolean>(yaml.document(0)).value(), Node::Error);
        REQUIRE_THROWS_WITH(NRef<Boolean>(yaml.document(0)).value(), "Node Error: Node not a boolean.");
    }
    SECTION("Dictionary reference an array.", "[YAML][Node][Reference][Dictionary][Exception]")
    {
        BufferSource yamlSource{ R"([777,9000,"apples"])" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(NRef<Dictionary>(yaml.document(0)).size(), Node::Error);
        REQUIRE_THROWS_WITH(NRef<Dictionary>(yaml.document(0)).size(), "Node Error: Node not an dictionary.");
    }
    SECTION("Array reference an object.", "[YAML][Node][Reference][Array][Exception]")
    {
        BufferSource yamlSource{ R"({"City":"Southampton","Population":500000 })" };
        yaml.parse(yamlSource);
        REQUIRE_THROWS_AS(NRef<Array>(yaml.document(0)).size(), Node::Error);
        REQUIRE_THROWS_WITH(NRef<Array>(yaml.document(0)).size(), "Node Error: Node not an array.");
    }
}
