
//
// Class: YAML_Impl_File
//
// Description: YAML class implementation layer to read and write
// YAML files in a number of different formats. Note that these
// methods are all static and do not need a YAML object to invoke.
// For more information on byte marks and their meaning check out link
// https://en.wikipedia.org/wiki/Byte_order_mark.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Write YAML string to a file stream.
/// </summary>
/// <param name="yamlFile">YAML file stream</param>
/// <param name="yamlString">YAML string</param>
void writeYAMLString(std::ofstream &yamlFile, const std::string &yamlString) { yamlFile << yamlString; }
/// <summary>
/// Write YAML string to a file stream.
/// </summary>
/// <param name="yamlFile">YAML file stream</param>
/// <param name="yamlString">YAML string</param>
/// <param name="format">YAML file format</param>
void writeYAMLString(std::ofstream &yamlFile, const std::u16string &yamlString, const YAML::Format format)
{
    if (format == YAML::Format::utf16BE) {
        yamlFile << static_cast<unsigned char>(0xFE) << static_cast<unsigned char>(0xFF);
        for (const auto ch : yamlString) {
            yamlFile.put(static_cast<char>(ch >> 8));
            yamlFile.put(static_cast<char>(ch));
        }
    } else if (format == YAML::Format::utf16LE) {
        yamlFile << static_cast<char>(0xFF) << static_cast<char>(0xFE);
        for (const auto ch : yamlString) {
            yamlFile.put(static_cast<char>(ch));
            yamlFile.put(static_cast<char>(ch >> 8));
        }
    } else {
        throw Error("Unsupported YAML file format (Byte Order Mark) specified in call to writeYAMLString().");
    }
}

/// <summary>
/// Read YAML string from a file stream.
/// </summary>
/// <param name="yamlFile">YAML file stream</param>
/// <returns>YAML string.</returns>
std::string readYAMLString(const std::ifstream &yamlFile)
{
    std::ostringstream yamlFileBuffer;
    yamlFileBuffer << yamlFile.rdbuf();
    return yamlFileBuffer.str();
}
std::u16string readYAMLString(std::ifstream &yamlFile, const YAML::Format format)
{
    std::u16string utf16String;
    // Move past byte order mark
    yamlFile.seekg(2);
    if (format == YAML::Format::utf16BE)
        while (true) {
            char16_t ch16 = static_cast<char>(yamlFile.get()) << 8;
            ch16 |= static_cast<char>(yamlFile.get());
            if (yamlFile.eof()) break;
            utf16String.push_back(ch16);
        }
    else if (format == YAML::Format::utf16LE) {
        while (true) {
            char16_t ch16 = static_cast<char>(yamlFile.get());
            ch16 |= static_cast<char>(yamlFile.get()) << 8;
            if (yamlFile.eof()) break;
            utf16String.push_back(ch16);
        }
    } else {
        throw Error("Unsupported YAML file format (Byte Order Mark) specified in call to readYAMLString().");
    }
    return utf16String;
}

/// <summary>
/// Return format of YAML file after checking for any byte order marks at
/// the beginning of the YAML file.
/// </summary>
/// <param name="fileName">YAML file name</param>
/// <returns>YAML file format.</returns>
YAML::Format YAML_Impl::getFileFormat(const std::string &fileName)
{
    std::ifstream yamlFile{ fileName, std::ios_base::binary };
    uint32_t byteOrderMark = static_cast<unsigned char>(yamlFile.get()) << 24;
    byteOrderMark |= static_cast<unsigned char>(yamlFile.get()) << 16;
    byteOrderMark |= static_cast<unsigned char>(yamlFile.get()) << 8;
    byteOrderMark |= static_cast<unsigned char>(yamlFile.get());
    if (byteOrderMark == 0x0000FEFF) { return YAML::Format::utf32BE; }
    if (byteOrderMark == 0xFFFE0000) { return YAML::Format::utf32LE; }
    if ((byteOrderMark & 0xFFFFFF00) == 0xEFBBBF00) { return YAML::Format::utf8BOM; }
    if ((byteOrderMark & 0xFFFF0000) == 0xFEFF0000) { return YAML::Format::utf16BE; }
    if ((byteOrderMark & 0xFFFF0000) == 0xFFFE0000) { return YAML::Format::utf16LE; }
    yamlFile.close();
    return YAML::Format::utf8;
}

/// <summary>
/// Open a YAML file, read its contents into a string buffer and return
/// the buffer. Note any CRLF in the source file are translated to just a
/// LF internally.
/// </summary>
/// <param name="fileName">YAML file name</param>
/// <returns>YAML string.</returns>
std::string YAML_Impl::fromFile(const std::string &fileName)
{
    const auto kCRLF = "\x0D\x0A";
    // Get file format
    const YAML::Format format = getFileFormat(fileName);
    // Read in YAML
    std::ifstream yamlFile{ fileName, std::ios_base::binary };
    std::string translated;
    switch (format) {
    case YAML::Format::utf8BOM:
        yamlFile.seekg(3);// Move past byte order mark
    case YAML::Format::utf8:
        translated = readYAMLString(yamlFile);
        break;
    case YAML::Format::utf16BE:
    case YAML::Format::utf16LE:
        translated = toUtf8(readYAMLString(yamlFile, format));
        break;
    default:
        throw Error("Unsupported YAML file format (Byte Order Mark) encountered.");
    }
    yamlFile.close();
    // Translate CRLF -> LF
    size_t pos = translated.find(kCRLF);
    while (pos != std::string::npos) {
        const auto kLF = "\x0A";
        translated.replace(pos, 2, kLF);
        pos = translated.find(kCRLF, pos + 1);
    }
    return translated;
}

/// <summary>
/// Create an YAML file and write YAML string to it.
/// </summary>
/// <param name="fileName">YAML file name</param>
/// <param name="yamlString">YAML string</param>
/// <param name="format">YAML file format</param>
void YAML_Impl::toFile(const std::string &fileName, const std::string &yamlString, const YAML::Format format)
{
    std::ofstream yamlFile{ fileName, std::ios::binary };
    switch (format) {
    case YAML::Format::utf8BOM:
        yamlFile << static_cast<unsigned char>(0xEF) << static_cast<unsigned char>(0xBB)
                 << static_cast<unsigned char>(0xBF);
    case YAML::Format::utf8:
        writeYAMLString(yamlFile, yamlString);
        break;
    case YAML::Format::utf16BE:
    case YAML::Format::utf16LE:
        writeYAMLString(yamlFile, toUtf16(yamlString), format);
        break;
    default:
        throw Error("Unsupported YAML file format (Byte Order Mark) specified.");
    }
    yamlFile.close();
}
}// namespace YAML_Lib
