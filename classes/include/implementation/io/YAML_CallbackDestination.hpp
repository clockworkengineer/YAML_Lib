#pragma once

#include <cstddef>

namespace YAML_Lib {

/// CallbackDestination — streaming IDestination that forwards every character
/// to a caller-supplied callback function.
///
/// No buffer is maintained; each character is delivered immediately to the
/// sink.  This enables streaming stringify output directly to a UART, serial
/// port, or any other character-oriented sink without any intermediate buffer.
///
/// The Sink function uses a plain C function pointer (no std::function) to
/// avoid type-erasure overhead on embedded targets.  A opaque context pointer
/// is threaded through so that the sink can access caller state without
/// needing a global or a closure.
///
/// Usage:
/// @code
///   // Sink that writes to a UART (MCU pseudo-code)
///   void uart_sink(char ch, void *) { UART_TDR = ch; }
///
///   CallbackDestination dest{uart_sink, nullptr};
///   yaml.stringify(dest);   // characters streamed to UART as they are produced
/// @endcode
class CallbackDestination final : public IDestination {
public:
  /// Plain C function pointer: receives one character and an opaque context.
  using Sink = void (*)(char ch, void *ctx);

  /// @param sink  Character sink; must not be nullptr.
  /// @param ctx   Opaque pointer forwarded verbatim to every sink call.
  CallbackDestination(Sink sink, void *ctx) noexcept
      : sink_{sink}, ctx_{ctx} {}

  CallbackDestination() = delete;
  CallbackDestination(const CallbackDestination &) = delete;
  CallbackDestination &operator=(const CallbackDestination &) = delete;
  CallbackDestination(CallbackDestination &&) = delete;
  CallbackDestination &operator=(CallbackDestination &&) = delete;
  ~CallbackDestination() override = default;

  void add(char ch) override {
    sink_(ch, ctx_);
    last_ = ch;
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

  /// No-op: a streaming destination has no persistent buffer to clear.
  void clear() override {
    last_ = kNull;
  }

  [[nodiscard]] char last() override { return last_; }

private:
  Sink  sink_;
  void *ctx_;
  char  last_{kNull};
};

} // namespace YAML_Lib
