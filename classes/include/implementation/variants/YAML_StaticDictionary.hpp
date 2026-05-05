#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace YAML_Lib {

/// StaticDictionary<N> — fixed-capacity replacement for Dictionary.
///
/// Stores up to N key/value pairs in three parallel std::array members
/// (keys, values, key-quotes) — no std::unordered_map, no heap index.
/// Key lookup is O(N) linear scan; acceptable for small N (≤ 32 entries
/// fit entirely in cache on any modern or mid-range MCU).
///
/// Intended for Profile B/C embedded targets where the YAML schema has a
/// known, bounded number of keys per dictionary.
///
/// @tparam N  Maximum number of dictionary entries.
template <std::size_t N>
struct StaticDictionary {
  static_assert(N > 0, "StaticDictionary capacity must be greater than zero.");

  StaticDictionary() = default;
  StaticDictionary(const StaticDictionary &) = delete;
  StaticDictionary &operator=(const StaticDictionary &) = delete;
  StaticDictionary(StaticDictionary &&) = default;
  StaticDictionary &operator=(StaticDictionary &&) = default;
  ~StaticDictionary() = default;

  /// Add a key/value entry; throws Node::Error if capacity N is exceeded.
  /// Duplicate keys are accepted (last write wins on operator[]).
  void add(const std::string_view &key, Node &&node,
           char quote = kNull) {
    if (count_ >= N) {
      YAML_THROW(Node::Error, "Static dictionary capacity exceeded.");
    }
    keys_[count_]   = std::string{key};
    quotes_[count_] = quote;
    values_[count_] = std::move(node);
    ++count_;
  }

  /// Returns true if the dictionary contains the given key (O(N) scan).
  [[nodiscard]] bool contains(const std::string_view &key) const noexcept {
    return findIndex(key) < count_;
  }

  /// Number of entries currently stored.
  [[nodiscard]] int size() const noexcept {
    return static_cast<int>(count_);
  }

  /// Maximum entries this dictionary can hold.
  [[nodiscard]] static constexpr std::size_t capacity() noexcept { return N; }

  /// Mutable access by key; throws Node::Error if key is not found.
  Node &operator[](const std::string_view &key) {
    const std::size_t i = findIndex(key);
    if (i >= count_) {
      YAML_THROW(Node::Error, "Invalid key used to access static dictionary.");
    }
    return values_[i];
  }
  /// Const access by key; throws Node::Error if key is not found.
  const Node &operator[](const std::string_view &key) const {
    const std::size_t i = findIndex(key);
    if (i >= count_) {
      YAML_THROW(Node::Error, "Invalid key used to access static dictionary.");
    }
    return values_[i];
  }

  /// Iterate over all filled entries.
  /// @param fn  Callable with signature (string_view key, const Node &value, char quote)
  template <typename Fn>
  void forEach(Fn &&fn) const {
    for (std::size_t i = 0; i < count_; ++i) {
      fn(std::string_view{keys_[i]}, values_[i], quotes_[i]);
    }
  }
  /// Mutable variant of forEach.
  template <typename Fn>
  void forEach(Fn &&fn) {
    for (std::size_t i = 0; i < count_; ++i) {
      fn(std::string_view{keys_[i]}, values_[i], quotes_[i]);
    }
  }

  [[nodiscard]] std::string toString() const { return ""; }
  [[nodiscard]] std::string toKey() const;  // defined in YAML_Node_Reference.hpp

private:
  [[nodiscard]] std::size_t findIndex(const std::string_view &key) const noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
      if (keys_[i] == key) {
        return i;
      }
    }
    return count_; // sentinel: not found
  }

  std::array<std::string, N> keys_{};
  std::array<char, N>        quotes_{};
  std::array<Node, N>        values_{};
  std::size_t                count_{0};
};

} // namespace YAML_Lib
