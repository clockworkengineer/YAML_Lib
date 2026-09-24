#include "implementation/common/YAML_StringifierFactory.hpp"
#include "implementation/stringify/Default_Stringify.hpp"
#include "implementation/stringify/JSON_Stringify.hpp"
#include "implementation/stringify/XML_Stringify.hpp"
#include "implementation/stringify/Bencode_Stringify.hpp"
#include <algorithm>
#include <cctype>
#include <mutex>
#include <shared_mutex>

namespace YAML_Lib {

StringifierFactory &StringifierFactory::instance() {
  static StringifierFactory factory;
  return factory;
}

StringifierFactory::StringifierFactory() {
  registerDefaults();
}

std::string StringifierFactory::normalizeName(std::string_view name) {
  std::string normalized;
  normalized.reserve(name.size());
  for (char ch : name) {
    normalized.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
  }
  return normalized;
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
  std::unique_lock<std::shared_mutex> lock(mutex);
  creators[format] = std::move(creator);
}

void StringifierFactory::registerCreator(std::string_view formatName, Creator creator) {
  std::unique_lock<std::shared_mutex> lock(mutex);
  namedCreators[normalizeName(formatName)] = std::move(creator);
}

std::unique_ptr<IStringify> StringifierFactory::create(StringifyFormat format) const {
  std::shared_lock<std::shared_mutex> lock(mutex);
  auto it = creators.find(format);
  if (it != creators.end()) {
    return (it->second)();
  }
  throw IStringify::Error("Unsupported StringifyFormat specified.");
}

std::unique_ptr<IStringify> StringifierFactory::create(std::string_view formatName) const {
  std::shared_lock<std::shared_mutex> lock(mutex);
  auto it = namedCreators.find(normalizeName(formatName));
  if (it != namedCreators.end()) {
    return (it->second)();
  }
  throw IStringify::Error("Unsupported format name specified: " + std::string(formatName));
}

} // namespace YAML_Lib
