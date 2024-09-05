#pragma once

#include "ISource.hpp"

namespace YAML_Lib {

class BufferSource final : public ISource {
public:
  explicit BufferSource(const std::string &buffer) : buffer(buffer) {
    if (buffer.empty()) {
      throw Error("Empty source buffer passed to be parsed.");
    }
  }
  BufferSource() = delete;
  BufferSource(const BufferSource &other) = delete;
  BufferSource &operator=(const BufferSource &other) = delete;
  BufferSource(BufferSource &&other) = delete;
  BufferSource &operator=(BufferSource &&other) = delete;
  ~BufferSource() override = default;

  [[nodiscard]] char current() const override {
    if (more()) {
      return buffer[bufferPosition];
    }
    return EOF;
  }
  void next() override {
    if (!more()) {
      throw Error("Tried to read past and of buffer.");
    }
    bufferPosition++;
    column++;
    if (current() == kLineFeed) {
      lineNo++;
      column = 1;
    }
  }
  [[nodiscard]] bool more() const override {
    return bufferPosition < buffer.size();
  }
  void reset() override {
    bufferPosition = 0;
    lineNo = 1;
    column = 1;
  }
  [[nodiscard]] std::size_t position() const override { return bufferPosition; }

  void backup(const unsigned long length) override { bufferPosition -= length; column -= length;}

private:
  std::size_t bufferPosition = 0;
  std::string buffer;
};
} // namespace YAML_Lib