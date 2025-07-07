#pragma once

#include <set>
#include <sstream>

#include "YAML.hpp"
#include "YAML_Core.hpp"

class YAML_Analyzer : public YAML_Lib::IAction {
public:
  YAML_Analyzer() = default;
  ~YAML_Analyzer() override = default;
  // Add Node details to analysis
  void onNode([[maybe_unused]] const YAML_Lib::Node &yNode) override {
    totalNodes++;
  }
  // Add string details to analysis
  void onString(const YAML_Lib::Node &yNode) override {
    const auto &yNodeString = YRef<YAML_Lib::String>(yNode);
    totalStrings++;
    sizeInBytes += sizeof(YAML_Lib::String);
    sizeInBytes += yNodeString.value().size();
    maxStringSize = std::max(yNodeString.value().size(), maxKeySize);
    uniqueStrings.insert(yNodeString.value());
  }
  // Add number details to analysis
  void onNumber(const YAML_Lib::Node &yNode) override {
    const auto &yNodeNumber = YRef<YAML_Lib::Number>(yNode);
    totalNumbers++;
    sizeInBytes += sizeof(YAML_Lib::Number);
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
  void onBoolean([[maybe_unused]] const YAML_Lib::Node &yNode) override {
    totalBoolean++;
    sizeInBytes += sizeof(YAML_Lib::Boolean);
  }
  // Add null details to analysis
  void onNull([[maybe_unused]] const YAML_Lib::Node &yNode) override {
    totalNull++;
    sizeInBytes += sizeof(YAML_Lib::Null);
  }
  // Add array details to analysis
  void onArray(const YAML_Lib::Node &yNode) override {
    const auto &yNodeArray = YRef<YAML_Lib::Array>(yNode);
    totalArrays++;
    sizeInBytes += sizeof(YAML_Lib::Array);
    maxArraySize = std::max(yNodeArray.size(), maxArraySize);
    for ([[maybe_unused]] auto &yNodeEntry : yNodeArray.value()) {
      sizeInBytes += sizeof(YAML_Lib::Node);
    }
  }
  // Add object details to analysis
  void onDictionary(const YAML_Lib::Node &yNode) override {
    const auto &yNodeDictionary = YRef<YAML_Lib::Dictionary>(yNode);
    totalDictionaries++;
    sizeInBytes += sizeof(YAML_Lib::Dictionary);
    maxDictionarySize =
        std::max(yNodeDictionary.value().size(), maxDictionarySize);
    for (auto &entry : yNodeDictionary.value()) {
      auto key = entry.getKey();
      uniqueKeys.insert(key);
      maxKeySize = std::max(key.size(), maxKeySize);
      sizeInBytes += key.size();
      sizeInBytes += sizeof(YAML_Lib::Dictionary::Entry);
      totalKeys++;
    }
  }
  // Output analysis details
  std::string dump() {
    std::stringstream os;
    os << "\n------------------YAML Tree Stats------------------\n";
    os << "YAML Tree contains " << totalNodes << " nodes.\n";
    os << "YAML Tree size " << sizeInBytes << " in bytes.\n";
    os << "------------------YAML YAML_Lib::Dictionary "
          "Stats------------------\n";
    os << "YAML Tree contains " << totalDictionaries << " objectEntries.\n";
    os << "YAML Tree max object size " << maxDictionarySize << ".\n";
    os << "YAML Tree total " << totalKeys << " keys.\n";
    os << "YAML Tree contains " << uniqueKeys.size() << " unique keys.\n";
    os << "YAML Tree max key size " << maxKeySize << " in bytes.\n";
    os << "------------------YAML YAML_Lib::Array Stats------------------\n";
    os << "YAML Tree contains " << totalArrays << " arrays.\n";
    os << "YAML Tree max array size " << maxArraySize << ".\n";
    os << "------------------YAML YAML_Lib::String Stats------------------\n";
    os << "YAML Tree total " << totalStrings << " strings.\n";
    os << "YAML Tree contains " << uniqueStrings.size() << " unique strings.\n";
    os << "YAML Tree max string size " << maxStringSize << " in bytes.\n";
    os << "------------------YAML YAML_Lib::Number Stats------------------\n";
    os << "YAML Tree contains " << totalNumbers << " numbers.\n";
    os << "YAML Tree contains " << totalInteger << " integers.\n";
    os << "YAML Tree contains " << totalLong << " longs.\n";
    os << "YAML Tree contains " << totalLongLong << " long longs.\n";
    os << "YAML Tree contains " << totalFloat << " floats.\n";
    os << "YAML Tree contains " << totalDouble << " doubles.\n";
    os << "YAML Tree contains " << totalLongDouble << " long doubles.\n";
    os << "------------------YAML YAML_Lib::Boolean Stats------------------\n";
    os << "YAML Tree contains " << totalBoolean << " booleans.\n";
    os << "------------------YAML YAML_Lib::Null Stats------------------\n";
    os << "YAML Tree contains " << totalNull << " nulls.\n";
    os << "----------------------------------------------------";
    return (os.str());
  }
  static std::string dumpNodeSizes() {
    std::stringstream os;
    os << "\n--------------------YAML_Lib::Node Sizes---------------------\n";
    os << "YAML_Lib::Node size " << sizeof(YAML_Lib::Node) << " in bytes.\n";
    os << "YAML_Lib::Dictionary size " << sizeof(YAML_Lib::Dictionary)
       << " in bytes.\n";
    os << "YAML_Lib::Dictionary Entry size "
       << sizeof(YAML_Lib::Dictionary::Entry) << " in bytes.\n";
    os << "YAML_Lib::Array size " << sizeof(YAML_Lib::Array) << " in bytes.\n";
    os << "YAML_Lib::Number::Values size " << sizeof(YAML_Lib::Number::Values)
       << " in bytes.\n";
    os << "YAML_Lib::Number size " << sizeof(YAML_Lib::Number)
       << " in bytes.\n";
    os << "YAML_Lib::String size " << sizeof(YAML_Lib::String)
       << " in bytes.\n";
    os << "YAML_Lib::Boolean size " << sizeof(YAML_Lib::Boolean)
       << " in bytes.\n";
    os << "YAML_Lib::Null size " << sizeof(YAML_Lib::Null) << " in bytes.\n";
    return (os.str());
  }
  static std::string dumpNumericSizes() {
    std::stringstream os;
    os << "\n--------------------System Numeric Sizes---------------------\n";
    os << "Short size " << sizeof(short) << " in bytes.\n";
    os << "Int size " << sizeof(int) << " in bytes.\n";
    os << "Long Long size " << sizeof(long long) << " in bytes.\n";
    os << "Float size " << sizeof(float) << " in bytes.\n";
    os << "Double size " << sizeof(double) << " in bytes.\n";
    os << "Long Double size " << sizeof(long double) << " in bytes.\n";
    return (os.str());
  }

private:
  // YAML analysis data
  // Node
  int64_t totalNodes{};
  size_t sizeInBytes{};
  // YAML_Lib::Dictionary
  int64_t totalDictionaries{};
  size_t maxDictionarySize{};
  int64_t totalKeys{};
  size_t maxKeySize{};
  std::set<std::string_view> uniqueKeys{};
  // YAML_Lib::Array
  size_t maxArraySize{};
  int64_t totalArrays{};
  // YAML_Lib::String
  int64_t totalStrings{};
  std::set<std::string_view> uniqueStrings{};
  size_t maxStringSize{};
  // YAML_Lib::Number
  int64_t totalNumbers{};
  int64_t totalInteger{};
  int64_t totalLong{};
  int64_t totalLongLong{};
  int64_t totalFloat{};
  int64_t totalDouble{};
  int64_t totalLongDouble{};
  // YAML_Lib::Boolean
  int64_t totalBoolean{};
  // YAML_Lib::Null
  int64_t totalNull{};
};