
#pragma once

namespace YAML_Lib {

// Dictionary
inline YNode &YNode::operator[](const std::string_view &key) {
  if (isA<Hole>(*this)) {
    *this = make<Dictionary>();
    YRef<Dictionary>(*this).add(Dictionary::Entry(key, make<Hole>()));
    return YRef<Dictionary>(*this).value().back().getYNode();
  }
  return YRef<Dictionary>(*this)[key];
}
inline const YNode &YNode::operator[](const std::string_view &key) const {
  return YRef<const Dictionary>(*this)[key];
}
// Array
inline YNode &YNode::operator[](const std::size_t index) {
  try {
    if (isA<Hole>(*this)) {
      *this = make<Array>();
    }
    if (isA<Array>(*this)) {
      return YRef<Array>(*this)[index];
    }
    if (isA<Document>(*this)) {
      return YRef<Document>(*this)[index];
    }
    throw Error("Not a document or array variant.");
  } catch ([[maybe_unused]] const Error &error) {
    YRef<Array>(*this).resize(index);
    if (isA<Array>(*this)) {
      YRef<Array>(*this).resize(index);
      return YRef<Array>(*this)[index];
    }
    if (isA<Document>(*this)) {
      YRef<Document>(*this).resize(index);
      return YRef<Document>(*this)[index];
    }
    throw Error("Not a document or array variant.");
  }
}
inline const YNode &YNode::operator[](const std::size_t index) const {
  if (isA<Array>(*this)) {
    return YRef<Array>(*this)[index];
  }
  if (isA<Document>(*this)) {
    return YRef<Document>(*this)[index];
  }
  throw Error("Not an array or document to index.");
}

} // namespace YAML_Lib
