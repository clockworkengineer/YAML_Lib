#pragma once

namespace YAML_Lib {

// =========
// Timestamp
// =========
struct Timestamp {
  // Constructors/Destructors
  Timestamp() = default;
  explicit Timestamp(const std::string_view &raw)
      : rawValue(std::string(raw)) {}
  Timestamp(const Timestamp &other) = default;
  Timestamp &operator=(const Timestamp &other) = default;
  Timestamp(Timestamp &&other) = default;
  Timestamp &operator=(Timestamp &&other) = default;
  ~Timestamp() = default;
  // Return reference to raw timestamp string
  [[nodiscard]] std::string_view value() const { return rawValue; }
  // Return string representation
  [[nodiscard]] std::string toString() const { return rawValue; }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const { return rawValue; }

private:
  std::string rawValue;
};
} // namespace YAML_Lib
