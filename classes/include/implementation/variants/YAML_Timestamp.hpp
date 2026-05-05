#pragma once

#ifdef YAML_LIB_TIMESTAMP_PARSE
#include <ctime>
#endif

namespace YAML_Lib {

// =========
// Timestamp
// =========
struct Timestamp {
  // Constructors/Destructors
  Timestamp() = default;
  Timestamp(const Timestamp &other) = default;
  Timestamp &operator=(const Timestamp &other) = default;
  Timestamp(Timestamp &&other) = default;
  Timestamp &operator=(Timestamp &&other) = default;
  ~Timestamp() = default;

  // Construct from string_view — copies into owned storage (both modes).
  explicit Timestamp(const std::string_view &raw) : rawValue(raw) {}

#ifndef YAML_LIB_TIMESTAMP_PARSE
  // OFF mode: move constructor so the parser can hand over the extracted
  // token string without an extra heap allocation.
  explicit Timestamp(std::string &&raw) noexcept : rawValue(std::move(raw)) {}
#endif

  // Return reference to raw timestamp string
  [[nodiscard]] std::string_view value() const { return rawValue; }
  // Return string representation
  [[nodiscard]] std::string toString() const { return rawValue; }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const { return rawValue; }

#ifdef YAML_LIB_TIMESTAMP_PARSE
  // -----------------------------------------------------------------------
  // E8: ISO 8601 → broken-down time / time_t conversion.
  // Supports: YYYY-MM-DD, YYYY-MM-DDThh:mm[:ss], YYYY-MM-DD hh:mm[:ss]
  // Timezone offsets and 'Z' suffix are ignored (local-time semantics via
  // mktime).
  // -----------------------------------------------------------------------
  [[nodiscard]] std::tm toTm() const noexcept {
    std::tm t{};
    const std::string_view sv{rawValue};
    if (sv.size() >= 10) {
      t.tm_year = field(sv, 0, 4) - 1900;
      t.tm_mon  = field(sv, 5, 2) - 1;
      t.tm_mday = field(sv, 8, 2);
    }
    if (sv.size() >= 16 && (sv[10] == 'T' || sv[10] == ' ')) {
      t.tm_hour = field(sv, 11, 2);
      t.tm_min  = field(sv, 14, 2);
    }
    if (sv.size() >= 19 && sv[16] == ':') {
      t.tm_sec = field(sv, 17, 2);
    }
    t.tm_isdst = -1; // let mktime determine DST
    return t;
  }

  [[nodiscard]] std::time_t toTimeT() const noexcept {
    auto t = toTm();
    return std::mktime(&t);
  }
#endif

private:
#ifdef YAML_LIB_TIMESTAMP_PARSE
  static int field(std::string_view sv, std::size_t start,
                   std::size_t len) noexcept {
    int val = 0;
    for (std::size_t i = start; i < start + len && i < sv.size(); ++i) {
      val = val * 10 + static_cast<int>(sv[i] - '0');
    }
    return val;
  }
#endif
  std::string rawValue;
};
} // namespace YAML_Lib
