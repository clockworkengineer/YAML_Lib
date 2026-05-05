#pragma once

#include <array>
#include <cstddef>
#include <span>

namespace YAML_Lib {

/// StaticSequenceBase<N, Derived> — CRTP base for fixed-capacity sequences.
///
/// Drop-in structural replacement for SequenceBase<Derived> that uses a
/// compile-time-sized std::array<Node, N> instead of std::pmr::vector<Node>.
/// No dynamic allocation; maximum element count is enforced at runtime.
///
/// Intended for Profile B/C embedded targets where the maximum YAML array or
/// document size is known at compile time.
///
/// @tparam N       Maximum number of elements.
/// @tparam Derived Concrete derived type (CRTP).
template <std::size_t N, typename Derived>
struct StaticSequenceBase {
  static_assert(N > 0, "StaticSequenceBase capacity must be greater than zero.");

  using Entry = Node;

  StaticSequenceBase() = default;
  StaticSequenceBase(const StaticSequenceBase &) = delete;
  StaticSequenceBase &operator=(const StaticSequenceBase &) = delete;
  StaticSequenceBase(StaticSequenceBase &&) = default;
  StaticSequenceBase &operator=(StaticSequenceBase &&) = default;
  ~StaticSequenceBase() = default;

  /// Add an element; throws Node::Error if capacity N is exceeded.
  void add(Entry e) {
    if (count_ >= N) {
      YAML_THROW(Node::Error, "Static sequence capacity exceeded.");
    }
    entries_[count_++] = std::move(e);
  }

  /// Number of elements currently stored (not the capacity N).
  [[nodiscard]] std::size_t size() const noexcept { return count_; }

  /// Maximum elements this container can hold.
  [[nodiscard]] static constexpr std::size_t capacity() noexcept { return N; }

  /// Mutable view over the filled portion of the backing array.
  [[nodiscard]] std::span<Entry> value() noexcept {
    return std::span<Entry>{entries_.data(), count_};
  }
  /// Const view over the filled portion of the backing array.
  [[nodiscard]] std::span<const Entry> value() const noexcept {
    return std::span<const Entry>{entries_.data(), count_};
  }

  [[nodiscard]] std::string toString() const { return ""; }

  Entry &operator[](const std::size_t index) {
    if (index < count_) {
      return entries_[index];
    }
    YAML_THROW(Node::Error, "Invalid index used to access static sequence.");
  }
  const Entry &operator[](const std::size_t index) const {
    if (index < count_) {
      return entries_[index];
    }
    YAML_THROW(Node::Error, "Invalid index used to access static sequence.");
  }

  /// Grow to at least (index + 1) elements, filling new slots with Hole nodes.
  /// Body defined in YAML_Node_Reference.hpp after Node::make<Hole>() is available.
  void resize(std::size_t index);

protected:
  std::array<Entry, N> entries_{};
  std::size_t count_{0};
};

} // namespace YAML_Lib
