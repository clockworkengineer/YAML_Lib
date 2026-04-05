#pragma once

namespace YAML_Lib {

// ======================================================================
// StreamDestination — IDestination backed by any std::ostream.
//
// Allows stringifying YAML to std::cout, std::ostringstream,
// std::ofstream, or any other C++ output stream.
//
// Usage:
//   std::ostringstream ss;
//   yaml.stringify(StreamDestination{ss});
//   std::cout << ss.str();
//
//   yaml.stringify(StreamDestination{std::cout});
// ======================================================================
class StreamDestination final : public IDestination {
public:
  explicit StreamDestination(std::ostream &stream) : stream(stream) {}
  StreamDestination() = delete;
  StreamDestination(const StreamDestination &) = delete;
  StreamDestination &operator=(const StreamDestination &) = delete;
  StreamDestination(StreamDestination &&) = delete;
  StreamDestination &operator=(StreamDestination &&) = delete;
  ~StreamDestination() override = default;

  void add(const char ch) override {
    stream.put(ch);
    lastChar = ch;
  }
  void add(const std::string &bytes) override {
    stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!bytes.empty()) {
      lastChar = bytes.back();
    }
  }
  void add(const char *bytes) override {
    if (bytes != nullptr && bytes[0] != '\0') {
      stream.write(bytes, static_cast<std::streamsize>(std::strlen(bytes)));
      lastChar = bytes[std::strlen(bytes) - 1];
    }
  }
  void add(const std::string_view &bytes) override {
    stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!bytes.empty()) {
      lastChar = bytes.back();
    }
  }
  void clear() override {
    // Streams cannot have data removed once written; just reset the
    // last character tracker so callers get a consistent sentinel.
    lastChar = kNull;
  }
  [[nodiscard]] char last() override { return lastChar; }

private:
  std::ostream &stream;
  char lastChar{kNull};
};

} // namespace YAML_Lib
