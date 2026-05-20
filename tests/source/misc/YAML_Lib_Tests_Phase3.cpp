#include "YAML_Lib_Tests.hpp"
#include "implementation/common/YAML_Arena.hpp"
#include <cstdio>

using namespace YAML_Lib;

namespace {
void customPanicHandler(std::string_view, unsigned long, unsigned long) noexcept {}

struct CustomDestination : IDestination {
  std::string output;
  void add(char ch) override { output.push_back(ch); }
  void clear() override { output.clear(); }
  char last() override { return output.empty() ? '\0' : output.back(); }
};

struct PrefixStringify : IStringify {
  void stringify(const Node &yNode, IDestination &destination, unsigned long) const override {
    destination.add('[');
    if (isA<Dictionary>(yNode)) {
      destination.add("DICT]");
    } else if (isA<Array>(yNode)) {
      destination.add("ARRAY]");
    } else {
      destination.add("NODE]");
    }
  }
};

struct CountingResource : std::pmr::memory_resource {
  std::pmr::memory_resource *const upstream;
  std::size_t allocations{0};
  std::size_t bytes{0};

  explicit CountingResource(std::pmr::memory_resource *upstreamResource =
                                std::pmr::get_default_resource())
      : upstream(upstreamResource) {}

private:
  void *do_allocate(std::size_t bytesRequested,
                     std::size_t alignment) override {
    ++allocations;
    bytes += bytesRequested;
    return upstream->allocate(bytesRequested, alignment);
  }

  void do_deallocate(void *p, std::size_t bytesRequested,
                     std::size_t alignment) override {
    upstream->deallocate(p, bytesRequested, alignment);
  }

  bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
    return this == &other;
  }
};

struct ConstantParser : IParser {
  std::vector<Node> parse(ISource &source) override {
    (void)source;
    std::vector<Node> documents;
    documents.emplace_back(Node{{{"custom", "parser"}}});
    return documents;
  }
};
} // namespace

TEST_CASE("Missing root dictionary key creates an entry safely", "[YAML][API][Index]") {
  YAML yaml;
  yaml["new_key"] = "value";

  REQUIRE(isA<Dictionary>(yaml.document(0)));
  REQUIRE(NRef<Dictionary>(yaml.document(0)).contains("new_key"));
  REQUIRE(isA<String>(yaml.document(0)["new_key"]));
  REQUIRE(NRef<String>(yaml.document(0)["new_key"]).value() == "value");
}

TEST_CASE("Root numeric index access grows an array without exceptions", "[YAML][API][Index]") {
  YAML yaml;
  yaml[2] = "third";

  REQUIRE(isA<Array>(yaml.document(0)));
  REQUIRE(NRef<Array>(yaml.document(0)).size() == 3);
  REQUIRE(isA<String>(yaml.document(0)[2]));
  REQUIRE(NRef<String>(yaml.document(0)[2]).value() == "third");
}

TEST_CASE("YAML::Options supports strict boolean parsing and memory resources", "[YAML][Options][API]") {
  Options options;
  options.strict_booleans = true;
  options.memory_resource = std::pmr::get_default_resource();

  YAML yaml(options);
  yaml.parse(BufferSource{"---\nvalue: yes\n"});

  REQUIRE(isA<String>(yaml.document(0)["value"]));
  REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "yes");
}

TEST_CASE("YAML parse uses configured PMR allocations", "[YAML][Performance][PMR]") {
  CountingResource countResource;
  Options options;
  options.memory_resource = &countResource;

  YAML yaml(options);
  yaml.parse(BufferSource{"---\nvalue: test\n"});

  REQUIRE(countResource.allocations > 0);
  REQUIRE(isA<String>(yaml.document(0)["value"]));
  REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "test");
}

TEST_CASE("YAML constructor accepts a PMR memory resource", "[YAML][Performance][PMR]") {
  MonotonicArena<16384> arena;
  YAML yaml(arena.resource());
  yaml.parse(BufferSource{"---\nvalue: pmr\n"});

  REQUIRE(isA<String>(yaml.document(0)["value"]));
  REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "pmr");
}

TEST_CASE("YAML::Options supports custom parser and stringifier implementations", "[YAML][Options][Customize]") {
  Options options;
  options.parser = new ConstantParser();
  options.stringifier = makeStringify<PrefixStringify>();

  YAML yaml(options);
  yaml.parse(BufferSource{"---\nignored: yes\n"});

  CustomDestination destination;
  yaml.stringify(destination);
  REQUIRE(destination.output == "[DICT]");
}

TEST_CASE("Error handler registration is preserved for no-exceptions builds", "[YAML][NoExceptions]") {
  setErrorHandler(customPanicHandler);
  REQUIRE(getErrorHandler() == customPanicHandler);
}

#ifdef YAML_LIB_FILE_IO
TEST_CASE("File I/O API is available when YAML_LIB_FILE_IO is enabled", "[YAML][FileIO][Feature]") {
  const std::string expected = "---\nname: Alice\n";
  const std::string fileName = "./test_phase3_file_io.yaml";

  YAML::toFile(fileName, expected, YAML::Format::utf8);
  REQUIRE(YAML::fromFile(fileName) == expected);

  std::remove(fileName.c_str());
}
#endif
