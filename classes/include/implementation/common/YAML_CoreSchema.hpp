#pragma once

#include <algorithm>
#include <charconv>
#include <limits>
#include <string>
#include <string_view>

#include "YAML_Core.hpp"
#include "interface/ISchema.hpp"

namespace YAML_Lib {

/**
 * @brief YAML 1.2 Core Schema implementation of ISchema interface.
 *
 * Implements Open/Closed Principle (OCP) by encapsulating default scalar type resolution
 * and tag coercion rules into an extensible strategy class.
 */
class CoreSchema : public ISchema {
public:
  explicit CoreSchema(bool strictBooleans = false) : strictBooleans_(strictBooleans) {}
  ~CoreSchema() override = default;

  void setStrictBooleans(bool strict) noexcept {
    strictBooleans_ = strict;
  }

  [[nodiscard]] Node resolveScalar(std::string_view scalarText, bool isQuoted) const override {
    if (isQuoted) {
      return Node::make<String>(scalarText);
    }

    if (scalarText.empty()) {
      return Node::make<Null>();
    }

    // 1. Null check
    if (scalarText == "null" || scalarText == "Null" || scalarText == "NULL" || scalarText == "~") {
      return Node::make<Null>();
    }

    // 2. Boolean check
    if (scalarText == "true" || scalarText == "True" || scalarText == "TRUE") {
      return Node::make<Boolean>(true, scalarText);
    }
    if (scalarText == "false" || scalarText == "False" || scalarText == "FALSE") {
      return Node::make<Boolean>(false, scalarText);
    }

    if (!strictBooleans_) {
      if (scalarText == "yes" || scalarText == "Yes" || scalarText == "YES" ||
          scalarText == "on"  || scalarText == "On"  || scalarText == "ON") {
        return Node::make<Boolean>(true, scalarText);
      }
      if (scalarText == "no"  || scalarText == "No"  || scalarText == "NO" ||
          scalarText == "off" || scalarText == "Off" || scalarText == "OFF") {
        return Node::make<Boolean>(false, scalarText);
      }
    }

    // 3. Number check
    std::string textStr(scalarText);
    if (!textStr.empty() && (textStr[0] == '.' || textStr[0] == '+' || textStr[0] == '-')) {
      std::string lower = textStr;
      std::transform(lower.begin(), lower.end(), lower.begin(),
                     [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
      if (lower == ".inf" || lower == "+.inf") {
        return Node::make<Number>(std::numeric_limits<double>::infinity());
      }
      if (lower == "-.inf") {
        return Node::make<Number>(-std::numeric_limits<double>::infinity());
      }
      if (lower == ".nan") {
        return Node::make<Number>(std::numeric_limits<double>::quiet_NaN());
      }
    }

    // Octal parsing
    if (textStr.size() >= 3 && textStr[0] == '0' && (textStr[1] == 'o' || textStr[1] == 'O')) {
      long long val = 0;
      const std::string digits = textStr.substr(2);
      const auto res = std::from_chars(digits.data(), digits.data() + digits.size(), val, 8);
      if (res.ec == std::errc() && res.ptr == digits.data() + digits.size()) {
        textStr = std::to_string(val);
      }
    }

    if (!textStr.empty()) {
      if (Number number{textStr}; number.is<int>() || number.is<long>() ||
                                  number.is<long long>() || number.is<float>() ||
                                  number.is<double>() || number.is<long double>()) {
        return Node::make<Number>(number);
      }
    }

    // 4. Fallback to String
    return Node::make<String>(scalarText);
  }

  [[nodiscard]] Node resolveTag([[maybe_unused]] std::string_view tagHandle,
                                 std::string_view tagSuffix,
                                 Node node) const override {
    if (tagSuffix == "str") {
      return Node::make<String>(node.toString());
    }
    if (tagSuffix == "int" || tagSuffix == "float") {
      return Node::make<Number>(node.toString());
    }
    if (tagSuffix == "bool") {
      const auto s = node.toString();
      const bool val = (s == "true" || s == "1" || s == "yes");
      return Node::make<Boolean>(val, val ? "true" : "false");
    }
    if (tagSuffix == "null") {
      return Node::make<Null>();
    }
    return std::move(node);
  }

private:
  bool strictBooleans_{false};
};

} // namespace YAML_Lib
