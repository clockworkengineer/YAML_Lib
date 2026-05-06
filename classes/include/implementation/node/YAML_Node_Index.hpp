
#pragma once

namespace YAML_Lib {

// Dictionary
inline Node &Node::operator[](const std::string_view &key) {
  if (isA<Hole>(*this)) {
    *this = make<Dictionary>();
  }
  if (isA<Dictionary>(*this)) {
    auto &dictionary = NRef<Dictionary>(*this);
    if (dictionary.contains(key)) {
      return dictionary[key];
    }
    dictionary.add(Dictionary::Entry(key, make<Hole>()));
    return dictionary[key];
  }
  YAML_THROW(Error, "Not a dictionary variant.");
}
inline const Node &Node::operator[](const std::string_view &key) const {
  return NRef<Dictionary>(*this)[key];
}
// Array
inline Node &Node::operator[](const std::size_t index) {
  if (isA<Hole>(*this)) {
    *this = make<Array>();
  }
  if (isA<Array>(*this)) {
    auto &array = NRef<Array>(*this);
    if (index >= array.size()) {
      array.resize(index);
    }
    return array[index];
  }
  if (isA<Document>(*this)) {
    auto &document = NRef<Document>(*this);
    if (index >= document.size()) {
      document.resize(index);
    }
    return document[index];
  }
  YAML_THROW(Error, "Not a document or array variant.");
}
inline const Node &Node::operator[](const std::size_t index) const {
  if (isA<Array>(*this)) {
    return NRef<Array>(*this)[index];
  }
  if (isA<Document>(*this)) {
    return NRef<Document>(*this)[index];
  }
  YAML_THROW(Error, "Not an array or document to index.");
}

} // namespace YAML_Lib
