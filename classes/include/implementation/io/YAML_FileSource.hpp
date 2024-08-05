#pragma once

#include "ISource.hpp"

namespace YAML_Lib {

class FileSource final : public ISource
{
public:
  // FileSource Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("FileSource Error: " + message) {}
  };
  // Constructors/Destructors
  explicit FileSource(const std::string &sourceFileName)
  {
    source.open(sourceFileName.c_str(), std::ios_base::binary);
    if (!source.is_open()) { throw Error("File input stream failed to open or does not exist."); }
    if (current_character() == kCarriageReturn) {
      source.get();
      if (current_character() != kLineFeed) { source.unget(); }
    }
  }
  FileSource() = default;
  FileSource(const FileSource &other) = delete;
  FileSource &operator=(const FileSource &other) = delete;
  FileSource(FileSource &&other) = delete;
  FileSource &operator=(FileSource &&other) = delete;
  ~FileSource() override = default;

  [[nodiscard]] Char current() const override { return current_character(); }
  void next() override
  {
    if (!more()) { throw Error("Parse buffer empty before parse complete."); }
    source.get();
    if (current() == kCarriageReturn) {
      source.get();
      if (current() != kLineFeed) { source.unget(); }
    }
    columnNo++;
    if (current() == kLineFeed) {
      lineNo++;
      columnNo = 1;
    }
  }
  [[nodiscard]] bool more() const override { return source.peek() != EOF; }
  void backup(const long length) override
  {
    if (static_cast<long>(source.tellg()) - length >= 0 || current() == static_cast<Char>(EOF)) {
      source.clear();
      source.seekg(-length, std::ios_base::cur);
    } else {
      source.seekg(0, std::ios_base::beg);
    }
  }
  [[nodiscard]] long position() const override { return source.tellg(); }
  void reset() override
  {
    lineNo = 1;
    columnNo = 1;
    source.clear();
    source.seekg(0, std::ios_base::beg);
  }
  [[nodiscard]] std::string getRange(const long start, const long end) override
  {
    std::string rangeBuffer(static_cast<std::size_t>(end) - start, ' ');
    const long currentPosition = source.tellg();
    source.seekg(start, std::ios_base::beg);
    source.read(&rangeBuffer[0], static_cast<std::streamsize>(end) - start);
    source.seekg(currentPosition, std::ios_base::beg);
    return rangeBuffer;
  }

private:
  [[nodiscard]] Char current_character() const { return static_cast<Char>(source.peek()); }
  mutable std::ifstream source;
};
}// namespace YAML_Lib