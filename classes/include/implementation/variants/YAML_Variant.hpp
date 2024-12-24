#pragma once

namespace YAML_Lib {

struct Variant {
  // YNode Variant Types
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
  explicit Variant(const Type nodeType = Type::base, [[maybe_unused]] unsigned long indent = 0)
      : yNodeType(nodeType) {}
  Variant(const Variant &other) = default;
  Variant &operator=(const Variant &other) = default;
  Variant(Variant &&other) = default;
  Variant &operator=(Variant &&other) = default;
  virtual ~Variant() = default;
  // Get BNode type
  [[nodiscard]] Type getNodeType() const { return (yNodeType); }
  // Convert variant to a string
  [[nodiscard]] virtual const std::string toString() const { return ""; }
  // Convert variant to a key
  [[nodiscard]] virtual const std::string toKey() const { return ""; }
private:
  Type yNodeType;
};
} // namespace YAML_Lib