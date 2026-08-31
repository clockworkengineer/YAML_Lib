#pragma once

#include <memory_resource>
#include <vector>
#include "YAML_Core.hpp"

namespace YAML_Lib {

/**
 * @brief Manages document container storage, indexing, and memory allocation resources.
 *
 * Implements Single Responsibility Principle (SRP) by decoupling document container
 * storage and access boundaries from higher-level parsing, traversal, and file I/O operations.
 */
class DocumentStore {
public:
  DocumentStore(std::pmr::memory_resource *mr = nullptr) : memoryResource(mr) {}
  ~DocumentStore() = default;

  DocumentStore(const DocumentStore &) = delete;
  DocumentStore &operator=(const DocumentStore &) = delete;
  DocumentStore(DocumentStore &&) noexcept = default;
  DocumentStore &operator=(DocumentStore &&) noexcept = default;

  [[nodiscard]] std::size_t size() const noexcept {
    return documents.size();
  }

  [[nodiscard]] bool empty() const noexcept {
    return documents.empty();
  }

  void clear() noexcept {
    documents.clear();
  }

  void setDocuments(std::vector<Node> newDocs) {
    documents = std::move(newDocs);
  }

  void addDocument(Node doc) {
    documents.push_back(std::move(doc));
  }

  [[nodiscard]] std::vector<Node> &getDocuments() noexcept {
    return documents;
  }

  [[nodiscard]] const std::vector<Node> &getDocuments() const noexcept {
    return documents;
  }

  [[nodiscard]] Node &document(const unsigned long index) {
    if (index >= documents.size()) {
      YAML_THROW(Error, "Document does not exist.");
    }
    return documents[index][0];
  }

  [[nodiscard]] const Node &document(const unsigned long index) const {
    if (index >= documents.size()) {
      YAML_THROW(Error, "Document does not exist.");
    }
    return documents[index][0];
  }

  [[nodiscard]] Node &operator[](const std::string_view &key) {
    if (documents.empty()) {
      YAML_THROW(Error, "No documents available.");
    }
    return documents[0][key];
  }

  [[nodiscard]] const Node &operator[](const std::string_view &key) const {
    if (documents.empty()) {
      YAML_THROW(Error, "No documents available.");
    }
    return documents[0][key];
  }

  [[nodiscard]] Node &operator[](std::size_t index) {
    if (documents.empty()) {
      YAML_THROW(Error, "No documents available.");
    }
    return documents[0][index];
  }

  [[nodiscard]] const Node &operator[](std::size_t index) const {
    if (documents.empty()) {
      YAML_THROW(Error, "No documents available.");
    }
    return documents[0][index];
  }

  [[nodiscard]] std::pmr::memory_resource *getMemoryResource() const noexcept {
    return memoryResource;
  }

private:
  std::pmr::memory_resource *memoryResource{nullptr};
  std::vector<Node> documents;
};

} // namespace YAML_Lib
