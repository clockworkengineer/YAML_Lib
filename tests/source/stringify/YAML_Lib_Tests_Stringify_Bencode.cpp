#include "YAML_Lib_Tests.hpp"
#include "Bencode_Stringify.hpp"

TEST_CASE("Check YAML stringification to Bencode of simple types.", "[YAML][Stringify][Simple][Bencode]")
{
    const YAML yaml(makeStringify<Bencode_Stringify>());
    SECTION("Stringify a string (abcdefghijklmnopqrstuvwxyz) to Bencode.", "[YAML][Stringify][String][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ R"("abcdefghijklmnopqrstuvwxyz")" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "26:abcdefghijklmnopqrstuvwxyz");
    }
    SECTION("Stringify a boolean (true) to Bencode.", "[YAML][Stringify][Boolean][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ "True" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "4:True");
    }
    SECTION("Stringify a boolean (false) to Bencode.", "[YAML][Stringify][Boolean][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ "False" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "5:False");
    }
    SECTION("Stringify a integer (98345) to Bencode.", "[YAML][Stringify][Integer][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ "98345" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "i98345e");
    }
    SECTION("Stringify a integer (-98345) to Bencode.", "[YAML][Stringify][Integer][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ "-98345" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "i-98345e");
    }
    SECTION("Stringify a floating point (55.6667) to Bencode.", "[YAML][Stringify][Float][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ "55.6667" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "i55e");
    }
    SECTION("Stringify a floating point (-55.6667) to Bencode.", "[YAML][Stringify][Float][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ "-55.6667" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "i-55e");
    }
    SECTION("Stringify a null to Bencode.", "[YAML][Stringify][Simple][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ { "null" } });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "4:null");
    }
    SECTION("Stringify an array to Bencode.", "[YAML][Stringify][Array][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ { "[1,444,555,666,67.99]" } });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "li1ei444ei555ei666ei67ee");
    }
    SECTION("Stringify an empty array to Bencode.", "[YAML][Stringify][Array][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ { "[]" } });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "le");
    }
    SECTION("Stringify an object to Bencode.", "[YAML][Stringify][Object][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ { R"({"Age":77,"Name":"Rob"})" } });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "d3:Agei77e4:Name3:Robe");
    }
    SECTION("Stringify an empty object to Bencode.", "[YAML][Stringify][Object][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ { R"({})" } });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "de");
    }
    SECTION(R"(Stringify an nested array ({"City":"London","Population":[1,2,3,4,5]}) to Bencode.)",
            "[YAML][Stringify][Array][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ R"({"City":"London","Population":[1,2,3,4,5]})" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "d4:City6:London10:Populationli1ei2ei3ei4ei5eee");
    }
    SECTION(R"(Stringify a nested object ([true,"Out of time",7.89043e+18,{"key":4444}]) to a Bencode.)",
            "[YAML][Stringify][Object][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ R"([True,"Out of time",7.89043e+18,{"key":4444}])" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "l4:True11:Out of timei7890430242211233792ed3:keyi4444eee");
    }
    SECTION(R"(Stringify [{},{},{}] to a Bencode.)", "[YAML][Stringify][Object][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ R"([{},{},{}])" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "ldededee");
    }
    SECTION(R"(Stringify [[], [],[]] to a Bencode.)", "[YAML][Stringify][Object][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ R"([[], [],[]])" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "llelelee");
    }
    SECTION(R"(Stringify {"Test" : [[],[],[]]} to a Bencode.)", "[YAML][Stringify][Object][Bencode]")
    {
        BufferDestination yamlDestination;
        yaml.parse(BufferSource{ R"({"Test" : [[],[],[]]})" });
        yaml.stringify(yamlDestination);
        REQUIRE(yamlDestination.toString() == "d4:Testlleleleee");
    }
}
