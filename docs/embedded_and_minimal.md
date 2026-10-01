# Embedded, Minimal, and Hardened Systems Guide

`YAML_Lib` 1.4.0 is engineered for versatility across a wide spectrum of environments — from full-featured desktop and cloud services to resource-constrained microcontrollers, real-time operating systems (RTOS), and safety-critical embedded targets.

This guide details how to configure, build, and write code for minimal-footprint, hardened, and exception-free environments.

---

## Table of Contents

- [Build Configuration Profiles](#build-configuration-profiles)
- [Exception-Free Programming Model (`YAML_LIB_NO_EXCEPTIONS`)](#exception-free-programming-model-yaml_lib_no_exceptions)
- [Zero-Filesystem In-Memory Environments (`YAML_LIB_FILE_IO=OFF`)](#zero-filesystem-in-memory-environments-yaml_lib_file_iooff)
- [Deterministic Memory Management with PMR](#deterministic-memory-management-with-pmr)
- [Low-Memory Event Streaming with SAX (`YAML_LIB_SAX_API`)](#low-memory-event-streaming-with-sax-yaml_lib_sax_api)
- [Defensive Security Hardening (`Options::secureOptions`)](#defensive-security-hardening-optionssecureoptions)

---

## Build Configuration Profiles

`YAML_Lib` provides modular CMake options allowing you to strip unused subsystems and dependencies:

| CMake Option | Default | Effect when Disabled / Enabled |
| :--- | :--- | :--- |
| `YAML_LIB_NO_EXCEPTIONS` | `OFF` | Enables `-fno-exceptions` / `/EHs-c-`. Replaces `throw` statements with registered panic handlers and non-throwing fallbacks. |
| `YAML_LIB_FILE_IO` | `ON` | Set `OFF` to eliminate `<fstream>` and standard filesystem dependencies. Ideal for bare-metal or sandbox environments. |
| `YAML_LIB_TIMESTAMP_PARSE` | `ON` | Set `OFF` to remove ISO 8601 timestamp regex parsing and time point conversion code. |
| `YAML_LIB_SAX_API` | `OFF` | Set `ON` to enable event-driven visitor traversal (`IYAMLEvents`). |

### Minimal Embedded Profile Example

To build a stripped-down, exception-free static library without file I/O or timestamp parsing:

```sh
cmake -S . -B build_minimal \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_YAML_EXAMPLES=OFF \
  -DBUILD_YAML_TESTS=OFF \
  -DYAML_LIB_NO_EXCEPTIONS=ON \
  -DYAML_LIB_FILE_IO=OFF \
  -DYAML_LIB_TIMESTAMP_PARSE=OFF
cmake --build build_minimal -j$(nproc)
```

Alternatively, use the project helper script:
```sh
./scripts/Linux-Build-Minimal.sh
```

---

## Exception-Free Programming Model (`YAML_LIB_NO_EXCEPTIONS`)

When compiling under `-DYAML_LIB_NO_EXCEPTIONS=ON`:
1. The compiler enforces `-fno-exceptions` (or MSVC equivalent).
2. The library replaces exception throws with calls to the global panic handler.
3. Throwing APIs like `std::expected` parsing (which relies on exceptions in the standard library implementation when converting) are cleanly excluded from compilation.

### 1. Registering a Custom Panic Handler

If an irrecoverable syntax or parse failure occurs and exceptions are disabled, `YAML_Lib` invokes a panic callback before aborting:

```cpp
#include "YAML.hpp"
#include <iostream>

void myEmbeddedPanic(const std::string& message) {
    // Log to UART, write to telemetry buffer, or trigger hardware watchdog reset
    std::cerr << "EMBEDDED PANIC: " << message << "\n";
    // Hardware watchdog reset or graceful halt
    while (true) {}
}

int main() {
    YAML_Lib::Error::setPanicHandler(myEmbeddedPanic);

    // Continue normal initialization...
}
```

### 2. Defensive Type Checking with `.is<T>()` and `.value_or()`

In exception-free code, always use non-throwing type checks and fallback accessors instead of direct assertions:

```cpp
YAML_Lib::BufferSource src("baudrate: 115200\n");
YAML_Lib::YAML yaml;
yaml.parse(src);

// Safe interrogation: check before reading
if (yaml["baudrate"].is<int>()) {
    int baud = yaml["baudrate"].as<int>();
}

// Or provide a safe default fallback:
int timeout = yaml.value_or("timeout_ms", 1000);
```

---

## Zero-Filesystem In-Memory Environments (`YAML_LIB_FILE_IO=OFF`)

On microcontrollers or in sandboxed WebAssembly targets where standard filesystem access (`<fstream>`) is unavailable:
- Configure `-DYAML_LIB_FILE_IO=OFF`.
- Use in-memory buffers: `BufferSource` for reading and `BufferDestination` for writing.

```cpp
#include "YAML.hpp"
#include <iostream>

void processPayload(const uint8_t* flash_data, std::size_t size) {
    // Treat flash memory or network buffer as string_view
    std::string_view yaml_view(reinterpret_cast<const char*>(flash_data), size);

    YAML_Lib::BufferSource source(yaml_view);
    YAML_Lib::YAML doc;
    doc.parse(source);

    // Serialize output to memory buffer
    YAML_Lib::BufferDestination dest;
    doc.stringify(dest);

    std::cout << "Emitted YAML (" << dest.get().size() << " bytes)\n";
}
```

---

## Deterministic Memory Management with PMR

`YAML_Lib` supports `std::pmr::memory_resource` for all internal node and string allocations, eliminating non-deterministic heap fragmentation in long-running embedded tasks.

### Using `std::pmr::monotonic_buffer_resource`

Stack-allocate a buffer for zero-overhead parsing:

```cpp
#include "YAML.hpp"
#include <memory_resource>
#include <array>

void parseOnStack(std::string_view input) {
    // 64 KB static arena on the stack or in dedicated SRAM
    std::array<std::byte, 65536> stack_buffer;
    std::pmr::monotonic_buffer_resource pool(stack_buffer.data(), stack_buffer.size());

    // Pass custom memory resource to YAML
    YAML_Lib::Options opts;
    opts.memory_resource = &pool;

    YAML_Lib::YAML yaml(opts);
    YAML_Lib::BufferSource src(input);
    yaml.parse(src);

    int sensor_id = yaml["sensor"]["id"].as<int>();
    // When pool goes out of scope, all memory is instantly recycled with 0 heap frees
}
```

---

## Low-Memory Event Streaming with SAX (`YAML_LIB_SAX_API`)

For streams too large to hold in RAM simultaneously, configure `-DYAML_LIB_SAX_API=ON` to process nodes sequentially via the visitor pattern without materializing a full DOM tree:

```cpp
#include "YAML.hpp"
#include "interface/IYAMLEvents.hpp"

class StreamLogger : public YAML_Lib::IYAMLEvents {
public:
    void onDocumentStart() override { /* ... */ }
    void onDocumentEnd() override { /* ... */ }
    void onScalar(const std::string& value, const std::string& tag) override {
        // Process scalar on the fly without retaining in RAM
    }
    void onSequenceStart() override {}
    void onSequenceEnd() override {}
    void onMappingStart() override {}
    void onMappingEnd() override {}
};
```

---

## Defensive Security Hardening (`Options::secureOptions`)

When receiving untrusted input over public interfaces or network endpoints, use `Options::secureOptions()` to enforce resource ceilings and prevent DoS attacks:

```cpp
YAML_Lib::Options secure = YAML_Lib::Options::secureOptions();
// Default secure protections:
// - strict_booleans = true (prevents 1.1 'yes'/'no' ambiguity)
// - max_documents = 1 (prevents document flooding)
// - max_parse_depth = 64 (prevents call stack exhaustion)
// - max_alias_expansions = 64 (prevents Billion Laughs attacks)
// - max_scalar_length = 64 KB (prevents buffer bloat)
// - max_collection_size = 1024

YAML_Lib::YAML parser(secure);
```
