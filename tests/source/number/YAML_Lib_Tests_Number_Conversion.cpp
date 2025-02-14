#include "YAML_Lib_Tests.hpp"

TEST_CASE("Check JNode Number conversion.", "[YAML][JNode][Number]")
{
    const YAML yaml;
    // Note: That floating point -> integer will be rounded up/down
    SECTION("Float converted to int.", "[YAML][JNode][Number][Float]")
    {
        BufferSource yamlSource{ "6788888.8990" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE(YRef<Number>(yaml.document(0)).value<int>() == 6788889);
    }
    SECTION("Float converted to long.", "[YAML][JNode][Number][Float]")
    {
        BufferSource yamlSource{ "6788888.8990" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long>());
        REQUIRE(YRef<Number>(yaml.document(0)).value<long>() == 6788889);
    }
    SECTION("Float converted to long long.", "[YAML][JNode][Number][Float]")
    {
        BufferSource yamlSource{ "6788888.8990" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long long>());
        REQUIRE(YRef<Number>(yaml.document(0)).value<long long>() == 6788889ll);
    }
    SECTION("Float converted to double.", "[YAML][JNode][Number][Float]")
    {
        BufferSource yamlSource{ "678.8990" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yaml.document(0)).value<double>(), 678.8990, 0.0001));
    }
    SECTION("Float converted to long double.", "[YAML][JNode][Number][Float]")
    {
        BufferSource yamlSource{ "678.8990" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yaml.document(0)).value<long double>(), 678.8990l, 0.0001));
    }
    SECTION("Int converted to long.", "[YAML][JNode][Number][Int]")
    {
        BufferSource yamlSource{ "78989" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long>());
        REQUIRE(YRef<Number>(yaml.document(0)).value<long>() == 78989);
    }
    SECTION("Int converted to long long.", "[YAML][JNode][Number][Int]")
    {
        BufferSource yamlSource{ "78989" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long long>());
        REQUIRE(YRef<Number>(yaml.document(0)).value<long long>() == 78989ll);
    }
    SECTION("Int converted to float.", "[YAML][JNode][Number][Int]")
    {
        BufferSource yamlSource{ "78989" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yaml.document(0)).value<float>(), 78989.0f, 0.0001));
    }
    SECTION("Int converted to double.", "[YAML][JNode][Number][Int]")
    {
        BufferSource yamlSource{ "78989" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yaml.document(0)).value<double>(), 78989.0, 0.0001));
    }
    SECTION("Int converted to long double.", "[YAML][JNode][Number][Int]")
    {
        BufferSource yamlSource{ "78989" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long double>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yaml.document(0)).value<long double>(), 78989.0l, 0.0001));
    }
    SECTION("Large positive integer conversion.", "[YAML][JNode][Number][Long Long]")
    {
        BufferSource yamlSource{ "9223372036854775807" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<int>());
        // If long and long long same size then integer never stored in long long
        // but you can fetch it as a long long
        if constexpr (sizeof(long long) > sizeof(long)) {
            REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long>());
            REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<long long>());
        } else {
            REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<long>());
            REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long long>());
        }
        REQUIRE(YRef<Number>(yaml.document(0)).value<long long>() == 9223372036854775807ll);
    }
    SECTION("Large negative integer conversion.", "[YAML][JNode][Number][Long Long]")
    {
        BufferSource yamlSource{ "-877994604561387500" };
        yaml.parse(yamlSource);
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<int>());
        // If long and long long same size then integer never stored in long long
        // but you can fetch it as a long long
        if constexpr (sizeof(long long) > sizeof(long)) {
            REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long>());
            REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<long long>());
        } else {
            REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<long>());
            REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long long>());
        }
        REQUIRE(YRef<Number>(yaml.document(0)).value<long long>() == -877994604561387500ll);
    }
    SECTION("Positive integer overflow conversion.", "[YAML][JNode][Number][Long Long]")
    {
        BufferSource yamlSource{ "9223372036854775808" };// LLONG_MAX + 1
        yaml.parse(yamlSource);
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long long>());
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yaml.document(0)).value<float>(), 9223372036854775808.0f, 0.0001));
    }
    SECTION("Negative integer overflow conversion.", "[YAML][JNode][Number][Long Long]")
    {
        BufferSource yamlSource{ "-9223372036854775809" };// LLONG_MIN - 1
        yaml.parse(yamlSource);
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<int>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long>());
        REQUIRE_FALSE(YRef<Number>(yaml.document(0)).is<long long>());
        REQUIRE_FALSE(!YRef<Number>(yaml.document(0)).is<float>());
        REQUIRE_FALSE(!equalFloatingPoint(YRef<Number>(yaml.document(0)).value<float>(), -9223372036854775808.0f, 0.0001));
    }
}
