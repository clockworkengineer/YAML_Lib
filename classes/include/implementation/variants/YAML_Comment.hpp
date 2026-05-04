#pragma once

namespace YAML_Lib {

struct Comment {
  // Constructors/Destructors
  explicit Comment(const std::string_view &comment = "")
      : yamlComment(std::string(comment)) {}
  Comment(const Comment &other) = default;
  Comment &operator=(const Comment &other) = default;
  Comment(Comment &&other) = default;
  Comment &operator=(Comment &&other) = default;
  ~Comment() = default;
  // Return reference to comment
  [[nodiscard]] std::string_view value() const { return yamlComment; }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return "# " + yamlComment; }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const { return ""; }

private:
  std::string yamlComment;
};
} // namespace YAML_Lib
