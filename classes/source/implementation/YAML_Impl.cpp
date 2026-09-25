
//
// Class: YAML_Impl
//
// Description: High level entry points in YAML class implementation layer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"
#include <mutex>

namespace YAML_Lib {

YAML_Impl::YAML_Impl(IStringify* stringify, IParser* parser, std::pmr::memory_resource* mr)
    : memoryResource{mr}, documentStore{mr} {
  if (parser == nullptr) {
    defaultParser = std::make_unique<Default_Parser>(std::make_unique<Default_Translator>());
    yamlParser = defaultParser.get();
  } else {
    ownedParser.reset(parser);
    yamlParser = ownedParser.get();
  }
  if (stringify == nullptr) {
    defaultStringify = std::make_unique<Default_Stringify>(std::make_unique<Default_Translator>());
    yamlStringify = defaultStringify.get();
  } else {
    ownedStringify.reset(stringify);
    yamlStringify = ownedStringify.get();
  }
}

YAML_Impl::YAML_Impl(const Options& options)
    : memoryResource{options.memory_resource}, documentStore{options.memory_resource} {
  options.validate();
  Default_Parser::setStrictBooleans(options.strict_booleans);

  if (options.parser == nullptr) {
    defaultParser =
        std::make_unique<Default_Parser>(std::make_unique<Default_Translator>(), options);
    yamlParser = defaultParser.get();
  } else if (options.own_parser) {
    ownedParser.reset(options.parser);
    yamlParser = ownedParser.get();
  } else {
    yamlParser = options.parser;
  }
  if (options.stringifier == nullptr) {
    defaultStringify = std::make_unique<Default_Stringify>(std::make_unique<Default_Translator>());
    yamlStringify = defaultStringify.get();
  } else if (options.own_stringifier) {
    ownedStringify.reset(options.stringifier);
    yamlStringify = ownedStringify.get();
  } else {
    yamlStringify = options.stringifier;
  }
}

YAML_Impl::YAML_Impl(YAML_Impl&& other) noexcept
    : memoryResource(other.memoryResource),
      defaultParser(std::move(other.defaultParser)),
      defaultStringify(std::move(other.defaultStringify)),
      ownedParser(std::move(other.ownedParser)),
      ownedStringify(std::move(other.ownedStringify)),
      documentStore(std::move(other.documentStore)) {
  if (defaultParser) {
    yamlParser = defaultParser.get();
  } else if (ownedParser) {
    yamlParser = ownedParser.get();
  } else {
    yamlParser = other.yamlParser;
  }
  if (defaultStringify) {
    yamlStringify = defaultStringify.get();
  } else if (ownedStringify) {
    yamlStringify = ownedStringify.get();
  } else {
    yamlStringify = other.yamlStringify;
  }
  other.yamlParser = nullptr;
  other.yamlStringify = nullptr;
  other.memoryResource = nullptr;
}

YAML_Impl& YAML_Impl::operator=(YAML_Impl&& other) noexcept {
  if (this != &other) {
    memoryResource = other.memoryResource;
    defaultParser = std::move(other.defaultParser);
    defaultStringify = std::move(other.defaultStringify);
    ownedParser = std::move(other.ownedParser);
    ownedStringify = std::move(other.ownedStringify);
    documentStore = std::move(other.documentStore);
    if (defaultParser) {
      yamlParser = defaultParser.get();
    } else if (ownedParser) {
      yamlParser = ownedParser.get();
    } else {
      yamlParser = other.yamlParser;
    }
    if (defaultStringify) {
      yamlStringify = defaultStringify.get();
    } else if (ownedStringify) {
      yamlStringify = ownedStringify.get();
    } else {
      yamlStringify = other.yamlStringify;
    }
    other.yamlParser = nullptr;
    other.yamlStringify = nullptr;
    other.memoryResource = nullptr;
  }
  return *this;
}

std::unique_ptr<YAML_Impl> YAML_Impl::clone() const {
  auto copy = std::make_unique<YAML_Impl>(nullptr, nullptr, memoryResource);
  std::vector<Node> clonedDocs;
  clonedDocs.reserve(documentStore.size());
  for (const auto& doc : documentStore.getDocuments()) {
    clonedDocs.push_back(doc.clone());
  }
  copy->documentStore.setDocuments(std::move(clonedDocs));
  return copy;
}

/// <summary>
/// Function header.
/// </summary>
std::string YAML_Impl::version() {
  std::stringstream versionString;
  versionString << "YAML_Lib Version  " << YAML_VERSION_MAJOR << "." << YAML_VERSION_MINOR << "."
                << YAML_VERSION_PATCH;
  return versionString.str();
}

static std::mutex s_pmrDefaultMutex;

/// <summary>
/// Function header.
/// </summary>
void YAML_Impl::parse(ISource& source) {
  // RAII guard: if the caller supplied a PMR resource, install it as the PMR
  // default for the duration of parse so that all std::pmr::* containers
  // created during parse (Array/Document entries, Dictionary entries/index)
  // draw from that resource. The previous default is restored on scope exit.
  // Thread safety: guarded by s_pmrDefaultMutex while active.
  std::unique_lock<std::mutex> lock(s_pmrDefaultMutex, std::defer_lock);
  if (memoryResource != nullptr) {
    lock.lock();
  }
  struct ResourceScope {
    std::pmr::memory_resource* prev_;
    const bool active_;
    explicit ResourceScope(std::pmr::memory_resource* mr)
        : prev_{mr ? std::pmr::get_default_resource() : nullptr}, active_{mr != nullptr} {
      if (active_) std::pmr::set_default_resource(mr);
    }
    ~ResourceScope() {
      if (active_) std::pmr::set_default_resource(prev_);
    }
  } scope{memoryResource};
  documentStore.setDocuments(yamlParser->parse(source));
}

}  // namespace YAML_Lib
