#pragma once

namespace YAML_Lib {

// SequenceBase<Derived> — CRTP base shared by Array and Document.
//
// Array and Document are structurally identical: both wrap a
// std::vector<Node> with add/size/value/operator[]/resize.  The only
// difference is the concrete type (important for std::variant dispatch via
// unique_ptr<Array> vs unique_ptr<Document>) and Array's toKey() output.
//
// forward declarations of Array/Document in YAML_Node.hpp remain valid
// because the derived structs are still real struct types.
template <typename Derived>
struct SequenceBase {
  using Entry   = Node;
  using Entries = std::vector<Entry>;

  SequenceBase() = default;
  SequenceBase(const SequenceBase &) = delete;
  SequenceBase &operator=(const SequenceBase &) = delete;
  SequenceBase(SequenceBase &&) = default;
  SequenceBase &operator=(SequenceBase &&) = default;
  ~SequenceBase() = default;

  void add(Entry yNode) { entries_.emplace_back(std::move(yNode)); }
  [[nodiscard]] std::size_t size() const { return entries_.size(); }
  Entries &value() { return entries_; }
  [[nodiscard]] const Entries &value() const { return entries_; }
  [[nodiscard]] std::string toString() const { return ""; }

  Node &operator[](const std::size_t index) {
    if (index < entries_.size()) {
      return entries_[index];
    }
    throw Node::Error("Invalid index used to access document.");
  }
  const Node &operator[](const std::size_t index) const {
    if (index < entries_.size()) {
      return entries_[index];
    }
    throw Node::Error("Invalid index used to access document.");
  }

  // Defined in YAML_Node_Reference.hpp after Node::make<Hole>() is available.
  void resize(const std::size_t index);

protected:
  Entries entries_;
};

} // namespace YAML_Lib
