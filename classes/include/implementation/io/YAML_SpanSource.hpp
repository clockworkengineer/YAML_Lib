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
class SpanSource final : public BufferedSourceBase {
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
  [[nodiscard]] bool more() const override { return bufferPosition < len_; }

protected:
  [[nodiscard]] const char *endOfInputMessage() const noexcept override {
    return "Tried to read past end of span.";
  }

private:
  const char *data_;
  std::size_t len_;
};

} // namespace YAML_Lib
