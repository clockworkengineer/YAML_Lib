#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check use of Node constructors.", "[YAML][Node][Constructor]")
{
    SECTION("Construct Node(integer).", "[YAML][Node][Constructor][Integer]")
    {
        Node jNode(999);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!NRef<Number>(jNode).is<int>());
        REQUIRE(NRef<Number>(jNode).value<int>() == 999);
    }
    SECTION("Construct Node(long).", "[YAML][Node][Constructor][Long]")
    {
        Node jNode(99988899l);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!NRef<Number>(jNode).is<long>());
        REQUIRE(NRef<Number>(jNode).value<int>() == 99988899l);
    }
    SECTION("Construct Node(long long).", "[YAML][Node][Constructor][Long Long]")
    {
        Node jNode(99988899ll);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!NRef<Number>(jNode).is<long long>());
        REQUIRE(NRef<Number>(jNode).value<long long>() == 99988899ll);
    }
    SECTION("Construct Node(float).", "[YAML][Node][Constructor][Float]")
    {
        Node jNode(777.999f);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!NRef<Number>(jNode).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(jNode).value<float>(), 777.999f, 0.0001));
    }
    SECTION("Construct Node(double).", "[YAML][Node][Constructor][Double]")
    {
        Node jNode(66666.8888);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!NRef<Number>(jNode).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(jNode).value<double>(), 66666.8888, 0.0001));
    }
    SECTION("Construct Node(long double).", "[YAML][Node][Constructor][Long Double]")
    {
        Node jNode(66666.8888l);
        REQUIRE_FALSE(!isA<Number>(jNode));
        REQUIRE_FALSE(!NRef<Number>(jNode).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(jNode).value<long double>(), 66666.8888l, 0.0001));
    }
    SECTION("Construct Node(C string).", "[YAML][Node][Constructor][CString]")
    {
        Node jNode("test string");
        REQUIRE_FALSE(!isA<String>(jNode));
        REQUIRE(NRef<String>(jNode).value() == "test string");
    }
    SECTION("Construct Node(string).", "[YAML][Node][Constructor][String]")
    {
        Node jNode("test string");
        REQUIRE_FALSE(!isA<String>(jNode));
        REQUIRE(NRef<String>(jNode).value() == "test string");
    }
    SECTION("Construct Node(boolean).", "[YAML][Node][Constructor][Boolean]")
    {
        Node jNode(false);
        REQUIRE_FALSE(!isA<Boolean>(jNode));
        REQUIRE_FALSE(NRef<Boolean>(jNode).value());
    }
    SECTION("Construct Node(null).", "[YAML][Node][Constructor][Null]")
    {
        Node jNode(nullptr);
        REQUIRE_FALSE(!isA<Null>(jNode));
        REQUIRE(NRef<Null>(jNode).value() == nullptr);
    }
    SECTION("Construct Node(array).", "[YAML][Node][Constructor][Array]")
    {
        Node jNode{ 1, 2, 3, 4 };
        REQUIRE_FALSE(!isA<Array>(jNode));
        auto &array = NRef<Array>(jNode).value();
        REQUIRE(NRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(NRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(NRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(NRef<Number>(array[3]).value<int>() == 4);
    }
    SECTION("Construct Node(object).", "[YAML][Node][Constructor][Dictionary]")
    {
        Node jNode{ { "key1", 55 }, { "key2", 26666 } };
        REQUIRE_FALSE(!isA<Dictionary>(jNode));
        auto &object = NRef<Dictionary>(jNode);
        REQUIRE(NRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(NRef<Number>(object["key2"]).value<int>() == 26666);
    }
    SECTION("Construct Node(array with nested array).", "[YAML][Node][Constructor][Array]")
    {
        Node jNode{ 1, 2, 3, 4, Node{ 5, 6, 7, 8 } };
        REQUIRE_FALSE(!isA<Array>(jNode));
        auto &array = NRef<Array>(jNode).value();
        REQUIRE(NRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(NRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(NRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(NRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(NRef<Number>(array[4][0]).value<int>() == 5);
        REQUIRE(NRef<Number>(array[4][1]).value<int>() == 6);
        REQUIRE(NRef<Number>(array[4][2]).value<int>() == 7);
        REQUIRE(NRef<Number>(array[4][3]).value<int>() == 8);
    }
    SECTION("Construct Node(object with nested object).", "[YAML][Node][Constructor][Dictionary]")
    {
        Node jNode{ { "key1", 55 }, { "key2", 26666 }, { "key3", Node{ { "key4", 5555 }, { "key5", 7777 } } } };
        REQUIRE_FALSE(!isA<Dictionary>(jNode));
        auto &object = NRef<Dictionary>(jNode);
        REQUIRE(NRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(NRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(NRef<Number>(object["key3"]["key4"]).value<int>() == 5555);
        REQUIRE(NRef<Number>(object["key3"]["key5"]).value<int>() == 7777);
    }
    SECTION("Construct Node(array with nested object).", "[YAML][Node][Constructor][Array]")
    {
        Node jNode{ 1, 2, 3, 4, Node{ { "key4", 5555 }, { "key5", 7777 } } };
        REQUIRE_FALSE(!isA<Array>(jNode));
        auto &array = NRef<Array>(jNode).value();
        REQUIRE(NRef<Number>(array[0]).value<int>() == 1);
        REQUIRE(NRef<Number>(array[1]).value<int>() == 2);
        REQUIRE(NRef<Number>(array[2]).value<int>() == 3);
        REQUIRE(NRef<Number>(array[3]).value<int>() == 4);
        REQUIRE(NRef<Number>(array[4]["key4"]).value<int>() == 5555);
        REQUIRE(NRef<Number>(array[4]["key5"]).value<int>() == 7777);
    }
    SECTION("Construct Node(object with nested array).", "[YAML][Node][Constructor][Dictionary]")
    {
        Node jNode{ { "key1", 55 }, { "key2", 26666 }, { "key3", Node{ 5, 6, 7, 8 } } };
        REQUIRE_FALSE(!isA<Dictionary>(jNode));
        auto &object = NRef<Dictionary>(jNode);
        REQUIRE(NRef<Number>(object["key1"]).value<int>() == 55);
        REQUIRE(NRef<Number>(object["key2"]).value<int>() == 26666);
        REQUIRE(NRef<Number>(object["key3"][0]).value<int>() == 5);
        REQUIRE(NRef<Number>(object["key3"][1]).value<int>() == 6);
        REQUIRE(NRef<Number>(object["key3"][2]).value<int>() == 7);
        REQUIRE(NRef<Number>(object["key3"][3]).value<int>() == 8);
    }
}
