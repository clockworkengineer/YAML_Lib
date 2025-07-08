#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check Node Number assignment/creation.", "[YAML][Node][Number][Assignment]")
{
    SECTION("Create Node with int assigment.", "[YAML][Node][Number][Integer][Assignment]")
    {
        Node yNode;
        yNode = 666;
        REQUIRE_FALSE(!NRef<Number>(yNode).is<int>());
        REQUIRE(NRef<Number>(yNode).value<int>() == 666);
    }
    SECTION("Create Node with long assigment.", "[YAML][Node][Number][Long][Assignment]")
    {
        Node yNode;
        yNode = 666l;
        REQUIRE_FALSE(!NRef<Number>(yNode).is<long>());
        REQUIRE(NRef<Number>(yNode).value<long>() == 666l);
    }
    SECTION("Create Node with long long assigment.", "[YAML][Node][Number][Long Long][Assignment]")
    {
        Node yNode;
        yNode = 666ll;
        REQUIRE_FALSE(!NRef<Number>(yNode).is<long long>());
        REQUIRE(NRef<Number>(yNode).value<long>() == 666ll);
    }
    SECTION("Create Node with float assigment.", "[YAML][Node][Number][Float][Assignment]")
    {
        Node yNode;
        yNode = 666.666f;
        REQUIRE_FALSE(!NRef<Number>(yNode).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(yNode).value<float>(), 666.666f, 0.0001));
    }
    SECTION("Create Node with double assigment.", "[YAML][Node][Number][Double][Assignment]")
    {
        Node yNode;
        yNode = 666.666;
        REQUIRE_FALSE(!NRef<Number>(yNode).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(yNode).value<double>(), 666.666, 0.0001));
    }
    SECTION("Create Node with long double assigment.", "[YAML][Node][Number][Long Double][Assignment]")
    {
        Node yNode;
        yNode = 666.666l;
        REQUIRE_FALSE(!NRef<Number>(yNode).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(NRef<Number>(yNode).value<long double>(), 666.666l, 0.0001));
    }
}
