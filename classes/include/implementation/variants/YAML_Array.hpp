#pragma once

namespace YAML_Lib {

struct Array final : SequenceBase<Array> {
  // toKey() builds "[a, b, c]" — defined in YAML_Node_Reference.hpp
  // after Node::toString() is fully available.
  [[nodiscard]] std::string toKey() const;
  // Override operator[] to produce the expected "array" error message.
  Node &operator[](const std::size_t index) {
    if (index < entries_.size()) { return entries_[index]; }
    YAML_THROW(Node::Error, "Invalid index used to access array.");
  }
  const Node &operator[](const std::size_t index) const {
    if (index < entries_.size()) { return entries_[index]; }
    YAML_THROW(Node::Error, "Invalid index used to access array.");
  }
};

} // namespace YAML_Lib