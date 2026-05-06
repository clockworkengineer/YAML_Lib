#pragma once

namespace YAML_Lib {

// ---------------------------------------------------------------
// Macro: define a simple prefixed Error struct in one line.
// Usage: YAML_MAKE_ERROR(Error, "ISource Error")
// Produces: struct Error final : std::runtime_error { ... }
// Not used for YAML::Error / YAML::SyntaxError which have extra
// constructors (with position pair).
// ---------------------------------------------------------------
#define YAML_MAKE_ERROR(StructName, Prefix)                                    \
  struct StructName final : std::runtime_error {                               \
    explicit StructName(const std::string_view &message)                       \
        : std::runtime_error(std::string(Prefix ": ").append(message)) {}      \
  }

// ---------------------------------------------------------------
// YAML error types (unchanged for backward compatibility)
// ---------------------------------------------------------------
struct Error final : std::runtime_error {
    explicit Error(const std::string_view &message)
        : std::runtime_error(std::string("YAML Error: ").append(message)) {
    }

    explicit Error(const std::pair<unsigned long, unsigned long> &position,
                   const std::string_view &message = "")
        : std::runtime_error(
            std::string("YAML Error [Line: ").append(std::to_string(position.first))
            .append(" Column: ").append(std::to_string(position.second))
            .append("]: ").append(message)) {
    }
};

struct SyntaxError final : std::runtime_error {
    explicit SyntaxError(const std::string_view &message)
        : std::runtime_error(std::string("YAML Syntax Error: ").append(message)) {
    }

    explicit SyntaxError(const std::pair<unsigned long, unsigned long> &position,
                         const std::string_view message = "")
        : std::runtime_error(
            std::string("YAML Syntax Error [Line: ").append(std::to_string(position.first))
            .append(" Column: ").append(std::to_string(position.second))
            .append("]: ").append(message)) {
    }
};

// ---------------------------------------------------------------
// E1: Exception-free error reporting
// ---------------------------------------------------------------
// Type for a user-registered panic handler.  Called with the error message
// and source position (line/column, both 0 when no position is available)
// before the library aborts.  The handler MUST NOT return.
using PanicHandler = void (*)(std::string_view message,
                               unsigned long line,
                               unsigned long column) noexcept;

/// Register a custom panic handler that is called instead of the default
/// stderr dump + std::abort() when YAML_LIB_NO_EXCEPTIONS is active.
/// Safe to call from normal exceptions builds (handler is stored but unused).
void setErrorHandler(PanicHandler handler) noexcept;

/// Return the currently registered panic handler (nullptr = default).
[[nodiscard]] PanicHandler getErrorHandler() noexcept;

#ifdef YAML_LIB_NO_EXCEPTIONS
/// Internal: invoke the panic handler (or default stderr + abort).
/// [[noreturn]] lets the compiler treat code after YAML_THROW as unreachable.
[[noreturn]] void errorPanic(std::string_view message,
                              unsigned long line,
                              unsigned long col) noexcept;
#endif

} // namespace YAML_Lib

// ---------------------------------------------------------------
// YAML_THROW(ExceptionType, message)
//   Drop-in replacement for:  throw ExType(message)
//   Works for Error, SyntaxError, Node::Error, Default_Translator::Error,
//   std::runtime_error, and all interface nested Error types.
//
// YAML_THROW_POS(source_ref, message)
//   Drop-in replacement for:  YAML_THROW(SyntaxError, src.getPosition(), message)
//   Pass *this when the throw site is inside an ISource subclass method.
// ---------------------------------------------------------------
#ifdef YAML_LIB_NO_EXCEPTIONS
#  define YAML_THROW(ExType, msg) \
       ::YAML_Lib::errorPanic((msg), 0UL, 0UL)
#  define YAML_THROW_POS(src, msg) \
       ::YAML_Lib::errorPanic((msg), \
           static_cast<unsigned long>((src).getPosition().first), \
           static_cast<unsigned long>((src).getPosition().second))
#else
#  define YAML_THROW(ExType, msg)     throw ExType(msg)
#  define YAML_THROW_POS(src, msg)    throw SyntaxError((src).getPosition(), (msg))
#endif