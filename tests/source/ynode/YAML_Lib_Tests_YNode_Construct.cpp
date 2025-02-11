#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check use of YNode constructors.", "[YAML][YNode][Constructor]")
{
    SECTION("Construct YNode(integer).", "[YAML][YNode][Constructor][Integer]")
    {
        YNode jNode(999);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!YRef<Number>(jNode).is<int>());
        REQUIRE(YRef<Number>(jNode).value<int>() == 999);
    }
    SECTION("Construct YNode(long).", "[YAML][YNode][Constructor][Long]")
    {
        YNode jNode(99988899l);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!YRef<Number>(jNode).is<long>());
        REQUIRE(YRef<Number>(jNode).value<int>() == 99988899l);
    }
    SECTION("Construct YNode(long long).", "[YAML][YNode][Constructor][Long Long]")
    {
        YNode jNode(99988899ll);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!YRef<Number>(jNode).is<long long>());
        REQUIRE(YRef<Number>(jNode).value<long long>() == 99988899ll);
    }
    SECTION("Construct YNode(float).", "[YAML][YNode][Constructor][Float]")
    {
        YNode jNode(777.999f);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!YRef<Number>(jNode).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(jNode).value<float>(), 777.999f, 0.0001));
    }
    SECTION("Construct YNode(double).", "[YAML][YNode][Constructor][Double]")
    {
        YNode jNode(66666.8888);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!YRef<Number>(jNode).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(jNode).value<double>(), 66666.8888, 0.0001));
    }
    SECTION("Construct YNode(long double).", "[YAML][YNode][Constructor][Long Double]")
    {
        YNode jNode(66666.8888l);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!YRef<Number>(jNode).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(jNode).value<long double>(), 66666.8888l, 0.0001));
    }
    SECTION("Construct YNode(C string).", "[YAML][YNode][Constructor][CString]")
    {
        YNode jNode("test string");
        REQUIRE_FALSE(!isA<String>(jNode));
        REQUIRE(YRef<String>(jNode).value() == "test string");
    }
    SECTION("Construct YNode(string).", "[YAML][YNode][Constructor][String]")
    {
        YNode jNode("test string");
        REQUIRE_FALSE(!isA<String>(jNode));
        REQUIRE(YRef<String>(jNode).value() == "test string");
    }
    SECTION("Construct YNode(boolean).", "[YAML][YNode][Constructor][Boolean]")
    {
        YNode jNode(false);
        REQUIRE_FALSE(!isA<Boolean>(jNode));
        REQUIRE_FALSE(YRef<Boolean>(jNode).value());
    }
    SECTION("Construct YNode(null).", "[YAML][YNode][Constructor][Null]")
    {
        YNode jNode(nullptr);
        REQUIRE_FALSE(!isA<Null>(jNode));
        REQUIRE(YRef<Null>(jNode).value() == nullptr);
    }
    SECTION("Construct YNode(array).", "[YAML][YNode][Constructor][Array]")
    {
        YNode jNode{ 1, 2, 3, 4 };
        REQUIRE_FALSE(!isA<Array>(jNode));
        auto &array = YRef<Array>(jNode).value();
        REQUIRE(YRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(YRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(YRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(YRef<Number>(array[3]).value<int>() == 4);
    }
    SECTION("Construct YNode(object).", "[YAML][YNode][Constructor][Dictionary]")
    {
        YNode jNode{ { "key1", 55 }, { "key2", 26666 } };
        REQUIRE_FALSE(!isA<Dictionary>(jNode));
        auto &object = YRef<Dictionary>(jNode);
        REQUIRE(YRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(YRef<Number>(object["key2"]).value<int>() == 26666);
    }
    SECTION("Construct YNode(array with nested array).", "[YAML][YNode][Constructor][Array]")
    {
        YNode jNode{ 1, 2, 3, 4, YNode{ 5, 6, 7, 8 } };
        REQUIRE_FALSE(!isA<Array>(jNode));
        auto &array = YRef<Array>(jNode).value();
        REQUIRE(YRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(YRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(YRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(YRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(YRef<Number>(array[4][0]).value<int>() == 5);
        REQUIRE(YRef<Number>(array[4][1]).value<int>() == 6);
        REQUIRE(YRef<Number>(array[4][2]).value<int>() == 7);
        REQUIRE(YRef<Number>(array[4][3]).value<int>() == 8);
    }
    SECTION("Construct YNode(object with nested object).", "[YAML][YNode][Constructor][Dictionary]")
    {
        YNode jNode{ { "key1", 55 }, { "key2", 26666 }, { "key3", YNode{ { "key4", 5555 }, { "key5", 7777 } } } };
        REQUIRE_FALSE(!isA<Dictionary>(jNode));
        auto &object = YRef<Dictionary>(jNode);
        REQUIRE(YRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(YRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(YRef<Number>(object["key3"]["key4"]).value<int>() == 5555);
        REQUIRE(YRef<Number>(object["key3"]["key5"]).value<int>() == 7777);
    }
    SECTION("Construct YNode(array with nested object).", "[YAML][YNode][Constructor][Array]")
    {
        YNode jNode{ 1, 2, 3, 4, YNode{ { "key4", 5555 }, { "key5", 7777 } } };
        REQUIRE_FALSE(!isA<Array>(jNode));
        auto &array = YRef<Array>(jNode).value();
        REQUIRE(YRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(YRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(YRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(YRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(YRef<Number>(array[4]["key4"]).value<int>() == 5555);
        REQUIRE(YRef<Number>(array[4]["key5"]).value<int>() == 7777);
    }
    SECTION("Construct YNode(object with nested array).", "[YAML][YNode][Constructor][Dictionary]")
    {
        YNode jNode{ { "key1", 55 }, { "key2", 26666 }, { "key3", YNode{ 5, 6, 7, 8 } } };
        REQUIRE_FALSE(!isA<Dictionary>(jNode));
        auto &object = YRef<Dictionary>(jNode);
        REQUIRE(YRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(YRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(YRef<Number>(object["key3"][0]).value<int>() == 5);
        REQUIRE(YRef<Number>(object["key3"][1]).value<int>() == 6);
        REQUIRE(YRef<Number>(object["key3"][2]).value<int>() == 7);
        REQUIRE(YRef<Number>(object["key3"][3]).value<int>() == 8);
    }
}
