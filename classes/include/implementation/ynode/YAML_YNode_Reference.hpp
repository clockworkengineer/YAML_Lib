#pragma once

namespace YAML_Lib {

// =======================
// What is YNode variant ?
// =======================
template<typename T> bool isA(const YNode &yNode)
{
  if constexpr (std::is_same_v<T, String>) {
    return yNode.getVariant().getNodeType() == Variant::Type::string;
  } else if constexpr (std::is_same_v<T, Number>) {
    return yNode.getVariant().getNodeType() == Variant::Type::number;
  } else if constexpr (std::is_same_v<T, Array>) {
    return yNode.getVariant().getNodeType() == Variant::Type::array;
  } else if constexpr (std::is_same_v<T, Object>) {
    return yNode.getVariant().getNodeType() == Variant::Type::object;
  } else if constexpr (std::is_same_v<T, Boolean>) {
    return yNode.getVariant().getNodeType() == Variant::Type::boolean;
  } else if constexpr (std::is_same_v<T, Null>) {
    return yNode.getVariant().getNodeType() == Variant::Type::null;
  } else if constexpr (std::is_same_v<T, Hole>) {
    return yNode.getVariant().getNodeType() == Variant::Type::hole;
  } else {
    return false;
  }
}

// =========================
// YNode reference converter
// =========================
template<typename T> void checkYNode(const YNode &yNode)
{
  if constexpr (std::is_same_v<T, String>) {
    if (!isA<T>(yNode)) { throw YNode::Error("YNode not a string."); }
  } else if constexpr (std::is_same_v<T, Number>) {
    if (!isA<T>(yNode)) { throw YNode::Error("YNode not a number."); }
  } else if constexpr (std::is_same_v<T, Array>) {
    if (!isA<T>(yNode)) { throw YNode::Error("YNode not an array."); }
  } else if constexpr (std::is_same_v<T, Object>) {
    if (!isA<T>(yNode)) { throw YNode::Error("YNode not an object."); }
  } else if constexpr (std::is_same_v<T, Boolean>) {
    if (!isA<T>(yNode)) { throw YNode::Error("YNode not a boolean."); }
  } else if constexpr (std::is_same_v<T, Null>) {
    if (!isA<T>(yNode)) { throw YNode::Error("YNode not a null."); }
  }else if constexpr (std::is_same_v<T, Hole>) {
    if (!isA<T>(yNode)) { throw YNode::Error("YNode not a hole."); }
  }
}
template<typename T> T &YRef(YNode &yNode)
{
  checkYNode<T>(yNode);
  return static_cast<T &>(yNode.getVariant());
}
template<typename T> const T &YRef(const YNode &yNode)
{
  checkYNode<T>(yNode);
  return static_cast<const T &>(yNode.getVariant());
}
template<typename T> T &YRef(Object::Entry &yNodeEntry)
{
  checkYNode<T>(yNodeEntry.getYNode());
  return static_cast<T &>(yNodeEntry.getYNode().getVariant());
}
template<typename T> const T &YRef(const Object::Entry &yNodeEntry)
{
  checkYNode<T>(yNodeEntry.getYNode());
  return static_cast<const T &>(yNodeEntry.getYNode().getVariant());
}
}// namespace YAML_Lib