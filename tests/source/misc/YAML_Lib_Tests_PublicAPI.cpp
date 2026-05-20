#include "YAML_Lib_Tests.hpp"
#include <fstream>
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
