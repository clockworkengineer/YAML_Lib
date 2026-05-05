#pragma once

#include <array>
#include <cstddef>
#include <cstring>

namespace YAML_Lib {

/// FixedDestination<N> — bounded, heap-free IDestination backed by a
/// caller-supplied char array.
///
/// Writes stringified YAML into a fixed-size buffer whose address is supplied
/// at construction time. The buffer is typically a stack array or a static
/// array — no heap allocation is involved.
///
/// When the buffer is full further writes are silently discarded; call
/// overflow() to detect truncation after stringify completes.
///
/// @tparam N  Maximum number of bytes (including any null-terminator the
///            caller may choose to add after stringify).
///
/// Usage:
/// @code
///   char buf[512];
///   FixedDestination<512> dest{buf};
///   yaml.stringify(dest);
///   if (dest.overflow()) { /* buffer too small */ }
///   buf[dest.size()] = '\0';   // null-terminate manually if desired
///   uart_write(buf, dest.size());
/// @endcode
template <std::size_t N>
class FixedDestination final : public IDestination {
public:
  static_assert(N > 0, "FixedDestination capacity must be greater than zero.");

  /// Bind to an existing char array.  The array MUST outlive this object.
  explicit FixedDestination(char (&buf)[N]) noexcept : buf_{buf} {}

  FixedDestination() = delete;
  FixedDestination(const FixedDestination &) = delete;
  FixedDestination &operator=(const FixedDestination &) = delete;
  FixedDestination(FixedDestination &&) = delete;
  FixedDestination &operator=(FixedDestination &&) = delete;
  ~FixedDestination() override = default;

  void add(char ch) override {
    if (pos_ < N) {
      buf_[pos_++] = ch;
    } else {
      overflow_ = true;
    }
  }

  void add(const std::string_view &bytes) override {
    for (const char ch : bytes) {
      add(ch);
    }
  }

  void add(const std::string &bytes) override {
    for (const char ch : bytes) {
      add(ch);
    }
  }

  void add(const char *bytes) override {
    while (*bytes) {
      add(*bytes++);
    }
  }

  void clear() override {
    pos_ = 0;
    overflow_ = false;
  }

  [[nodiscard]] char last() override {
    return pos_ > 0 ? buf_[pos_ - 1] : kNull;
  }

  /// Number of bytes written so far (not counting any trailing null).
  [[nodiscard]] std::size_t size() const noexcept { return pos_; }

  /// True if at least one byte was discarded because the buffer was full.
  [[nodiscard]] bool overflow() const noexcept { return overflow_; }

  /// Maximum capacity in bytes.
  [[nodiscard]] static constexpr std::size_t capacity() noexcept { return N; }

  /// View over the bytes written so far.
  [[nodiscard]] std::string_view view() const noexcept {
    return std::string_view{buf_, pos_};
  }

private:
  char       *buf_;
  std::size_t pos_{0};
  bool        overflow_{false};
};

} // namespace YAML_Lib
