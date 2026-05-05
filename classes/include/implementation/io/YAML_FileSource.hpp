#pragma once

namespace YAML_Lib {

class FileSource final : public BufferedSourceBase {
public:
  explicit FileSource(const std::string_view &filename) {
    std::ifstream source(filename.data(), std::ios_base::binary | std::ios_base::ate);
    if (!source.is_open()) {
      YAML_THROW(Error, "File input stream failed to open or does not exist.");
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
  [[nodiscard]] bool more() const override {
    return bufferPosition < buffer.size();
  }

protected:
  [[nodiscard]] const char *endOfInputMessage() const noexcept override {
    return "Tried to read past end of file.";
  }

private:
  std::string buffer;
};
} // namespace YAML_Lib
