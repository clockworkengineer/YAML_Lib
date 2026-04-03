#pragma once

namespace YAML_Lib {

struct Variant {
  // Node Variant Types
  enum class Type : uint8_t {
    base = 0,
    dictionary,
    array,
    number,
    string,
    boolean,
    null,
    hole,
    comment,
    document
  };
  // Constructors/Destructors
  explicit Variant(const Type nodeType = Type::base) : yNodeType(nodeType) {}
  Variant(const Variant &other) = default;
  Variant &operator=(const Variant &other) = default;
  Variant(Variant &&other) = default;
  Variant &operator=(Variant &&other) = default;
  virtual ~Variant() = default;
  // Get BNode type
  [[nodiscard]] Type getNodeType() const { return yNodeType; }
  // Convert variant to a string
  [[nodiscard]] virtual std::string toString() const { return ""; }
  // Convert variant to a key
  [[nodiscard]] virtual std::string toKey() const { return ""; }
  // Get/set YAML tag (e.g. "!!str", "!!int", "!custom")
  [[nodiscard]] std::string_view getTag() const { return yamlTag; }
  void setTag(const std::string_view &tag) { yamlTag = std::string(tag); }

private:
  Type yNodeType;
  std::string yamlTag;
};
} // namespace YAML_Lib