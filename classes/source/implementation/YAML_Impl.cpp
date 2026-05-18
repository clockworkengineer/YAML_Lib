
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YAML_Impl::YAML_Impl(IStringify *stringify, IParser *parser,
                     std::pmr::memory_resource *mr)
    : memoryResource{mr} {
  if (parser == nullptr) {
    yamlParser = std::make_unique<Default_Parser>(std::make_unique<Default_Translator>());
  } else {
    yamlParser.reset(parser);
  }
  if (stringify == nullptr) {
    yamlStringify = std::make_unique<Default_Stringify>(std::make_unique<Default_Translator>());
  } else {
    yamlStringify.reset(stringify);
  }
}

YAML_Impl::YAML_Impl(const Options &options)
    : memoryResource{options.memory_resource} {
  Default_Parser::setStrictBooleans(options.strict_booleans);

  if (options.parser == nullptr) {
    yamlParser = std::make_unique<Default_Parser>(
        std::make_unique<Default_Translator>(), options);
  } else {
    yamlParser.reset(options.parser);
  }
  if (options.stringifier == nullptr) {
    yamlStringify = std::make_unique<Default_Stringify>(std::make_unique<Default_Translator>());
  } else {
    yamlStringify.reset(options.stringifier);
  }
}

std::string YAML_Impl::version() {
  std::stringstream versionString;
  versionString << "YAML_Lib Version  " << YAML_VERSION_MAJOR << "."
                << YAML_VERSION_MINOR << "." << YAML_VERSION_PATCH;
  return versionString.str();
}

void YAML_Impl::parse(ISource &source) {
  // RAII guard: if the caller supplied a PMR resource, install it as the PMR
  // default for the duration of parse so that all std::pmr::* containers
  // created during parse (Array/Document entries, Dictionary entries/index)
  // draw from that resource. The previous default is restored on scope exit.
  // NOTE: this modifies the process-wide PMR default; single-threaded use only.
  struct ResourceScope {
    std::pmr::memory_resource *prev_;
    const bool active_;
    explicit ResourceScope(std::pmr::memory_resource *mr)
        : prev_{mr ? std::pmr::get_default_resource() : nullptr},
          active_{mr != nullptr} {
      if (active_) std::pmr::set_default_resource(mr);
    }
    ~ResourceScope() {
      if (active_) std::pmr::set_default_resource(prev_);
    }
  } scope{memoryResource};
  yamlTree = yamlParser->parse(source);
}

} // namespace YAML_Lib
