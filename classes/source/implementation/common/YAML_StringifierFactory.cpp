#include "implementation/common/YAML_StringifierFactory.hpp"
#include "implementation/stringify/Default_Stringify.hpp"
#include "implementation/stringify/JSON_Stringify.hpp"
#include "implementation/stringify/XML_Stringify.hpp"
#include "implementation/stringify/Bencode_Stringify.hpp"

namespace YAML_Lib {

StringifierFactory &StringifierFactory::instance() {
  static StringifierFactory factory;
  return factory;
}

StringifierFactory::StringifierFactory() {
  registerDefaults();
}

void StringifierFactory::registerDefaults() {
  registerCreator(StringifyFormat::YAML, []() {
    return std::make_unique<Default_Stringify>();
  });
  registerCreator(StringifyFormat::JSON, []() {
    return std::make_unique<JSON_Stringify>();
  });
  registerCreator(StringifyFormat::XML, []() {
    return std::make_unique<XML_Stringify>();
  });
  registerCreator(StringifyFormat::Bencode, []() {
    return std::make_unique<Bencode_Stringify>();
  });
}

void StringifierFactory::registerCreator(StringifyFormat format, Creator creator) {
  creators[format] = std::move(creator);
}

std::unique_ptr<IStringify> StringifierFactory::create(StringifyFormat format) const {
  auto it = creators.find(format);
  if (it != creators.end()) {
    return (it->second)();
  }
  throw IStringify::Error("Unsupported StringifyFormat specified.");
}

} // namespace YAML_Lib
