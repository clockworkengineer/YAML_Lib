#include "YAML_Lib_Tests.hpp"
#include "YAML_Format.hpp"
#include "YAML_Serialization.hpp"

#include <string>
#include <vector>
#include <map>
#include <optional>

// Custom struct for testing YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE
struct ServerConfig {
  std::string host;
  int port{0};
  bool enabled{false};
};

YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(ServerConfig, host, port, enabled)

TEST_CASE("C++23 — std::expected parsing support.", "[YAML][Cpp23][Expected]") {
#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#if !defined(YAML_LIB_NO_EXCEPTIONS) && defined(__cpp_exceptions)
  SECTION("loadExpected parses valid YAML successfully") {
    auto res = YAML::loadExpected("name: Alice\nage: 30\n");
    REQUIRE(res.has_value());
    YAML& yaml = res.value();
    REQUIRE(yaml.as<std::string>("name") == "Alice");
    REQUIRE(yaml.as<int>("age") == 30);
  }

  SECTION("loadExpected returns error on malformed YAML") {
    auto res = YAML::loadExpected("key: [unclosed sequence\n");
    REQUIRE_FALSE(res.has_value());
    REQUIRE(!res.error().empty());
  }

  SECTION("parseExpected parses into existing instance") {
    YAML yaml;
    BufferSource src{"item: 42\n"};
    auto res = yaml.parseExpected(src);
    REQUIRE(res.has_value());
    REQUIRE(yaml.as<int>("item") == 42);
  }

  SECTION("parseExpected returns error on malformed stream") {
    YAML yaml;
    BufferSource src{"---\n[bad\n"};
    auto res = yaml.parseExpected(src);
    REQUIRE_FALSE(res.has_value());
  }
#endif
#endif
}

TEST_CASE("C++23 — Modern as<T>() and is<T>() accessors on Node and YAML.",
          "[YAML][Cpp23][Accessors]") {
  const YAML yaml("title: YAML_Lib\nversion: 2\nactive: true\nratio: 3.14\n");
  const auto& doc = yaml.document(0);

  SECTION("as<T> conversions") {
    REQUIRE(doc["title"].as<std::string>() == "YAML_Lib");
    REQUIRE(doc["title"].as<std::string_view>() == "YAML_Lib");
    REQUIRE(doc["version"].as<int>() == 2);
    REQUIRE(doc["version"].as<long long>() == 2LL);
    REQUIRE(doc["active"].as<bool>() == true);
    REQUIRE(doc["ratio"].as<double>() > 3.13);
  }

  SECTION("YAML::as<T> shortcut") {
    REQUIRE(yaml.as<std::string>("title") == "YAML_Lib");
    REQUIRE(yaml.as<int>("version") == 2);
    REQUIRE(yaml.as<bool>("active") == true);
  }

  SECTION("is<T> type queries") {
    REQUIRE(doc["title"].is<std::string>());
    REQUIRE(doc["version"].is<int>());
    REQUIRE(doc["active"].is<bool>());
    REQUIRE(doc["ratio"].is<double>());
    REQUIRE_FALSE(doc["title"].is<int>());
    REQUIRE_FALSE(doc["version"].is<bool>());
  }

  SECTION("value_or fallback semantics") {
    REQUIRE(doc["title"].value_or(std::string("fallback")) == "YAML_Lib");
    REQUIRE(doc.value_or("missing", 999) == 999);
    REQUIRE(yaml.value_or("version", 0) == 2);
    REQUIRE(yaml.value_or("nonexistent", 8080) == 8080);
  }

  SECTION("get_if optional queries") {
    auto titleOpt = doc["title"].get_if<std::string>();
    REQUIRE(titleOpt.has_value());
    REQUIRE(*titleOpt == "YAML_Lib");

    auto wrongType = doc["title"].get_if<int>();
    REQUIRE_FALSE(wrongType.has_value());
  }
}

TEST_CASE("C++23 — Structured bindings and dictionary iteration.",
          "[YAML][Cpp23][StructuredBindings]") {
  YAML yaml("a: 10\nb: 20\nc: 30\n");
  auto& dict = NRef<Dictionary>(yaml.document(0));

  SECTION("Iteration using items() and structured bindings") {
    std::map<std::string, int> collected;
    for (auto&& [key, node] : dict.items()) {
      collected[std::string(key)] = node.as<int>();
    }
    REQUIRE(collected.size() == 3);
    REQUIRE(collected["a"] == 10);
    REQUIRE(collected["b"] == 20);
    REQUIRE(collected["c"] == 30);
  }

  SECTION("Iteration directly via Node::items()") {
    std::vector<std::string> keys;
    for (auto&& [key, node] : yaml.document(0).items()) {
      keys.emplace_back(key);
    }
    REQUIRE(keys == std::vector<std::string>{"a", "b", "c"});
  }
}

TEST_CASE("C++23 — Sequence range-based for loops.", "[YAML][Cpp23][SequenceIteration]") {
  YAML yaml("[100, 200, 300]\n");
  auto& arr = NRef<Array>(yaml.document(0));

  std::vector<int> numbers;
  for (auto&& elem : arr) {
    numbers.push_back(elem.as<int>());
  }
  REQUIRE(numbers == std::vector<int>{100, 200, 300});
}

TEST_CASE("C++23 — std::format integration.", "[YAML][Cpp23][Format]") {
#if defined(__cpp_lib_format) && __cpp_lib_format >= 202110L
  YAML yaml("key: value\n");
  std::string formattedYaml = std::format("{}", yaml);
  REQUIRE(formattedYaml.find("key: value") != std::string::npos);

  std::string formattedNode = std::format("{}", yaml.document(0)["key"]);
  REQUIRE(formattedNode == "value");
#endif
}

TEST_CASE("C++23 — Non-intrusive object and STL serialization.", "[YAML][Cpp23][Serialization]") {
  SECTION("Struct serialization and deserialization via YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE") {
    ServerConfig cfg{"localhost", 8080, true};
    Node node;
    to_yaml(node, cfg);

    REQUIRE(node.is<Dictionary>());
    REQUIRE(node["host"].as<std::string>() == "localhost");
    REQUIRE(node["port"].as<int>() == 8080);
    REQUIRE(node["enabled"].as<bool>() == true);

    ServerConfig loaded;
    from_yaml(node, loaded);
    REQUIRE(loaded.host == "localhost");
    REQUIRE(loaded.port == 8080);
    REQUIRE(loaded.enabled == true);
  }

  SECTION("STL container serialization (std::vector, std::map, std::optional)") {
    std::vector<int> numbers = {1, 2, 3, 4};
    Node vecNode;
    to_yaml(vecNode, numbers);
    REQUIRE(vecNode.is<Array>());

    std::vector<int> loadedVec;
    from_yaml(vecNode, loadedVec);
    REQUIRE(loadedVec == numbers);

    std::map<std::string, std::string> mapData = {{"env", "prod"}, {"region", "us-east"}};
    Node mapNode;
    to_yaml(mapNode, mapData);
    REQUIRE(mapNode.is<Dictionary>());

    std::map<std::string, std::string> loadedMap;
    from_yaml(mapNode, loadedMap);
    REQUIRE(loadedMap == mapData);

    std::optional<int> optVal = 42;
    Node optNode;
    to_yaml(optNode, optVal);
    std::optional<int> loadedOpt;
    from_yaml(optNode, loadedOpt);
    REQUIRE(loadedOpt == 42);

    std::optional<int> emptyOpt = std::nullopt;
    Node emptyNode;
    to_yaml(emptyNode, emptyOpt);
    std::optional<int> loadedEmpty;
    from_yaml(emptyNode, loadedEmpty);
    REQUIRE_FALSE(loadedEmpty.has_value());
  }
}
