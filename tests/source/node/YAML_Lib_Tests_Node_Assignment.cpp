#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check use of Node assigment operators.", "[YAML][Node][Assignment]")
{
    Node yNode;
    SECTION("Assign integer to Node.", "[YAML][Node][Assignment][Integer]")
    {
        yNode = 999;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!NRef<Number>(yNode).is<int>());
        REQUIRE(NRef<Number>(yNode).value<int>() == 999);
    }
    SECTION("Assign long to Node.", "[YAML][Node][Assignment][Long]")
    {
        yNode = 99988899l;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!NRef<Number>(yNode).is<long>());
        REQUIRE(NRef<Number>(yNode).value<int>() == 99988899l);
    }
    SECTION("Assign long long to Node.", "[YAML][Node][Assignment][Long Long]")
    {
        yNode = 99988899ll;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!NRef<Number>(yNode).is<long long>());
        REQUIRE(NRef<Number>(yNode).value<long long>() == 99988899ll);
    }
    SECTION("Assign float to Node.", "[YAML][Node][Assignment][Float]")
    {
        yNode = 777.999f;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!NRef<Number>(yNode).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(yNode).value<float>(), 777.999f, 0.0001));
    }
    SECTION("Assign double to Node.", "[YAML][Node][Assignment][Double]")
    {
        yNode = 66666.8888;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!NRef<Number>(yNode).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(yNode).value<double>(), 66666.8888, 0.0001));
    }
    SECTION("Assign long double to Node.", "[YAML][Node][Assignment][Long Double]")
    {
        yNode = 66666.8888l;
        REQUIRE_FALSE(!isA<Number>(yNode));
        REQUIRE_FALSE(!NRef<Number>(yNode).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(yNode).value<long double>(), 66666.8888l, 0.0001));
    }
    SECTION("Assign C string to Node.", "[YAML][Node][Assignment][CString]")
    {
        yNode = "test string";
        REQUIRE_FALSE(!isA<String>(yNode));
        REQUIRE(NRef<String>(yNode).value() == "test string");
    }
    SECTION("Assign string to Node.", "[YAML][Node][Assignment][String]")
    {
        yNode = std::string("test string");
        REQUIRE_FALSE(!isA<String>(yNode));
        REQUIRE(NRef<String>(yNode).value() == "test string");
    }
    SECTION("Assign boolean to Node.", "[YAML][Node][Assignment][Boolean]")
    {
        yNode = false;
        REQUIRE_FALSE(!isA<Boolean>(yNode));
        REQUIRE_FALSE(NRef<Boolean>(yNode).value());
    }
    SECTION("Assign nullptr to Node.", "[YAML][Node][Assignment][Null]")
    {
        yNode = nullptr;
        REQUIRE_FALSE(!isA<Null>(yNode));
        REQUIRE(NRef<Null>(yNode).value() == nullptr);
    }
    SECTION("Assign array to Node.", "[YAML][Node][Assignment][Array]")
    {
        yNode = { 1, 2, 3, 4 };
        REQUIRE_FALSE(!isA<Array>(yNode));
        auto &array = NRef<Array>(yNode).value();
        REQUIRE(NRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(NRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(NRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(NRef<Number>(array[3]).value<int>() == 4);
    }
    SECTION("Assign object to Node.", "[YAML][Node][Assignment][Dictionary]")
    {
        yNode = { { "key1", 55 }, { "key2", 26666 } };
        REQUIRE_FALSE(!isA<Dictionary>(yNode));
        auto &object = NRef<Dictionary>(yNode);
        REQUIRE(NRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(NRef<Number>(object["key2"]).value<int>() == 26666);
    }
    SECTION("Assign Node with array with nested array.", "[YAML][Node][Constructor][Array]")
    {
        yNode = { 1, 2, 3, 4, Node{ 5, 6, 7, 8 } };
        REQUIRE_FALSE(!isA<Array>(yNode));
        auto &array = NRef<Array>(yNode).value();
        REQUIRE(NRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(NRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(NRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(NRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(NRef<Number>(array[4][0]).value<int>() == 5);
        REQUIRE(NRef<Number>(array[4][1]).value<int>() == 6);
        REQUIRE(NRef<Number>(array[4][2]).value<int>() == 7);
        REQUIRE(NRef<Number>(array[4][3]).value<int>() == 8);
    }
    SECTION("Assign Node with object with a nested object.", "[YAML][Node][Constructor][Dictionary]")
    {
        yNode = { { "key1", 55 }, { "key2", 26666 }, { "key3", Node{ { "key4", 5555 }, { "key5", 7777 } } } };
        REQUIRE_FALSE(!isA<Dictionary>(yNode));
        auto &object = NRef<Dictionary>(yNode);
        REQUIRE(NRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(NRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(NRef<Number>(object["key3"]["key4"]).value<int>() == 5555);
        REQUIRE(NRef<Number>(object["key3"]["key5"]).value<int>() == 7777);
    }
    SECTION("Assign Node with array with a nested object.", "[YAML][Node][Constructor][Array]")
    {
        yNode = { 1, 2, 3, 4, Node{ { "key4", 5555 }, { "key5", 7777 } } };
        REQUIRE_FALSE(!isA<Array>(yNode));
        auto &array = NRef<Array>(yNode).value();
        REQUIRE(NRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(NRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(NRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(NRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(NRef<Number>(array[4]["key4"]).value<int>() == 5555);
        REQUIRE(NRef<Number>(array[4]["key5"]).value<int>() == 7777);
    }
    SECTION("Assign Node with object with nested a array.", "[YAML][Node][Constructor][Dictionary]")
    {
        yNode = { { "key1", 55 }, { "key2", 26666 }, { "key3", Node{ 5, 6, 7, 8 } } };
        REQUIRE_FALSE(!isA<Dictionary>(yNode));
        auto &object = NRef<Dictionary>(yNode);
        REQUIRE(NRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(NRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(NRef<Number>(object["key3"][0]).value<int>() == 5);
        REQUIRE(NRef<Number>(object["key3"][1]).value<int>() == 6);
        REQUIRE(NRef<Number>(object["key3"][2]).value<int>() == 7);
        REQUIRE(NRef<Number>(object["key3"][3]).value<int>() == 8);
    }
}
