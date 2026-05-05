#pragma once

namespace YAML_Lib {

/// StaticArray<N> — fixed-capacity replacement for Array.
///
/// Uses StaticSequenceBase<N, StaticArray<N>> instead of SequenceBase<Array>,
/// so all N element slots live in the containing struct with no heap allocation.
///
/// @tparam N  Maximum number of array elements.
template <std::size_t N>
struct StaticArray final : StaticSequenceBase<N, StaticArray<N>> {
  // toKey() builds "[a, b, c]" — same logic as Array::toKey().
  // Defined in YAML_Node_Reference.hpp after Node::toString() is available.
  [[nodiscard]] std::string toKey() const;

  /// Override operator[] to emit the "array" diagnostic on out-of-range.
  using StaticSequenceBase<N, StaticArray<N>>::operator[];
  Node &operator[](const std::size_t index) {
    if (index < this->count_) {
      return this->entries_[index];
    }
    YAML_THROW(Node::Error, "Invalid index used to access static array.");
  }
  const Node &operator[](const std::size_t index) const {
    if (index < this->count_) {
      return this->entries_[index];
    }
    YAML_THROW(Node::Error, "Invalid index used to access static array.");
  }
};

/// StaticDocument<N> — fixed-capacity replacement for Document.
///
/// @tparam N  Maximum number of top-level nodes in the document.
template <std::size_t N>
struct StaticDocument final : StaticSequenceBase<N, StaticDocument<N>> {
  [[nodiscard]] std::string toKey() const { return ""; }
};

} // namespace YAML_Lib
