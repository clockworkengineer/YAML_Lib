#pragma once

namespace YAML_Lib {

struct Node {
  // Node Error
  struct Error final : std::runtime_error {
    explicit Error(const std::string_view &message)
        : std::runtime_error(std::string("Node Error: ").append(message)) {}
  };
  // Constructors/Destructors
  Node() = default;
  template <typename T> explicit Node(T value);
  Node(const YAML::ArrayInitializer &array);
  Node(const YAML::DictionaryInitializer &dictionary);
  Node(const Node &other) = delete;
  Node &operator=(const Node &other) = delete;
  Node(Node &&other) = default;
  Node &operator=(Node &&other) = default;
  ~Node() = default;
  // Assignment operators
  template <typename T> Node &operator=(T value) {
    return *this = Node(value);
  }
  // Has the variant been created?
  [[nodiscard]] bool isEmpty() const { return yNodeVariant == nullptr; }
  // Indexing operators
  Node &operator[](const std::string_view &key);
  const Node &operator[](const std::string_view &key) const;
  Node &operator[](std::size_t index);
  const Node &operator[](std::size_t index) const;
  // Get reference to Node variant
  Variant &getVariant() { return *yNodeVariant; }
  [[nodiscard]] const Variant &getVariant() const { return *yNodeVariant; }
  // Make Node
  template <typename T, typename... Args> static auto make(Args &&...args) {
    return Node{std::make_unique<T>(std::forward<Args>(args)...)};
  }

private:
  std::unique_ptr<Variant> yNodeVariant;
};
} // namespace YAML_Lib