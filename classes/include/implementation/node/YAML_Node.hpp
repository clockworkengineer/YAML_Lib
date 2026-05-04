#pragma once

namespace YAML_Lib {

// Forward declarations for container types (stored via unique_ptr in NodeVariant)
struct Array;
struct Dictionary;
struct Document;

// NodeVariant: scalars stored inline, containers via unique_ptr to avoid
// circular sizeof dependency (Array/Document contain vector<Node>).
using NodeVariant = std::variant<
    std::monostate,            // empty / "hole" sentinel
    Boolean, Null, Number, String, Timestamp, Comment, Hole,
    std::unique_ptr<Array>,
    std::unique_ptr<Dictionary>,
    std::unique_ptr<Document>>;

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
  [[nodiscard]] bool isEmpty() const {
    return std::holds_alternative<std::monostate>(yNodeVariant);
  }
  // Indexing operators
  Node &operator[](const std::string_view &key);
  const Node &operator[](const std::string_view &key) const;
  Node &operator[](std::size_t index);
  const Node &operator[](std::size_t index) const;
  // Get reference to Node variant
  NodeVariant &getVariant() { return yNodeVariant; }
  [[nodiscard]] const NodeVariant &getVariant() const { return yNodeVariant; }
  // Tag access (was on Variant base class; now lives here)
  [[nodiscard]] std::string_view getTag() const { return yamlTag; }
  void setTag(const std::string_view &tag) { yamlTag = std::string(tag); }
  // String conversion helpers (bodies defined in YAML_Node_Reference.hpp)
  [[nodiscard]] std::string toString() const;
  [[nodiscard]] std::string toKey() const;
  // Make Node — scalars stored inline, containers via unique_ptr
  template <typename T, typename... Args> static Node make(Args &&...args) {
    Node n;
    if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dictionary> ||
                  std::is_same_v<T, Document>) {
      n.yNodeVariant = std::make_unique<T>(std::forward<Args>(args)...);
    } else {
      n.yNodeVariant = T(std::forward<Args>(args)...);
    }
    return n;
  }

private:
  NodeVariant yNodeVariant;
  std::string yamlTag;
};
} // namespace YAML_Lib