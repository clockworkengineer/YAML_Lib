
#pragma once

#include "ISource.hpp"

namespace YAML_Lib {

class FileSource final : public ISource {
public:
  explicit FileSource(const std::string &filename) : filename(filename) {
    source.open(filename.c_str(), std::ios_base::binary);
    if (!source.is_open()) {
      throw Error("File input stream failed to open or does not exist.");
    }
  }
  FileSource() = delete;
  FileSource(const FileSource &other) = delete;
  FileSource &operator=(const FileSource &other) = delete;
  FileSource(FileSource &&other) = delete;
  FileSource &operator=(FileSource &&other) = delete;
  ~FileSource() override = default;

  char current() const override { return static_cast<char>(source.peek()); }
  void next() override {
    if (current() == kLineFeed) {
      lineNo++;
      column = 1;
    } else {
      column++;
    }
    if (!more()) {
      throw Error("Tried to read past end of file.");
    }
    source.get();
    if (current() == kCarriageReturn) {
      source.get();
      if (current() != kLineFeed) {
        source.unget();
      }
    }
  }
  bool more() const override { return source.peek() != EOF; }
  void reset() override {
    lineNo = 1;
    column = 1;
    source.clear();
    source.seekg(0, std::ios_base::beg);
  }
  std::size_t position() const override {
    if (more()) {
      return source.tellg();
    }
    return std::filesystem::file_size(filename);
  }

  void save() override {
    contexts.emplace_back(lineNo, column, source.tellg());
  }
  void restore() override {
    Context context{contexts.back()};
    contexts.pop_back();
    lineNo = context.lineNo;
    column = context.column;
    source.seekg(context.bufferPosition, std::ios_base::beg);
  }

protected:
  void backup(const unsigned long length) override {
    if (static_cast<long>(column) - length < 1) {
      throw Error("Backup past start column.");
    }
    source.clear();
    source.seekg(-static_cast<long>(length), std::ios_base::cur);
    column -= length;
  }

private:
  mutable std::ifstream source;
  std::string filename;
};
} // namespace YAML_Lib
