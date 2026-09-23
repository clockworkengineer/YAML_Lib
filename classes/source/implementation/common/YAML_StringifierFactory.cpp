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

  registerCreator("YAML", []() { return std::make_unique<Default_Stringify>(); });
  registerCreator("JSON", []() { return std::make_unique<JSON_Stringify>(); });
  registerCreator("XML", []() { return std::make_unique<XML_Stringify>(); });
  registerCreator("Bencode", []() { return std::make_unique<Bencode_Stringify>(); });
}

void StringifierFactory::registerCreator(StringifyFormat format, Creator creator) {
  creators[format] = std::move(creator);
}

void StringifierFactory::registerCreator(std::string_view formatName, Creator creator) {
  namedCreators[std::string(formatName)] = std::move(creator);
}

std::unique_ptr<IStringify> StringifierFactory::create(StringifyFormat format) const {
  auto it = creators.find(format);
  if (it != creators.end()) {
    return (it->second)();
  }
  throw IStringify::Error("Unsupported StringifyFormat specified.");
}

std::unique_ptr<IStringify> StringifierFactory::create(std::string_view formatName) const {
  auto it = namedCreators.find(std::string(formatName));
  if (it != namedCreators.end()) {
    return (it->second)();
  }
  throw IStringify::Error("Unsupported format name specified: " + std::string(formatName));
}

} // namespace YAML_Lib
