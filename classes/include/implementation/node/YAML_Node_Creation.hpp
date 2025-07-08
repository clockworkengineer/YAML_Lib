
#pragma once

namespace YAML_Lib {

// Construct Node from raw values
template <typename T> Node::Node(T value) {
  if constexpr (std::is_same_v<T, bool>) {
    *this = Node::make<Boolean>(value, value ? "True" : "False");
  } else if constexpr (std::is_arithmetic_v<T>) {
    *this = Node::make<Number>(value);
  } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
    *this = make<Null>();
  } else if constexpr (std::is_same_v<T, const char *> ||
                       std::is_same_v<T, std::string>) {
    *this = Node::make<String>(value);
  } else if constexpr (std::is_convertible_v<T, std::unique_ptr<Variant>>) {
    yNodeVariant = std::move(value);
  }
}
// Convert an initializer list type to Node
static Node typeToNode(const YAML::InitializerListTypes &type) {
  if (const auto pValue = std::get_if<int>(&type)) {
    return Node(*pValue);
  }
  if (const auto pValue = std::get_if<long>(&type)) {
    return Node(*pValue);
  }
  if (const auto pValue = std::get_if<long long>(&type)) {
    return Node(*pValue);
  }
  if (const auto pValue = std::get_if<float>(&type)) {
    return Node(*pValue);
  }
  if (const auto pValue = std::get_if<double>(&type)) {
    return Node(*pValue);
  }
  if (const auto pValue = std::get_if<long double>(&type)) {
    return Node(*pValue);
  }
  if (const auto pValue = std::get_if<bool>(&type)) {
    return Node(*pValue);
  }
  if (const auto pValue = std::get_if<std::string>(&type)) {
    return Node(*pValue);
  }
  if ([[maybe_unused]] auto pValue = std::get_if<std::nullptr_t>(&type)) {
    return Node(nullptr);
  }
  if (const auto pValue = std::get_if<Node>(&type)) {
    return std::move(*const_cast<Node *>(pValue));
  }
  throw Node::Error("Node for unsupported type could not be created.");
}
// Construct Node Array from the initializer list
inline Node::Node(const YAML::ArrayInitializer &array) {
  *this = make<Array>();
  for (const auto &entry : array) {
    NRef<Array>(*this).add(typeToNode(entry));
  }
}
// Construct Node Dictionary from the initializer list
inline Node::Node(const YAML::DictionaryInitializer &dictionary) {
  *this = make<Dictionary>();
  for (const auto &[fst, snd] : dictionary) {
    NRef<Dictionary>(*this).add(Dictionary::Entry(fst, typeToNode(snd)));
  }
}

} // namespace YAML_Lib
