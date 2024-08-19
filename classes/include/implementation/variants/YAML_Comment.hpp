#pragma once

namespace YAML_Lib {

struct Comment final : Variant
{
  // Constructors/Destructors
  explicit Comment(std::string comment = "") : Variant(Type::comment), yamlComment(std::move(comment)) {}
  Comment(const Comment &other) = delete;
  Comment &operator=(const Comment &other) = delete;
  Comment(Comment &&other) = default;
  Comment &operator=(Comment &&other) = default;
  ~Comment() = default;
  // Return reference to comment
  [[nodiscard]] const std::string &value() const { return yamlComment; }

private:
  std::string yamlComment;
};
}// namespace YAML_Lib
