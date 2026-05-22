#pragma once

#include <cstring>
#include <ostream>

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
    if (!stream) {
      YAML_THROW(IDestination::Error, "Failed to write to output stream.");
    }
    lastChar = ch;
  }
  void add(const std::string &bytes) override {
    if (bytes.empty()) {
      return;
    }
    stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!stream) {
      YAML_THROW(IDestination::Error, "Failed to write to output stream.");
    }
    lastChar = bytes.back();
  }
  void add(const char *bytes) override {
    if (bytes != nullptr && bytes[0] != '\0') {
      const std::size_t length = std::strlen(bytes);
      stream.write(bytes, static_cast<std::streamsize>(length));
      if (!stream) {
        YAML_THROW(IDestination::Error, "Failed to write to output stream.");
      }
      lastChar = bytes[length - 1];
    }
  }
  void add(const std::string_view &bytes) override {
    if (bytes.empty()) {
      return;
    }
    stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!stream) {
      YAML_THROW(IDestination::Error, "Failed to write to output stream.");
    }
    lastChar = bytes.back();
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
