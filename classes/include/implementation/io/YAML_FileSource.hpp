#pragma once

namespace YAML_Lib {

class FileSource final : public ISource {
public:
  explicit FileSource(const std::string_view &filename) {
    std::ifstream source(filename.data(), std::ios_base::binary | std::ios_base::ate);
    if (!source.is_open()) {
      throw Error("File input stream failed to open or does not exist.");
    }
    const auto size = static_cast<std::streamsize>(source.tellg());
    source.seekg(0, std::ios_base::beg);
    buffer.resize(static_cast<std::size_t>(size));
    source.read(buffer.data(), size);
    // Normalise CR/LF and bare CR → LF so index arithmetic needs no special cases
    std::string normalised;
    normalised.reserve(buffer.size());
    for (std::size_t i = 0; i < buffer.size(); ++i) {
      if (buffer[i] == kCarriageReturn) {
        normalised += kLineFeed;
        if (i + 1 < buffer.size() && buffer[i + 1] == kLineFeed) {
          ++i; // skip the LF of a CRLF pair
        }
      } else {
        normalised += buffer[i];
      }
    }
    buffer = std::move(normalised);
  }
  FileSource() = delete;
  FileSource(const FileSource &other) = delete;
  FileSource &operator=(const FileSource &other) = delete;
  FileSource(FileSource &&other) = delete;
  FileSource &operator=(FileSource &&other) = delete;
  ~FileSource() override = default;

  [[nodiscard]] char current() const override {
    if (more()) {
      return buffer[bufferPosition];
    }
    return EOF;
  }
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
    bufferPosition++;
  }
  [[nodiscard]] bool more() const override {
    return bufferPosition < buffer.size();
  }
  void reset() override {
    bufferPosition = 0;
    lineNo = 1;
    column = 1;
  }
  [[nodiscard]] std::size_t position() override { return bufferPosition; }
  void save() override {
    contexts.push_back(Context(lineNo, column, bufferPosition));
  }
  void restore() override {
    const Context context{contexts.back()};
    contexts.pop_back();
    lineNo = context.lineNo;
    column = context.column;
    bufferPosition = context.bufferPosition;
  }
  void discardSave() override {
    contexts.pop_back();
  }

protected:
  void backup(const unsigned long length) override {
    if (column - length < 1) {
      throw Error("Backup past start column.");
    }
    bufferPosition -= length;
    column -= length;
  }

private:
  std::string buffer;
};
} // namespace YAML_Lib
