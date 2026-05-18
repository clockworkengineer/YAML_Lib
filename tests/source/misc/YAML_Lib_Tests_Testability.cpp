#include "YAML_Lib_Tests.hpp"

using namespace YAML_Lib;
namespace {
struct FakeSource : ISource {
  explicit FakeSource(std::string text) : buffer(std::move(text)) {}

  char current() const override { return buffer[position_]; }
  void next() override {
    if (!more()) {
      throw ISource::Error("FakeSource::next() past end");
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
      throw ISource::Error("FakeSource::restore() without save");
    }
    const auto context = contexts.back();
    contexts.pop_back();
    lineNo = context.lineNo;
    column = context.column;
    position_ = context.bufferPosition;
  }
  void discardSave() override {
    if (contexts.empty()) {
      throw ISource::Error("FakeSource::discardSave() without save");
    }
    contexts.pop_back();
  }

private:
  void backup(unsigned long length) override {
    if (length > position_) {
      throw ISource::Error("FakeSource::backup() beyond start");
    }
    position_ -= length;
    column = 1;
    lineNo = 1;
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
};

struct FakeDestination : IDestination {
  void add(char ch) override { output.push_back(ch); }
  void clear() override { output.clear(); }
  char last() override { return output.empty() ? '\0' : output.back(); }

  std::string output;
};

struct FakeStringify : IStringify {
  void stringify(const Node &yNode, IDestination &destination, unsigned long) const override {
    destination.add("FAKE");
    if (isA<Dictionary>(yNode) && NRef<Dictionary>(yNode).contains("test")) {
      destination.add("-PASS");
    }
  }
};

struct FakeParser : IParser {
  std::vector<Node> parse(ISource &source) override {
    (void)source;
    std::vector<Node> docs;
    docs.emplace_back(Node{{{"test", "ok"}}});
    return docs;
  }
};
} // namespace

TEST_CASE("Test helpers can be used to mock custom I/O and stringifier behavior", "[YAML][Testability][Mock]") {
  Options options;
  options.parser = new FakeParser();
  options.stringifier = makeStringify<FakeStringify>();

  YAML yaml(options);
  FakeSource source("---\nignored: yes\n");
  yaml.parse(source);

  FakeDestination dest;
  yaml.stringify(dest);

  REQUIRE(dest.output == "FAKE-PASS");
}

TEST_CASE("FakeSource preserves line/column state across save/restore", "[YAML][Testability][ISource]") {
  FakeSource source("line1\nline2\n");
  source.next();
  source.next();
  source.save();
  source.next();
  source.restore();
  REQUIRE(source.position() == 2);
  REQUIRE(source.more());
}

TEST_CASE("FakeDestination captures output and respects clear/last semantics", "[YAML][Testability][IDestination]") {
  FakeDestination dest;
  dest.add('a');
  dest.add("bc");
  REQUIRE(dest.output == "abc");
  REQUIRE(dest.last() == 'c');
  dest.clear();
  REQUIRE(dest.output.empty());
}
