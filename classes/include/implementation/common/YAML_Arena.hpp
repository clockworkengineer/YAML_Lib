#pragma once

#include <array>
#include <cstddef>
#include <memory_resource>

namespace YAML_Lib {

/// MonotonicArena<N> — fixed-size, stack-backed PMR memory arena.
///
/// Wraps a std::pmr::monotonic_buffer_resource over a std::array<std::byte, N>
/// topped with an unsynchronized_pool_resource that manages a free-list for
/// reuse within the same buffer.
///
/// Usage:
/// @code
///   MonotonicArena<65536> arena;          // 64 KB on the stack (or static)
///   YAML yaml{arena.resource()};
///   yaml.parse(BufferSource{yamlText});   // all nodes allocated from arena
///   // arena freed when it goes out of scope — one bulk deallocation
/// @endcode
///
/// Constraints:
///   - The arena MUST outlive every YAML object that was parsed with it.
///   - parse() is single-threaded with respect to the PMR default resource;
///     do NOT parse from multiple threads using the same MonotonicArena.
///   - Calling parse() again on the same YAML object with the same arena
///     reuses any freed memory in the pool layer.
///
/// @tparam N  Capacity of the backing buffer in bytes.
template <std::size_t N>
class MonotonicArena {
public:
  MonotonicArena() noexcept
      : mbr_{buffer_.data(), buffer_.size(), std::pmr::null_memory_resource()},
        pool_{&mbr_} {}

  MonotonicArena(const MonotonicArena &) = delete;
  MonotonicArena &operator=(const MonotonicArena &) = delete;
  MonotonicArena(MonotonicArena &&) = delete;
  MonotonicArena &operator=(MonotonicArena &&) = delete;

  /// Returns the PMR memory_resource backed by this arena.
  [[nodiscard]] std::pmr::memory_resource *resource() noexcept { return &pool_; }

  /// Maximum capacity of this arena in bytes.
  static constexpr std::size_t capacity() noexcept { return N; }

private:
  alignas(std::max_align_t) std::array<std::byte, N> buffer_{};
  std::pmr::monotonic_buffer_resource mbr_;
  std::pmr::unsynchronized_pool_resource pool_;
};

} // namespace YAML_Lib
