#include "YAML_Lib_Tests.hpp"
#include "JSON_Stringify.hpp"

using namespace YAML_Lib;

TEST_CASE("YAML::Options validate rejects unsafe runtime settings", "[YAML][Options][Validation]") {
  Options options;
  options.max_documents = 1000001;
  REQUIRE_THROWS_AS(options.validate(), std::invalid_argument);

  options.max_documents = 1;
  options.max_parse_depth = 1000001;
  REQUIRE_THROWS_AS(options.validate(), std::invalid_argument);

  options.max_parse_depth = 64;
  options.max_alias_expansions = 1000001;
  REQUIRE_THROWS_AS(options.validate(), std::invalid_argument);
}

TEST_CASE("YAML::Options can install a JSON stringifier", "[YAML][Options][Customize][Stringify]") {
  Options options;
  options.stringifier = makeStringify<JSON_Stringify>();

  YAML yaml(options);
  yaml.parse(BufferSource{"---\nkey: value\n"});

  BufferDestination dest;
  yaml.stringify(dest);

  const std::string output = dest.toString();
  REQUIRE(output.find('{') != std::string::npos);
  REQUIRE(output.find("\"key\"") != std::string::npos);
}

TEST_CASE("Custom ISource and IDestination integrate in a lightweight end-to-end test", "[YAML][Testability][Integration]") {
  struct MemorySource : ISource {
    explicit MemorySource(std::string text) : buffer(std::move(text)) {}

    char current() const override { return buffer[position_]; }
    void next() override {
      if (!more()) {
        throw ISource::Error("MemorySource: read past end");
      }
      if (buffer[position_] == '\n') {
        lineNo++;
        column = 1;
      } else {
        column++;
      }
      position_++;
    }
    bool more() const override { return position_ < buffer.size(); }
    void reset() override { position_ = 0; lineNo = 1; column = 1; }
    std::size_t position() override { return position_; }
    void save() override { contexts.emplace_back(lineNo, column, position_); }
    void restore() override {
      if (contexts.empty()) {
        throw ISource::Error("MemorySource::restore() without save");
      }
      const auto context = contexts.back();
      contexts.pop_back();
      lineNo = context.lineNo;
      column = context.column;
      position_ = context.bufferPosition;
    }
    void discardSave() override {
      if (contexts.empty()) {
        throw ISource::Error("MemorySource::discardSave() without save");
      }
      contexts.pop_back();
    }

  private:
    void backup(unsigned long length) override {
      if (length > position_) {
        throw ISource::Error("MemorySource::backup() beyond start");
      }
      position_ -= length;
      lineNo = 1;
      column = 1;
      for (std::size_t i = 0; i < position_; ++i) {
        if (buffer[i] == '\n') {
          lineNo++;
          column = 1;
        } else {
          column++;
        }
      }
    }

    std::string buffer;
    std::size_t position_{};
    std::vector<Context> contexts;
  };

  struct MemoryDestination : IDestination {
    void add(char ch) override { output.push_back(ch); }
    void clear() override { output.clear(); }
    char last() override { return output.empty() ? '\0' : output.back(); }
    std::string output;
  };

  MemorySource source("---\nname: test\n");
  YAML yaml;
  yaml.parse(source);

  MemoryDestination dest;
  yaml.stringify(dest);

  REQUIRE(dest.output.find("name") != std::string::npos);
  const bool endedWithExpectedChar = dest.last() == '\n' ||
                                    dest.last() == '"' ||
                                    dest.last() == '}';
  REQUIRE(endedWithExpectedChar);
}
