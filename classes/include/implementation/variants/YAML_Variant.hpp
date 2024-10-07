#pragma once

namespace YAML_Lib {

struct Variant
{
  // YNode Variant Types
  enum class Type : uint8_t { base = 0, dictionary, array, number, string, boolean, null, hole, comment,document, anchor, alias };
  // Constructors/Destructors
  explicit Variant(const Type nodeType = Type::base, unsigned long indent=0) : yNodeType(nodeType), indentation(indent) {}
  Variant(const Variant &other) = default;
  Variant &operator=(const Variant &other) = default;
  Variant(Variant &&other) = default;
  Variant &operator=(Variant &&other) = default;
  ~Variant() = default;
  // Get BNode type
  [[nodiscard]] Type getNodeType() const {
    return(yNodeType);
  }
  // Get/Set indentation
  [[nodiscard]] unsigned long getIndentation() const { return indentation; }
  void setIndentation(unsigned long indent) { this->indentation = indent; }
private:
  Type yNodeType;
  // Array indentation
  unsigned long indentation {};
};
}// namespace YAML_Lib