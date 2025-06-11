#include "YAML_Lib_Tests.hpp"

TEST_CASE("Checks for getFileFormat() api.", "[YAML][GetFileFormat]")
{
    SECTION("Check that getFileFormat() works with UTF8.", "[YAML][GetFileFormat][UTF8]")
    {
        REQUIRE(YAML::getFileFormat(prefixTestDataPath("testfile033.yaml")) == YAML::Format::utf8);
    }
    SECTION("Check that getFileFormat() works with UTF8BOM.", "[YAML][GetFileFormat][UTF8BOM]")
    {
        REQUIRE(YAML::getFileFormat(prefixTestDataPath("testfile034.yaml")) == YAML::Format::utf8BOM);
    }
    SECTION("Check that getFileFormat() works with UTF16BE.", "[YAML][GetFileFormat][UTF16BE]")
    {
        REQUIRE(YAML::getFileFormat(prefixTestDataPath("testfile035.yaml")) == YAML::Format::utf16BE);
    }
    SECTION("Check that getFileFormat() works with UTF16LE.", "[YAML][GetFileFormat][UTF16LE]")
    {
        REQUIRE(YAML::getFileFormat(prefixTestDataPath("testfile036.yaml")) == YAML::Format::utf16LE);
    }
    // SECTION("Check that getFileFormat() works with UTF32BE.", "[YAML][GetFileFormat][UTF32BE]")
    // {
    //     REQUIRE(YAML::getFileFormat(prefixPath("testfile025.yaml")) == YAML::Format::utf32BE);
    // }
    // SECTION("Check that getFileFormat() works with UTF32LE.", "[YAML][GetFileFormat][UTF32LE]")
    // {
    //     REQUIRE(YAML::getFileFormat(prefixPath("testfile026.yaml")) == YAML::Format::utf32LE);
    // }
}
