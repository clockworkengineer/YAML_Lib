#pragma once

namespace YAML_Lib {

// ======================================================================
// StreamSource — ISource backed by any seekable std::istream.
//
// Allows parsing YAML from std::istringstream, std::ifstream, or any
// other seekable C++ input stream.  Requires the stream to support
// seekg/tellg (e.g. std::istringstream, std::ifstream opened in binary
// mode).  Non-seekable streams such as std::cin are not supported
// because save()/restore()/backup() require random access.
//
// Usage:
//   std::istringstream ss{"key: value\n"};
//   yaml.parse(StreamSource{ss});
// ======================================================================
class StreamSource final : public ISource {
public:
  explicit StreamSource(std::istream &stream) : stream(stream) {
    if (!stream.good()) {
      YAML_THROW(Error, "Stream is not in a good state.");
    }
  }
  StreamSource() = delete;
  StreamSource(const StreamSource &) = delete;
  StreamSource &operator=(const StreamSource &) = delete;
  StreamSource(StreamSource &&) = delete;
  StreamSource &operator=(StreamSource &&) = delete;
  ~StreamSource() override = default;

  [[nodiscard]] char current() const override {
    return static_cast<char>(stream.peek());
  }

  void next() override {
    if (current() == kLineFeed) {
      lineNo++;
      column = 1;
    } else {
      column++;
    }
    if (!more()) {
      YAML_THROW(Error, "Tried to read past end of stream.");
    }
    stream.get();
    // Capture the stream position BEFORE current() (peek()) can set eofbit.
    // Once peek() triggers EOF the stream returns tellg() == -1.
    {
      const auto pos = stream.tellg();
      if (pos != static_cast<std::streampos>(-1)) {
        bufferPosition = static_cast<std::size_t>(pos);
      }
    }
    if (current() == kCarriageReturn) {
      if (more()) {
        stream.get();
        if (current() != kLineFeed) {
          stream.unget();
        }
      }
    }
  }

  [[nodiscard]] bool more() const override { return stream.peek() != EOF; }

  void reset() override {
    lineNo = 1;
    column = 1;
    stream.clear();
    stream.seekg(0, std::ios_base::beg);
    bufferPosition = 0;
  }

  [[nodiscard]] std::size_t position() override {
    if (more()) {
      bufferPosition = static_cast<std::size_t>(stream.tellg());
    }
    return bufferPosition;
  }

  void save() override {
    bufferPosition = static_cast<std::size_t>(stream.tellg());
    contexts.push_back(Context(lineNo, column, bufferPosition));
  }

  void restore() override {
    const Context context{contexts.back()};
    contexts.pop_back();
    lineNo = context.lineNo;
    column = context.column;
    if (stream.eof()) {
      stream.clear();
    }
    stream.seekg(static_cast<std::streamoff>(context.bufferPosition),
                 std::ios_base::beg);
    bufferPosition = static_cast<std::size_t>(stream.tellg());
  }
  void discardSave() override {
    contexts.pop_back();
  }

protected:
  void backup(const unsigned long length) override {
    if (column - length < 1) {
      YAML_THROW(Error, "Backup past start column.");
    }
    stream.clear();
    stream.seekg(-static_cast<long>(length), std::ios_base::cur);
    column -= length;
  }

private:
  std::istream &stream;
};

} // namespace YAML_Lib
