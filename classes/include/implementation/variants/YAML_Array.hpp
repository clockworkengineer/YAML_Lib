#pragma once

namespace YAML_Lib {

struct Array final : SequenceBase<Array> {
  // toKey() builds "[a, b, c]" — defined in YAML_Node_Reference.hpp
  // after Node::toString() is fully available.
  [[nodiscard]] std::string toKey() const;
};

} // namespace YAML_Lib