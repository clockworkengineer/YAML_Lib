#include "YAML_Lib_Tests.hpp"
#include <filesystem>
#include <fstream>

#ifdef YAML_LIB_FILE_IO
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
    SECTION("Check that getFileFormat() rejects unsupported BOM formats.", "[YAML][GetFileFormat][BOM]")
    {
        const auto tempFile = std::filesystem::temp_directory_path() /
            ("yaml_lib_unsupported_bom_" + generateRandomFileName() + ".yaml");
        std::ofstream out(tempFile, std::ios::binary);
        out.put(static_cast<char>(0x2B));
        out.put(static_cast<char>(0x2F));
        out.put(static_cast<char>(0x76));
        out.put(static_cast<char>(0x38));
        out << "---\nkey: value\n";
        out.close();
        REQUIRE_THROWS_AS(YAML::getFileFormat(tempFile.string()), YAML_Lib::Error);
        std::filesystem::remove(tempFile);
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
#endif // YAML_LIB_FILE_IO
