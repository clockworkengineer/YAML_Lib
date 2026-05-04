#pragma once

namespace YAML_Lib {

struct Document final : SequenceBase<Document> {
  // Documents don't have a meaningful key representation.
  [[nodiscard]] std::string toKey() const { return ""; }
};

} // namespace YAML_Lib