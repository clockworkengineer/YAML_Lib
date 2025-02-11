#include "YAML_Lib_Tests.hpp"

class YAML_Analyzer final : public IAction {
public:
  YAML_Analyzer() = default;

  ~YAML_Analyzer() override = default;
  // Add YNode details to analysis
  void onYNode([[maybe_unused]] const YNode &yNode) override {
    totalNodes++;
  }
  // Add string details to analysis
  void onString([[maybe_unused]] const YNode &yNode) override {
    totalStrings++;
  }
  // Add number details to analysis
  void onNumber(const YNode &yNode) override {
    const auto &yNodeNumber = YRef<Number>(yNode);
    totalNumbers++;

    if (yNodeNumber.is<int>()) {
      totalInteger++;
    } else if (yNodeNumber.is<long>()) {
      totalLong++;
    } else if (yNodeNumber.is<long long>()) {
      totalLongLong++;
    } else if (yNodeNumber.is<float>()) {
      totalFloat++;
    } else if (yNodeNumber.is<double>()) {
      totalDouble++;
    } else if (yNodeNumber.is<long double>()) {
      totalLongDouble++;
    }
  }

  void onBoolean([[maybe_unused]] const YNode &yNode) override {
    totalBoolean++;
  }
  // Add null details to analysis
  void onNull([[maybe_unused]] const YNode &yNode) override {
    totalNull++;
  }
  // Add array details to analysis
  void onArray([[maybe_unused]] const YNode &yNode) override {
    totalArrays++;
  }
  // Add object details to analysis
  void onDictionary([[maybe_unused]] const YNode &yNode) override {
    totalDictionaries++;
  }

  // YAML analysis data
  // Node
  int64_t totalNodes{};
  // Object
  int64_t totalDictionaries{};
  // Array
  int64_t totalArrays{};
  // String
  int64_t totalStrings{};
  // Number
  int64_t totalNumbers{};
  int64_t totalInteger{};
  int64_t totalLong{};
  int64_t totalLongLong{};
  int64_t totalFloat{};
  int64_t totalDouble{};
  int64_t totalLongDouble{};
  // Boolean
  int64_t totalBoolean{};
  // Null
  int64_t totalNull{};
};

TEST_CASE("YAML YNode tree traverse tests ", "[YAML][Traverse]") {
  const YAML yaml;
  SECTION("Parse an Integer (266) and traverse", "[YAML][Traverse][Integer]") {
    BufferSource source{"266"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalInteger == 1);
    REQUIRE(analyzer.totalNumbers == 1);
  }

  SECTION("Parse an Integer (-266) and traverse", "[YAML][Traverse][Integer]") {
    BufferSource source{"-266"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalInteger == 1);
    REQUIRE(analyzer.totalNumbers == 1);
  }

  SECTION("Parse an Long (2147483647) and traverse", "[YAML][Traverse][Long]") {
    BufferSource source{"2147483647"}; // sizeof(int) == sizeof(long)
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalInteger == 1);
    REQUIRE(analyzer.totalNumbers == 1);
  }
  SECTION("Parse an Long (-2147483648) and traverse",
          "[YAML][Traverse][Long]") {
    BufferSource source{"-2147483648"}; // sizeof(int) == sizeof(long)
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalInteger == 1);
    REQUIRE(analyzer.totalNumbers == 1);
  }
  SECTION("Parse an LongLong (2147483648) and traverse",
          "[YAML][Traverse][LongLong]") {
    BufferSource source{"2147483648"}; // INT_MAX+1
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    if constexpr (sizeof(int) == sizeof(long)) {
      REQUIRE(analyzer.totalLongLong == 1);
    } else {
      REQUIRE(analyzer.totalLong == 1);
    }
    REQUIRE(analyzer.totalNumbers == 1);
  }

  SECTION("Parse an LongLong (-2147483649) and traverse",
          "[YAML][Traverse][LongLong]") {
    BufferSource source{"-2147483649"}; // INT_MIN-1
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    if constexpr (sizeof(int) == sizeof(long)) {
      REQUIRE(analyzer.totalLongLong == 1);
    } else {
      REQUIRE(analyzer.totalLong == 1);
    }
    REQUIRE(analyzer.totalNumbers == 1);
  }
  SECTION("Parse an float (32.11999) and traverse", "[YAML][Traverse][Float]") {
    BufferSource source{"32.11999"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalFloat == 1);
    REQUIRE(analyzer.totalNumbers == 1);
  }
  SECTION("Parse an double (3.402823466e+39) and traverse",
          "[YAML][Traverse][Double]") {
    BufferSource source{"3.402823466e+39"};
    yaml.parse(source); // sizeof(double) == sizeof(long double)
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalDouble == 1);
    REQUIRE(analyzer.totalNumbers == 1);
  }
  SECTION("Parse an long double (1.7976931348623158e+308) and traverse",
          "[YAML][Traverse][LongDouble]") {
    BufferSource source{"1.7976931348623158e+308"};
    yaml.parse(source); // sizeof(double) == sizeof(long double)
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalDouble == 1);
    REQUIRE(analyzer.totalNumbers == 1);
  }
  SECTION("Parse a bool (True) and traverse", "[YAML][Traverse][Boolean]") {
    BufferSource source{"True"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalBoolean == 1);
  }
  SECTION("Parse a string (\"test\") and traverse",
          "[YAML][Traverse][String]") {
    BufferSource source{"\"test\""};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 1);
    REQUIRE(analyzer.totalStrings == 1);
  }
  SECTION("Parse a array ([1,2,3,4]) and traverse", "[YAML][Traverse][Array]") {
    BufferSource source{"[1,2,3,4]"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 5);
    REQUIRE(analyzer.totalNumbers == 4);
    REQUIRE(analyzer.totalArrays == 1);
  }
  SECTION("Parse a object({\"one\" : 1, \"two\" : 2, \"three\" : 3 })) and "
          "traverse",
          "[YAML][Traverse][Object]") {
    BufferSource source{R"({"one" : 1, "two" : 2, "three" : 3 })"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 4);
    REQUIRE(analyzer.totalNumbers == 3);
    REQUIRE(analyzer.totalDictionaries == 1);
  }
  SECTION("Parse a null and traverse", "[YAML][Traverse][Null]") {
    BufferSource source{"null"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNull == 1);
  }
  SECTION("Parse a nested array "
          "({\"City\":\"London\",\"Population\":[1,2,3,4,5]}) and traverse",
          "[YAML][Traverse][Array]") {
    BufferSource source{R"({"City":"London","Population":[1,2,3,4,5]})"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 8);
    REQUIRE(analyzer.totalDictionaries == 1);
    REQUIRE(analyzer.totalArrays == 1);
    REQUIRE(analyzer.totalStrings == 1);
    REQUIRE(analyzer.totalNumbers == 5);
  }
  SECTION("Parse a nested object ([True,\"Out of "
          "time\",7.89043e+18,{\"key\":4444}]) and traverse",
          "[YAML][Traverse][Object]") {
    BufferSource source{R"([True,"Out of time",7.89043e+18,{"key":4444}])"};
    yaml.parse(source);
    YAML_Analyzer analyzer;
    yaml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 6);
    REQUIRE(analyzer.totalDictionaries == 1);
    REQUIRE(analyzer.totalArrays == 1);
    REQUIRE(analyzer.totalStrings == 1);
    REQUIRE(analyzer.totalNumbers == 2);
    REQUIRE(analyzer.totalBoolean == 1);
  }
}