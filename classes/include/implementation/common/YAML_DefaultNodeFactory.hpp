#pragma once

#include "YAML_Core.hpp"
#include "interface/INodeFactory.hpp"

namespace YAML_Lib {

/**
 * @brief Default Node Factory implementation delegating to Node::make.
 *
 * Implements Dependency Inversion Principle (DIP) and Open/Closed Principle (OCP).
 */
class DefaultNodeFactory : public INodeFactory {
public:
  DefaultNodeFactory() = default;
  ~DefaultNodeFactory() override = default;

  [[nodiscard]] Node createString(std::string_view value) const override {
    return Node::make<String>(value);
  }

  [[nodiscard]] Node createNumber(const Number &value) const override {
    return Node::make<Number>(value);
  }

  [[nodiscard]] Node createNumber(std::string_view value) const override {
    return Node::make<Number>(std::string(value));
  }

  [[nodiscard]] Node createBoolean(bool value, std::string_view representation) const override {
    return Node::make<Boolean>(value, representation);
  }

  [[nodiscard]] Node createNull() const override {
    return Node::make<Null>();
  }

  [[nodiscard]] Node createArray() const override {
    return Node::make<Array>();
  }

  [[nodiscard]] Node createDictionary() const override {
    return Node::make<Dictionary>();
  }

  [[nodiscard]] Node createDocument() const override {
    return Node::make<Document>();
  }

  [[nodiscard]] Node createHole() const override {
    return Node::make<Hole>();
  }

  [[nodiscard]] Node createTimestamp(std::string_view value) const override {
    return Node::make<Timestamp>(value);
  }

  [[nodiscard]] Node createComment(std::string_view value) const override {
    return Node::make<Comment>(value);
  }
};

} // namespace YAML_Lib
