# Extending YAML_Lib Guide

`YAML_Lib` provides clean extension points via strategy interfaces and registries. This guide demonstrates how to extend the library with custom stringification formats, custom schemas, node factories, and stream sources.

---

## 1. Registering Custom Stringifiers (`StringifierFactory`)

To add a custom output format (for example, TOML or CSV), implement `IStringify` and register a factory creator with `StringifierFactory`:

```cpp
#include "YAML_Writer.hpp"
#include <iostream>

class TOML_Stringify : public YAML_Lib::IStringify {
public:
  std::string stringify(const std::vector<YAML_Lib::Node> &nodes) override {
    std::string result;
    // ... Custom TOML serialization logic ...
    return result;
  }
};

// Register creator strategy
void registerTOML() {
  YAML_Lib::StringifierFactory::instance().registerCreator(
      "TOML", []() { return std::make_unique<TOML_Stringify>(); });
}

int main() {
  registerTOML();
  YAML_Lib::YAML yaml = YAML_Lib::YAML::parse("key: value");
  std::string tomlOutput = yaml.stringify("TOML");
  std::cout << tomlOutput << std::endl;
}
```

---

## 2. Creating Custom Schemas (`ISchema`)

To customize how scalar strings are coerced into node types (e.g. implementing JSON Schema or Failsafe Schema), implement `ISchema`:

```cpp
#include "YAML_Reader.hpp"

class FailsafeSchema : public YAML_Lib::ISchema {
public:
  YAML_Lib::Node resolveScalar(std::string_view scalarText, bool isQuoted) const override {
    // Failsafe Schema resolves all scalars as Strings regardless of content
    return YAML_Lib::Node::make<YAML_Lib::String>(scalarText);
  }

  YAML_Lib::Node resolveTag(std::string_view tagHandle, std::string_view tagSuffix, YAML_Lib::Node node) const override {
    return std::move(node);
  }
};

int main() {
  YAML_Lib::Default_Parser parser(std::make_unique<YAML_Lib::Default_Translator>());
  parser.setSchema(std::make_unique<FailsafeSchema>());

  YAML_Lib::BufferSource source("12345");
  auto nodes = parser.parse(source);
  // nodes[0] is resolved as String("12345") rather than Number
}
```

---

## 3. Custom Node Creation Strategy (`INodeFactory`)

To intercept or customize node allocation/construction, implement `INodeFactory`:

```cpp
#include "YAML_Reader.hpp"
#include "interface/INodeFactory.hpp"
#include "implementation/common/YAML_DefaultNodeFactory.hpp"

class LoggingNodeFactory : public YAML_Lib::DefaultNodeFactory {
public:
  YAML_Lib::Node createString(std::string_view value) const override {
    std::cout << "[Factory] Allocating String node: " << value << "\n";
    return YAML_Lib::DefaultNodeFactory::createString(value);
  }
};
```

---

## 4. Custom Input Sources (`ISource`)

To parse YAML directly from custom data structures or network streams, implement `ISource`:

```cpp
#include "YAML_Reader.hpp"

class CustomMemorySource : public YAML_Lib::ISource {
  std::string_view data_;
  size_t pos_{0};
public:
  explicit CustomMemorySource(std::string_view data) : data_(data) {}

  char get() override { return pos_ < data_.size() ? data_[pos_++] : '\0'; }
  char peek() const override { return pos_ < data_.size() ? data_[pos_] : '\0'; }
  bool eof() const override { return pos_ >= data_.size(); }
  size_t position() const override { return pos_; }
  void seek(size_t pos) override { pos_ = pos; }
};
```
