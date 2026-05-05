#pragma once

#include <cstddef>
#include <span>

namespace YAML_Lib {

/// SpanSource — zero-copy, zero-allocation ISource backed by a raw char span.
///
/// Points directly into a caller-supplied buffer (ROM, flash, stack, or heap).
/// No copy is made; the buffer MUST outlive this SpanSource object.
///
/// Unlike BufferSource, SpanSource:
///   - Accepts `const char *` + length or a `std::span<const char>` — both
///     work with ROM constants on Harvard-architecture MCUs.
///   - Does not own a std::string, so it pulls no heap allocation of its own.
///   - The inherited save()/restore() context stack uses the std::vector<Context>
///     in ISource; for Profile C (no heap) avoid deep nesting or use SourceGuard
///     only where strictly necessary.
///
/// Usage:
/// @code
///   static constexpr char kConfigYaml[] = "key: value\n";
///   SpanSource src{kConfigYaml, sizeof(kConfigYaml) - 1};
///   yaml.parse(src);
/// @endcode
class SpanSource final : public ISource {
public:
  /// Construct from a raw pointer and byte count.
  /// The pointer may be ROM; it is never written.
  SpanSource(const char *data, std::size_t len) noexcept
      : data_{data}, len_{len} {}

  /// Construct from a span (e.g. std::span{array}).
  explicit SpanSource(std::span<const char> span) noexcept
      : data_{span.data()}, len_{span.size()} {}

  SpanSource() = delete;
  SpanSource(const SpanSource &) = delete;
  SpanSource &operator=(const SpanSource &) = delete;
  SpanSource(SpanSource &&) = delete;
  SpanSource &operator=(SpanSource &&) = delete;
  ~SpanSource() override = default;

  [[nodiscard]] char current() const override {
    if (more()) {
      return data_[bufferPosition];
    }
    return static_cast<char>(EOF);
  }

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
      YAML_THROW(Error, "Tried to read past end of span.");
    }
    bufferPosition++;
  }

  [[nodiscard]] bool more() const override { return bufferPosition < len_; }

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
    const Context ctx{contexts.back()};
    contexts.pop_back();
    lineNo = ctx.lineNo;
    column = ctx.column;
    bufferPosition = ctx.bufferPosition;
  }

  void discardSave() override { contexts.pop_back(); }

protected:
  void backup(const unsigned long length) override {
    if (static_cast<long>(column) - static_cast<long>(length) < 1) {
      YAML_THROW(Error, "Backup past start column.");
    }
    bufferPosition -= length;
    column -= length;
  }

private:
  const char *data_;
  std::size_t len_;
};

} // namespace YAML_Lib
