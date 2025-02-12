#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check YNode Number assignment/creation.", "[YAML][YNode][Number][Assignment]")
{
    SECTION("Create YNode with int assigment.", "[YAML][YNode][Number][Integer][Assignment]")
    {
        YNode yNode;
        yNode = 666;
        REQUIRE_FALSE(!YRef<Number>(yNode).is<int>());
        REQUIRE(YRef<Number>(yNode).value<int>() == 666);
    }
    SECTION("Create YNode with long assigment.", "[YAML][YNode][Number][Long][Assignment]")
    {
        YNode yNode;
        yNode = 666l;
        REQUIRE_FALSE(!YRef<Number>(yNode).is<long>());
        REQUIRE(YRef<Number>(yNode).value<long>() == 666l);
    }
    SECTION("Create YNode with long long assigment.", "[YAML][YNode][Number][Long Long][Assignment]")
    {
        YNode yNode;
        yNode = 666ll;
        REQUIRE_FALSE(!YRef<Number>(yNode).is<long long>());
        REQUIRE(YRef<Number>(yNode).value<long>() == 666ll);
    }
    SECTION("Create YNode with float assigment.", "[YAML][YNode][Number][Float][Assignment]")
    {
        YNode yNode;
        yNode = 666.666f;
        REQUIRE_FALSE(!YRef<Number>(yNode).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yNode).value<float>(), 666.666f, 0.0001));
    }
    SECTION("Create YNode with double assigment.", "[YAML][YNode][Number][Double][Assignment]")
    {
        YNode yNode;
        yNode = 666.666;
        REQUIRE_FALSE(!YRef<Number>(yNode).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yNode).value<double>(), 666.666, 0.0001));
    }
    SECTION("Create YNode with long double assigment.", "[YAML][YNode][Number][Long Double][Assignment]")
    {
        YNode yNode;
        yNode = 666.666l;
        REQUIRE_FALSE(!YRef<Number>(yNode).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yNode).value<long double>(), 666.666l, 0.0001));
    }
}
