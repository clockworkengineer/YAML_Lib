#include "YAML_Impl.hpp"

#ifdef YAML_LIB_NO_EXCEPTIONS
#  include <cstdio>
#  include <cstdlib>
#endif

namespace YAML_Lib {

namespace {
  // Registered panic handler (nullptr = use default stderr + abort).
  PanicHandler g_errorHandler = nullptr;
}

void setErrorHandler(PanicHandler handler) noexcept {
  g_errorHandler = handler;
}

PanicHandler getErrorHandler() noexcept {
  return g_errorHandler;
}

#ifdef YAML_LIB_NO_EXCEPTIONS
[[noreturn]] void errorPanic(std::string_view message,
                              unsigned long line,
                              unsigned long col) noexcept {
  if (g_errorHandler != nullptr) {
    g_errorHandler(message, line, col);
  } else {
    if (line != 0) {
      std::fprintf(stderr,
                   "YAML_Lib fatal error [Line: %lu Column: %lu]: %.*s\n",
                   line, col,
                   static_cast<int>(message.size()), message.data());
    } else {
      std::fprintf(stderr, "YAML_Lib fatal error: %.*s\n",
                   static_cast<int>(message.size()), message.data());
    }
  }
  std::abort();
}
#endif

} // namespace YAML_Lib
