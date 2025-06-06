#pragma once

namespace YAML_Lib {

class FileDestination final : public IDestination {
public:
  explicit FileDestination(const std::string_view &filename) : filename(filename) {
    destination.open(filename.data(),
                     std::ios_base::binary | std::ios_base::trunc);
  }
  FileDestination() = delete;
  FileDestination(const FileDestination &other) = delete;
  FileDestination &operator=(const FileDestination &other) = delete;
  FileDestination(FileDestination &&other) = delete;
  FileDestination &operator=(FileDestination &&other) = delete;
  ~FileDestination() override = default;

  void add(const char ch) override {
    if (ch == kLineFeed) {
      destination.write("\r\n", 2);
      fileSize += 2;
    } else {
      destination.put(ch);
      fileSize++;
    }
    lastChar = ch;
  }
  void add(const std::string &bytes) override {
    for (const auto ch : bytes) {
      add(ch);
    }
    lastChar = bytes.back();
  }
  void add(const char *bytes) override {
    auto  len = strlen(bytes);
    for (std::size_t  index=0; index<len; index++) { add(bytes[index]); }
    destination.flush();
    lastChar = bytes[len - 1];
  }
  void add( const std::string_view &bytes) override {
    for (const auto ch : bytes) { add(ch); }
    destination.flush();
    lastChar = bytes.back();
  }
  void clear() override {
    if (destination.is_open()) {
      destination.close();
    }
    destination.open(filename.c_str(),
                     std::ios_base::binary | std::ios_base::trunc);
    if (!destination.is_open()) {
      throw Error("File output stream failed to open or could not be created.");
    }
    fileSize = 0;
    lastChar = 0;
  }
  void close() { destination.flush(); }

  [[nodiscard]] std::size_t size() const { return fileSize; }

  [[nodiscard]] char last() override { return lastChar; }

private:
  std::ofstream destination;
  std::string filename;
  std::size_t fileSize{};
  char lastChar{};
};
} // namespace YAML_Lib
