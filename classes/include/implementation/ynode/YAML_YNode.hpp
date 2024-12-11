#pragma once

namespace YAML_Lib {

struct YNode
{
  // YNode Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("YNode Error: " + message) {}
  };
  // Constructors/Destructors
  YNode() = default;
  template<typename T> explicit YNode(T value);
  YNode(const YAML::ArrayInitializer &array);
  YNode(const YAML::Dictionaryintializer &dictionary);
  YNode(const YNode &other) = delete;
  YNode &operator=(const YNode &other) = delete;
  YNode(YNode &&other) = default;
  YNode &operator=(YNode &&other) = default;
  ~YNode() = default;
  // Assignment operators
  template<typename T> YNode &operator=(T value) { return *this = YNode(value); }
  // Has the variant been created?
  [[nodiscard]] bool isEmpty() const { return yNodeVariant == nullptr; }
  // Indexing operators
  YNode &operator[](const std::string &key);
  const YNode &operator[](const std::string &key) const;
  YNode &operator[](std::size_t index);
  const YNode &operator[](std::size_t index) const;
  // Get reference to YNode variant
  Variant &getVariant() { return *yNodeVariant; }
  [[nodiscard]] const Variant &getVariant() const { return *yNodeVariant; }
  // Make YNode
  template<typename T, typename... Args> static auto make(Args &&...args)
  {
    return YNode{ std::make_unique<T>(std::forward<Args>(args)...) };
  }

private:
  std::unique_ptr<Variant> yNodeVariant;
};
}// namespace YAML_Lib