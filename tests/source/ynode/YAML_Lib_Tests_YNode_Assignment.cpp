#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check use of YNode assigment operators.", "[YAML][YNode][Assignment]")
{
    YNode yNode;
    SECTION("Assign integer to YNode.", "[YAML][YNode][Assignment][Integer]")
    {
        yNode = 999;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!YRef<Number>(yNode).is<int>());
        REQUIRE(YRef<Number>(yNode).value<int>() == 999);
    }
    SECTION("Assign long to YNode.", "[YAML][YNode][Assignment][Long]")
    {
        yNode = 99988899l;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!YRef<Number>(yNode).is<long>());
        REQUIRE(YRef<Number>(yNode).value<int>() == 99988899l);
    }
    SECTION("Assign long long to YNode.", "[YAML][YNode][Assignment][Long Long]")
    {
        yNode = 99988899ll;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!YRef<Number>(yNode).is<long long>());
        REQUIRE(YRef<Number>(yNode).value<long long>() == 99988899ll);
    }
    SECTION("Assign float to YNode.", "[YAML][YNode][Assignment][Float]")
    {
        yNode = 777.999f;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!YRef<Number>(yNode).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yNode).value<float>(), 777.999f, 0.0001));
    }
    SECTION("Assign double to YNode.", "[YAML][YNode][Assignment][Double]")
    {
        yNode = 66666.8888;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!YRef<Number>(yNode).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yNode).value<double>(), 66666.8888, 0.0001));
    }
    SECTION("Assign long double to YNode.", "[YAML][YNode][Assignment][Long Double]")
    {
        yNode = 66666.8888l;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!YRef<Number>(yNode).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yNode).value<long double>(), 66666.8888l, 0.0001));
    }
    SECTION("Assign C string to YNode.", "[YAML][YNode][Assignment][CString]")
    {
        yNode = "test string";
        REQUIRE_FALSE(!isA<String>(yNode));
        REQUIRE(YRef<String>(yNode).value() == "test string");
    }
    SECTION("Assign string to YNode.", "[YAML][YNode][Assignment][String]")
    {
        yNode = std::string("test string");
        REQUIRE_FALSE(!isA<String>(yNode));
        REQUIRE(YRef<String>(yNode).value() == "test string");
    }
    SECTION("Assign boolean to YNode.", "[YAML][YNode][Assignment][Boolean]")
    {
        yNode = false;
        REQUIRE_FALSE(!isA<Boolean>(yNode));
        REQUIRE_FALSE(YRef<Boolean>(yNode).value());
    }
    SECTION("Assign nullptr to YNode.", "[YAML][YNode][Assignment][Null]")
    {
        yNode = nullptr;
        REQUIRE_FALSE(!isA<Null>(yNode));
        REQUIRE(YRef<Null>(yNode).value() == nullptr);
    }
    SECTION("Assign array to YNode.", "[YAML][YNode][Assignment][Array]")
    {
        yNode = { 1, 2, 3, 4 };
        REQUIRE_FALSE(!isA<Array>(yNode));
        auto &array = YRef<Array>(yNode).value();
        REQUIRE(YRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(YRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(YRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(YRef<Number>(array[3]).value<int>() == 4);
    }
    SECTION("Assign object to YNode.", "[YAML][YNode][Assignment][Dictionary]")
    {
        yNode = { { "key1", 55 }, { "key2", 26666 } };
        REQUIRE_FALSE(!isA<Dictionary>(yNode));
        auto &object = YRef<Dictionary>(yNode);
        REQUIRE(YRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(YRef<Number>(object["key2"]).value<int>() == 26666);
    }
    SECTION("Assign YNode with array with nested array.", "[YAML][YNode][Constructor][Array]")
    {
        yNode = { 1, 2, 3, 4, YNode{ 5, 6, 7, 8 } };
        REQUIRE_FALSE(!isA<Array>(yNode));
        auto &array = YRef<Array>(yNode).value();
        REQUIRE(YRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(YRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(YRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(YRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(YRef<Number>(array[4][0]).value<int>() == 5);
        REQUIRE(YRef<Number>(array[4][1]).value<int>() == 6);
        REQUIRE(YRef<Number>(array[4][2]).value<int>() == 7);
        REQUIRE(YRef<Number>(array[4][3]).value<int>() == 8);
    }
    SECTION("Assign YNode with object with a nested object.", "[YAML][YNode][Constructor][Dictionary]")
    {
        yNode = { { "key1", 55 }, { "key2", 26666 }, { "key3", YNode{ { "key4", 5555 }, { "key5", 7777 } } } };
        REQUIRE_FALSE(!isA<Dictionary>(yNode));
        auto &object = YRef<Dictionary>(yNode);
        REQUIRE(YRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(YRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(YRef<Number>(object["key3"]["key4"]).value<int>() == 5555);
        REQUIRE(YRef<Number>(object["key3"]["key5"]).value<int>() == 7777);
    }
    SECTION("Assign YNode with array with a nested object.", "[YAML][YNode][Constructor][Array]")
    {
        yNode = { 1, 2, 3, 4, YNode{ { "key4", 5555 }, { "key5", 7777 } } };
        REQUIRE_FALSE(!isA<Array>(yNode));
        auto &array = YRef<Array>(yNode).value();
        REQUIRE(YRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(YRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(YRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(YRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(YRef<Number>(array[4]["key4"]).value<int>() == 5555);
        REQUIRE(YRef<Number>(array[4]["key5"]).value<int>() == 7777);
    }
    SECTION("Assign YNode with object with nested a array.", "[YAML][YNode][Constructor][Dictionary]")
    {
        yNode = { { "key1", 55 }, { "key2", 26666 }, { "key3", YNode{ 5, 6, 7, 8 } } };
        REQUIRE_FALSE(!isA<Dictionary>(yNode));
        auto &object = YRef<Dictionary>(yNode);
        REQUIRE(YRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(YRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(YRef<Number>(object["key3"][0]).value<int>() == 5);
        REQUIRE(YRef<Number>(object["key3"][1]).value<int>() == 6);
        REQUIRE(YRef<Number>(object["key3"][2]).value<int>() == 7);
        REQUIRE(YRef<Number>(object["key3"][3]).value<int>() == 8);
    }
}
