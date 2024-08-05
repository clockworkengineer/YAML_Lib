#pragma once

#include "ISource.hpp"

namespace XML_Lib {

class BufferSource final : public ISource
{
public:
  // Bits per byte
  static constexpr int kBitsPerByte{ 8 };
  // BufferSource Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("BufferSource Error: " + message) {}
  };
  // Constructors/Destructors
  // explicit BufferSource(const std::u16string &sourceBuffer)// UTF16 source BE/LE
  // {
  //   if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
  //   std::u16string utf16xml{ sourceBuffer };
  //   if (utf16xml.starts_with(u"<?xml")) {
  //     std::ranges::transform(utf16xml, utf16xml.begin(), [](const char16_t &ch) {
  //       return static_cast<uint16_t>(ch) >> kBitsPerByte | static_cast<uint16_t>(ch) << kBitsPerByte;
  //     });
  //   }
  //   buffer = utf16xml;
  //   convertCRLFToLF(buffer);
  // }
  // explicit BufferSource(const std::string &sourceBuffer) : buffer{ toUtf16(sourceBuffer) }
  // {
  //   if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
  //   convertCRLFToLF(buffer);
  // }
  BufferSource() = default;
  BufferSource(const BufferSource &other) = delete;
  BufferSource &operator=(const BufferSource &other) = delete;
  BufferSource(BufferSource &&other) = delete;
  BufferSource &operator=(BufferSource &&other) = delete;
  ~BufferSource() override = default;

  [[nodiscard]] Char current() const override
  {
    if (more()) { return buffer[bufferPosition]; }
    return static_cast<Char>(EOF);
  }
  void next() override
  {
    if (!more()) { throw Error("Parse buffer empty before parse complete."); }
    bufferPosition++;
    columnNo++;
    if (current() == kLineFeed) {
      lineNo++;
      columnNo = 1;
    }
  }
  [[nodiscard]] bool more() const override { return bufferPosition < static_cast<long>(buffer.size()); }
  void backup(const long length) override
  {
    bufferPosition -= length;
    if (bufferPosition < 0) { bufferPosition = 0; }
  }
  [[nodiscard]] long position() const override { return bufferPosition; }
  [[nodiscard]] std::string getRange(const long start, const long end) override
  {
    return toUtf8(buffer.substr(start, static_cast<std::size_t>(end) - start));
  }
  void reset() override
  {
    lineNo = 1;
    columnNo = 1;
    bufferPosition = 0;
  }

private:
  // static void convertCRLFToLF(String &xmlString)
  // {
  //   size_t pos = xmlString.find(u"\x0D\x0A");
  //   while (pos != std::string::npos) {
  //     xmlString.replace(pos, 2, u"\x0A");
  //     pos = xmlString.find(u"\x0D\x0A", pos + 1);
  //   }
  // }

  long bufferPosition = 0;
  String buffer;
};
}// namespace XML_Lib