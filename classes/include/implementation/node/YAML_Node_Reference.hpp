#pragma once

namespace YAML_Lib {

// =======================
// What is Node variant ?
// =======================
template <typename T> bool isA(const Node &yNode) {
  if constexpr (std::is_same_v<T, String>) {
    return yNode.getVariant().getNodeType() == Variant::Type::string;
  } else if constexpr (std::is_same_v<T, Number>) {
    return yNode.getVariant().getNodeType() == Variant::Type::number;
  } else if constexpr (std::is_same_v<T, Array>) {
    return yNode.getVariant().getNodeType() == Variant::Type::array;
  } else if constexpr (std::is_same_v<T, Dictionary>) {
    return yNode.getVariant().getNodeType() == Variant::Type::dictionary;
  } else if constexpr (std::is_same_v<T, Boolean>) {
    return yNode.getVariant().getNodeType() == Variant::Type::boolean;
  } else if constexpr (std::is_same_v<T, Null>) {
    return yNode.getVariant().getNodeType() == Variant::Type::null;
  } else if constexpr (std::is_same_v<T, Hole>) {
    return yNode.getVariant().getNodeType() == Variant::Type::hole;
  } else if constexpr (std::is_same_v<T, Comment>) {
    return yNode.getVariant().getNodeType() == Variant::Type::comment;
  } else if constexpr (std::is_same_v<T, Document>) {
    return yNode.getVariant().getNodeType() == Variant::Type::document;
  } else {
    return false;
  }
}

// =========================
// Node reference converter
// =========================
template <typename T> void checkNode(const Node &yNode) {
  if constexpr (std::is_same_v<T, String>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not a string.");
    }
  } else if constexpr (std::is_same_v<T, Number>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not a number.");
    }
  } else if constexpr (std::is_same_v<T, Array>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not an array.");
    }
  } else if constexpr (std::is_same_v<T, Dictionary>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not an dictionary.");
    }
  } else if constexpr (std::is_same_v<T, Boolean>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not a boolean.");
    }
  } else if constexpr (std::is_same_v<T, Null>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not a null.");
    }
  } else if constexpr (std::is_same_v<T, Hole>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not a hole.");
    }
  } else if constexpr (std::is_same_v<T, Comment>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not a comment.");
    }
  } else if constexpr (std::is_same_v<T, Document>) {
    if (!isA<T>(yNode)) {
      throw Node::Error("Node not a document.");
    }
  }
}
template <typename T> T &NRef(Node &yNode) {
  checkNode<T>(yNode);
  return static_cast<T &>(yNode.getVariant());
}
template <typename T> const T &NRef(const Node &yNode) {
  checkNode<T>(yNode);
  return static_cast<const T &>(yNode.getVariant());
}
template <typename T> T &NRef(Dictionary::Entry &yNodeEntry) {
  checkNode<T>(yNodeEntry.getNode());
  return static_cast<T &>(yNodeEntry.getNode().getVariant());
}
template <typename T> const T &NRef(const Dictionary::Entry &yNodeEntry) {
  checkNode<T>(yNodeEntry.getNode());
  return static_cast<const T &>(yNodeEntry.getNode().getVariant());
}
} // namespace YAML_Lib