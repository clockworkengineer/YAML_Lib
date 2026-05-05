#pragma once

namespace YAML_Lib {

// =============================================================================
// BufferedSourceBase — shared implementation for contiguous-buffer ISource types.
//
// Provides concrete implementations of next(), save(), restore(), discardSave(),
// reset(), position(), and backup() for any source backed by a flat byte buffer
// accessed via bufferPosition (index into a contiguous block of memory).
//
// Concrete subclasses only need to supply:
//   - current() const override   — return byte at bufferPosition (or EOF when done)
//   - more()    const override   — return (bufferPosition < bufferSize)
//   - endOfInputMessage() const  — string literal for the "read past end" error
//
// Shared across: BufferSource, SpanSource, FileSource.
// NOT used by: StreamSource (its next()/reset() use std::istream seekg/get).
// =============================================================================
class BufferedSourceBase : public ISource {
public:
  BufferedSourceBase() = default;
  BufferedSourceBase(const BufferedSourceBase &) = delete;
  BufferedSourceBase &operator=(const BufferedSourceBase &) = delete;
  BufferedSourceBase(BufferedSourceBase &&) = delete;
  BufferedSourceBase &operator=(BufferedSourceBase &&) = delete;
  ~BufferedSourceBase() override = default;

  // --------------------------
  // ISource overrides (shared)
  // --------------------------

  void next() override {
    const auto uc = static_cast<unsigned char>(current());
    if (kForbiddenChar[uc]) {
      char buf[5];
      std::snprintf(buf, sizeof(buf), "%04X", static_cast<unsigned>(uc));
      YAML_THROW_POS(*this,
                     std::string("Disallowed control character U+") + buf +
                         " in YAML stream.");
    }
    if (current() == kLineFeed) {
      lineNo++;
      column = 1;
    } else {
      column++;
    }
    if (!more()) {
      YAML_THROW(Error, endOfInputMessage());
    }
    bufferPosition++;
  }

  void reset() override {
    bufferPosition = 0;
    lineNo         = 1;
    column         = 1;
  }

  [[nodiscard]] std::size_t position() override { return bufferPosition; }

  void save() override {
    contexts.push_back(Context(lineNo, column, bufferPosition));
  }

  void restore() override {
    const Context ctx{contexts.back()};
    contexts.pop_back();
    lineNo         = ctx.lineNo;
    column         = ctx.column;
    bufferPosition = ctx.bufferPosition;
  }

  void discardSave() override { contexts.pop_back(); }

protected:
  void backup(const unsigned long length) override {
    if (static_cast<long>(column) - static_cast<long>(length) < 1) {
      YAML_THROW(Error, "Backup past start column.");
    }
    bufferPosition -= length;
    column         -= length;
  }

  /// Subclass supplies the "read past end" error message (string literal).
  [[nodiscard]] virtual const char *endOfInputMessage() const noexcept = 0;
};

} // namespace YAML_Lib
