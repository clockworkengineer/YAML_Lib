#include "YAML_Lib_Tests.hpp"
#include <cstdio>

using namespace YAML_Lib;

namespace {
void customPanicHandler(std::string_view, unsigned long, unsigned long) noexcept {}
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
  options.strictBooleans = true;
  options.memoryResource = std::pmr::get_default_resource();

  YAML yaml(options);
  yaml.parse(BufferSource{"---\nvalue: yes\n"});

  REQUIRE(isA<String>(yaml.document(0)["value"]));
  REQUIRE(NRef<String>(yaml.document(0)["value"]).value() == "yes");
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
