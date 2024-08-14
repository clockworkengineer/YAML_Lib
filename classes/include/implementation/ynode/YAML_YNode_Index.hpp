
#pragma once

namespace YAML_Lib {

// Object
inline YNode &YNode::operator[](const std::string &key)
{
  if (isA<Hole>(*this)) {
    *this = make<Object>();
    YRef<Object>(*this).add(Object::Entry(key, make<Hole>()));
    return YRef<Object>(*this).value().back().getYNode();
  }
  return YRef<Object>(*this)[key];
}
inline const YNode &YNode::operator[](const std::string &key) const { return YRef<const Object>(*this)[key]; }
// Array
inline YNode &YNode::operator[](const std::size_t index)
{
  try {
    if (isA<Hole>(*this)) { *this = make<Array>(); }
    return YRef<Array>(*this)[index];
  } catch ([[maybe_unused]] const Error &error) {
    YRef<Array>(*this).resize(index);
    return YRef<Array>(*this)[index];
  }
}
inline const YNode &YNode::operator[](const std::size_t index) const { return YRef<Array>(*this)[index]; }

}// namespace YAML_Lib
