
#pragma once

namespace YAML_Lib {

// Construct YNode from raw values
template<typename T> YNode::YNode(T value)
{
  if constexpr (std::is_same_v<T, bool>) {
    *this = YNode::make<Boolean>(value);
  } else if constexpr (std::is_arithmetic_v<T>) {
    *this = YNode::make<Number>(value);
  } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
    *this = make<Null>();
  } else if constexpr (std::is_same_v<T, const char *> || std::is_same_v<T, std::string>) {
    *this = YNode::make<String>(value);
  } else if constexpr (std::is_convertible_v<T, std::unique_ptr<Variant>>) {
    yNodeVariant = std::move(value);
  }
}
// Convert initializer list type to JMode
inline static YNode typeToYNode(const YAML::IntializerListTypes &type)
{
  if (const auto pValue = std::get_if<int>(&type)) { return YNode(*pValue); }
  if (const auto pValue = std::get_if<long>(&type)) { return YNode(*pValue); }
  if (const auto pValue = std::get_if<long long>(&type)) { return YNode(*pValue); }
  if (const auto pValue = std::get_if<float>(&type)) { return YNode(*pValue); }
  if (const auto pValue = std::get_if<double>(&type)) { return YNode(*pValue); }
  if (const auto pValue = std::get_if<long double>(&type)) { return YNode(*pValue); }
  if (const auto pValue = std::get_if<bool>(&type)) { return YNode(*pValue); }
  if (const auto pValue = std::get_if<std::string>(&type)) { return YNode(*pValue); }
  if ([[maybe_unused]] auto pValue = std::get_if<std::nullptr_t>(&type)) { return YNode(nullptr); }
  if (const auto pValue = std::get_if<YNode>(&type)) { return std::move(*const_cast<YNode *>(pValue)); }
  throw YNode::Error("YNode for unsupported type could not be created.");
}
// Construct YNode Array from initializer list
inline YNode::YNode(const YAML::ArrayInitializer &array)
{
  *this = make<Array>();
  for (const auto &entry : array) { YRef<Array>(*this).add(typeToYNode(entry)); }
}
// Construct YNode Dictionary from initializer list
inline YNode::YNode(const YAML::Dictionaryintializer &dictionary)
{
  *this = make<Dictionary>();
  for (const auto &[fst, snd] : dictionary) { YRef<Dictionary>(*this).add(Dictionary::Entry(fst, typeToYNode(snd))); }
}

}// namespace YAML_Lib
