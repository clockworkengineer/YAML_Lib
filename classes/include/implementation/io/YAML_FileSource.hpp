#pragma once

namespace YAML_Lib {

class FileSource final : public ISource {
public:
  explicit FileSource(const std::string_view &filename) : filename(filename) {
    source.open(filename.data(), std::ios_base::binary);
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
      if (more()) {
        source.get();
        if (current() != kLineFeed) {
          source.unget();
        }
      }
    }
    bufferPosition = source.tellg();
  }
  bool more() const override { return source.peek() != EOF; }
  void reset() override {
    lineNo = 1;
    column = 1;
    source.clear();
    source.seekg(0, std::ios_base::beg);
  }
  std::size_t position() override {
    if (more()) {
      bufferPosition = source.tellg();
    } else {
      bufferPosition = std::filesystem::file_size(filename);
    }
    return bufferPosition;
  }
  void save() override {
    bufferPosition = source.tellg();
    contexts.push_back(Context(lineNo, column, bufferPosition));
  }
  void restore() override {
    const Context context{contexts.back()};
    contexts.pop_back();
    lineNo = context.lineNo;
    column = context.column;
    if (source.eof()) {
      source.clear();
    }
    source.seekg(context.bufferPosition, std::ios_base::beg);
    bufferPosition = source.tellg();
  }

protected:
  void backup(const unsigned long length) override {
    if (column - length < 1) {
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
