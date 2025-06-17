#pragma once

namespace YAML_Lib {

struct Comment final : Variant {
  // Constructors/Destructors
  explicit Comment(const std::string_view & comment = "")
      : Variant(Type::comment), yamlComment(std::move(comment)) {}
  Comment(const Comment &other) = default;
  Comment &operator=(const Comment &other) = default;
  Comment(Comment &&other) = default;
  Comment &operator=(Comment &&other) = default;
  ~Comment() override = default;
  // Return reference to comment
  [[nodiscard]] std::string_view value() const { return yamlComment; }
  // Return string representation of value
  [[nodiscard]]  std::string toString() const override {
    return "# " + yamlComment;
  }

private:
  std::string yamlComment;
};
} // namespace YAML_Lib
