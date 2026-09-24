#include "YAML_Lib_Tests.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace YAML_Lib;

TEST_CASE("YAML::load and YAML::dump roundtrip through the public API", "[YAML][API][Public]") {
  std::unique_ptr<YAML> yaml = YAML::load("---\nname: Alice\nage: 30\n");
  REQUIRE(yaml != nullptr);
  REQUIRE(yaml->getNumberOfDocuments() == 1);

  const std::string output = yaml->dump();
  REQUIRE(output.find("name: Alice") != std::string::npos);
  REQUIRE(output.find("age: 30") != std::string::npos);
}

TEST_CASE("YAML::toString produces equivalent YAML text", "[YAML][API][Public]") {
  YAML yaml;
  yaml.parse(BufferSource{"---\nvalue: 42\n"});

  const std::string text = yaml.toString();
  REQUIRE(text.find("value: 42") != std::string::npos);
  REQUIRE(text.find("---") != std::string::npos);
}

TEST_CASE("Custom ISource implementations propagate source errors through parse", "[YAML][API][Source][Error]") {
  struct BrokenSource : ISource {
    explicit BrokenSource(std::string contents) : buffer(std::move(contents)) {}

    char current() const override {
      if (position_ >= buffer.size()) {
        throw ISource::Error("BrokenSource: current() past end");
      }
      return buffer[position_];
    }

    void next() override {
      if (position_ >= buffer.size()) {
        throw ISource::Error("BrokenSource: next() past end");
      }
      if (buffer[position_] == '\n') {
        lineNo++;
        column = 1;
      } else {
        column++;
      }
      position_++;
    }

    bool more() const override { return position_ <= buffer.size(); }
    void reset() override { position_ = 0; lineNo = 1; column = 1; }
    std::size_t position() override { return position_; }
    void save() override { contexts.emplace_back(lineNo, column, position_); }
    void restore() override {
      if (contexts.empty()) {
        throw ISource::Error("BrokenSource: restore() without save");
      }
      const auto context = contexts.back();
      contexts.pop_back();
      lineNo = context.lineNo;
      column = context.column;
      position_ = context.bufferPosition;
    }
    void discardSave() override {
      if (contexts.empty()) {
        throw ISource::Error("BrokenSource: discardSave() without save");
      }
      contexts.pop_back();
    }

  private:
    void backup(unsigned long length) override {
      if (length > position_) {
        throw ISource::Error("BrokenSource: backup() beyond start");
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

  BrokenSource source{"---\nkey: value\n"};
  YAML yaml;
  REQUIRE_THROWS_AS(yaml.parse(source), ISource::Error);
}

TEST_CASE("Stringify fails fast when std::ostream is already in a bad state", "[YAML][API][Destination][Error]") {
  YAML yaml;
  yaml.parse(BufferSource{"---\nkey: value\n"});

  std::ostringstream out;
  out.setstate(std::ios::badbit);
  StreamDestination dest{out};

  REQUIRE_THROWS_AS(yaml.stringify(dest), IDestination::Error);
}

#ifdef YAML_LIB_FILE_IO
TEST_CASE("YAML::loadFile parses a YAML file into a YAML object", "[YAML][API][FileIO]") {
  const std::string fileName = "test_public_api_loadfile.yaml";
  const std::string content = "---\nname: Bob\n";

  std::ofstream out(fileName, std::ios::binary);
  REQUIRE(out.good());
  out << content;
  out.close();

  std::unique_ptr<YAML> yaml = YAML::loadFile(fileName);
  REQUIRE(yaml != nullptr);
  REQUIRE(yaml->getNumberOfDocuments() == 1);
  REQUIRE(isA<String>(yaml->document(0)["name"]));
  REQUIRE(NRef<String>(yaml->document(0)["name"]).value() == "Bob");

  std::remove(fileName.c_str());
}
#endif

TEST_CASE("YAML supports move construction and move assignment", "[YAML][API][Move]") {
  YAML original("item: book\ncount: 5\n");
  REQUIRE(original.getNumberOfDocuments() == 1);
  REQUIRE(isA<String>(original["item"]));

  // Move construct
  YAML movedTo(std::move(original));
  REQUIRE(movedTo.getNumberOfDocuments() == 1);
  REQUIRE(NRef<String>(movedTo["item"]).value() == "book");

  // Move assign
  YAML assigned;
  assigned = std::move(movedTo);
  REQUIRE(assigned.getNumberOfDocuments() == 1);
  REQUIRE(NRef<String>(assigned["item"]).value() == "book");
}

TEST_CASE("YAML and Node support deep-copy via clone()", "[YAML][API][Clone]") {
  auto yaml = YAML::load("title: Original\nitems:\n  - a\n  - b\n");
  REQUIRE(yaml != nullptr);

  // Clone YAML object
  auto cloned = yaml->clone();
  REQUIRE(cloned != nullptr);
  REQUIRE(cloned->getNumberOfDocuments() == 1);
  REQUIRE(NRef<String>((*cloned)["title"]).value() == "Original");

  // Modify clone, verify original unchanged
  (*cloned)["title"] = "Modified";
  REQUIRE(NRef<String>((*cloned)["title"]).value() == "Modified");
  REQUIRE(NRef<String>((*yaml)["title"]).value() == "Original");

  // Clone individual Node
  Node clonedNode = (*yaml)["items"].clone();
  REQUIRE(isA<Array>(clonedNode));
  REQUIRE(NRef<Array>(clonedNode).size() == 2);
}

TEST_CASE("YAML exceptions can be caught via YAML_Lib::Exception", "[YAML][API][Exception]") {
  YAML yaml;
  bool caughtYAML_LibException = false;
  try {
    yaml.parse(BufferSource{"---\nkey:\n\tvalue\n"});
  } catch (const YAML_Lib::Exception &e) {
    caughtYAML_LibException = true;
    REQUIRE(std::string(e.what()).find("YAML Syntax Error") != std::string::npos);
  }
  REQUIRE(caughtYAML_LibException);
}

TEST_CASE("YAML::dump supports case-insensitive format names", "[YAML][API][Stringify]") {
  auto yaml = YAML::load("name: Test\nstatus: active\n");
  REQUIRE(yaml != nullptr);

  const std::string lowerJson = yaml->dump("json");
  const std::string upperJson = yaml->dump("JSON");
  REQUIRE(lowerJson == upperJson);
  REQUIRE(lowerJson.find("\"name\":\"Test\"") != std::string::npos);

  const std::string lowerYaml = yaml->dump("yaml");
  const std::string upperYaml = yaml->dump("YAML");
  REQUIRE(lowerYaml == upperYaml);
}

