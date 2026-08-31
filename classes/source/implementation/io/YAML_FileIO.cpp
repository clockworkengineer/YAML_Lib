#include "implementation/io/YAML_FileIO.hpp"

#ifdef YAML_LIB_FILE_IO

#include <fstream>
#include <sstream>
#include "YAML_Impl.hpp"
#include "implementation/converter/YAML_Converter.hpp"

namespace YAML_Lib {

namespace {

void writeYAMLString(std::ofstream &yamlFile, const std::string_view &yamlString) {
  yamlFile << yamlString;
}

void writeYAMLString(std::ofstream &yamlFile, const std::u16string &yamlString, const YAML::Format format) {
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
    YAML_THROW(Error, "Unsupported YAML file format (Byte Order Mark) specified in call to writeYAMLString().");
  }
}

std::string readYAMLString(const std::ifstream &yamlFile) {
  std::ostringstream yamlFileBuffer;
  yamlFileBuffer << yamlFile.rdbuf();
  return yamlFileBuffer.str();
}

std::u16string readYAMLString(std::ifstream &yamlFile, const YAML::Format format) {
  std::u16string utf16String;
  yamlFile.seekg(2);
  if (format == YAML::Format::utf16BE) {
    while (true) {
      const int hi = yamlFile.get();
      const int lo = yamlFile.get();
      if (hi == EOF && lo == EOF) {
        break;
      }
      if (hi == EOF || lo == EOF) {
        YAML_THROW(Error, "Truncated UTF-16 YAML file.");
      }
      const char16_t ch16 = static_cast<char16_t>(
          (static_cast<unsigned char>(hi) << 8) |
          static_cast<unsigned char>(lo));
      utf16String.push_back(ch16);
    }
  } else if (format == YAML::Format::utf16LE) {
    while (true) {
      const int lo = yamlFile.get();
      const int hi = yamlFile.get();
      if (lo == EOF && hi == EOF) {
        break;
      }
      if (lo == EOF || hi == EOF) {
        YAML_THROW(Error, "Truncated UTF-16 YAML file.");
      }
      const char16_t ch16 = static_cast<char16_t>(
          static_cast<unsigned char>(lo) |
          (static_cast<unsigned char>(hi) << 8));
      utf16String.push_back(ch16);
    }
  } else {
    YAML_THROW(Error, "Unsupported YAML file format (Byte Order Mark) specified in call to readYAMLString().");
  }
  return utf16String;
}

void validateInputFile(const std::ifstream &file, const std::string_view &fileName) {
  if (!file) {
    YAML_THROW(Error, "Failed to open YAML file '" + std::string(fileName) + "'.");
  }
}

void validateOutputFile(const std::ofstream &file, const std::string_view &fileName) {
  if (!file) {
    YAML_THROW(Error, "Failed to open YAML output file '" + std::string(fileName) + "'.");
  }
}

} // namespace

YAML::Format YAML_FileReader::getFileFormat(const std::string_view &fileName) {
  const std::string fileNameStr{fileName};
  std::ifstream yamlFile{fileNameStr, std::ios_base::binary};
  validateInputFile(yamlFile, fileName);

  char bomBuffer[4] = {};
  yamlFile.read(bomBuffer, sizeof(bomBuffer));
  if (!yamlFile && !yamlFile.eof()) {
    YAML_THROW(Error, "Failed to read YAML file '" + fileNameStr + "'.");
  }

  const uint32_t byteOrderMark = (static_cast<unsigned char>(bomBuffer[0]) << 24) |
                                 (static_cast<unsigned char>(bomBuffer[1]) << 16) |
                                 (static_cast<unsigned char>(bomBuffer[2]) << 8) |
                                 (static_cast<unsigned char>(bomBuffer[3]));
  yamlFile.close();
  if (byteOrderMark == 0x2B2F7638 || byteOrderMark == 0xF7BBBF00 ||
      byteOrderMark == 0xDD736673 || byteOrderMark == 0xFBEE2800 ||
      byteOrderMark == 0x84319533) {
    YAML_THROW(Error, "Unsupported YAML file format (Byte Order Mark) encountered.");
  }
  if (byteOrderMark == 0x0000FEFF) { return YAML::Format::utf32BE; }
  if (byteOrderMark == 0xFFFE0000) { return YAML::Format::utf32LE; }
  if ((byteOrderMark & 0xFFFFFF00) == 0xEFBBBF00) { return YAML::Format::utf8BOM; }
  if ((byteOrderMark & 0xFFFF0000) == 0xFEFF0000) { return YAML::Format::utf16BE; }
  if ((byteOrderMark & 0xFFFF0000) == 0xFFFE0000) { return YAML::Format::utf16LE; }
  return YAML::Format::utf8;
}

std::string YAML_FileReader::read(const std::string_view &fileName) {
  const auto kCRLF = "\x0D\x0A";
  const YAML::Format format = getFileFormat(fileName);
  const std::string fileNameStr{fileName};
  std::ifstream yamlFile{fileNameStr, std::ios_base::binary};
  validateInputFile(yamlFile, fileName);
  std::string translated;
  switch (format) {
  case YAML::Format::utf8BOM:
    yamlFile.seekg(3);
    if (!yamlFile) {
      YAML_THROW(Error, "Failed to read YAML file '" + std::string(fileName) + "'.");
    }
  case YAML::Format::utf8:
    translated = readYAMLString(yamlFile);
    break;
  case YAML::Format::utf16BE:
  case YAML::Format::utf16LE:
    translated = toUtf8(readYAMLString(yamlFile, format));
    break;
  default:
    YAML_THROW(Error, "Unsupported YAML file format (Byte Order Mark) encountered.");
  }
  yamlFile.close();
  size_t pos = translated.find(kCRLF);
  while (pos != std::string::npos) {
    const auto kLF = "\x0A";
    translated.replace(pos, 2, kLF);
    pos = translated.find(kCRLF, pos + 1);
  }
  return translated;
}

void YAML_FileWriter::write(const std::string_view &fileName,
                           const std::string_view &yamlString,
                           const YAML::Format format) {
  const std::string fileNameStr{fileName};
  std::ofstream yamlFile{fileNameStr, std::ios::binary};
  validateOutputFile(yamlFile, fileName);
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
    YAML_THROW(Error, "Unsupported YAML file format (Byte Order Mark) specified.");
  }
  if (!yamlFile) {
    YAML_THROW(Error, "Failed to write YAML file '" + std::string(fileName) + "'.");
  }
  yamlFile.close();
}

} // namespace YAML_Lib

#endif // YAML_LIB_FILE_IO
