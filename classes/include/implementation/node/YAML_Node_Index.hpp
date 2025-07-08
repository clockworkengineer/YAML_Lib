
#pragma once

namespace YAML_Lib {

// Dictionary
inline Node &Node::operator[](const std::string_view &key) {
  if (isA<Hole>(*this)) {
    *this = make<Dictionary>();
    NRef<Dictionary>(*this).add(Dictionary::Entry(key, make<Hole>()));
    return NRef<Dictionary>(*this).value().back().getNode();
  }
  return NRef<Dictionary>(*this)[key];
}
inline const Node &Node::operator[](const std::string_view &key) const {
  return NRef<const Dictionary>(*this)[key];
}
// Array
inline Node &Node::operator[](const std::size_t index) {
  try {
    if (isA<Hole>(*this)) {
      *this = make<Array>();
    }
    if (isA<Array>(*this)) {
      return NRef<Array>(*this)[index];
    }
    if (isA<Document>(*this)) {
      return NRef<Document>(*this)[index];
    }
    throw Error("Not a document or array variant.");
  } catch ([[maybe_unused]] const Error &error) {
    NRef<Array>(*this).resize(index);
    if (isA<Array>(*this)) {
      NRef<Array>(*this).resize(index);
      return NRef<Array>(*this)[index];
    }
    if (isA<Document>(*this)) {
      NRef<Document>(*this).resize(index);
      return NRef<Document>(*this)[index];
    }
    throw Error("Not a document or array variant.");
  }
}
inline const Node &Node::operator[](const std::size_t index) const {
  if (isA<Array>(*this)) {
    return NRef<Array>(*this)[index];
  }
  if (isA<Document>(*this)) {
    return NRef<Document>(*this)[index];
  }
  throw Error("Not an array or document to index.");
}

} // namespace YAML_Lib
