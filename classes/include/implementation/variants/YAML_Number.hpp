#pragma once

#include <charconv>

namespace YAML_Lib {

struct Number {
  // Number values variant
  using Values = std::variant<std::monostate, int, long, long long, float,
                              double, long double>;

  // All string conversion base default
  static constexpr int kStringConversionBase{10};
  // Floating point notation
  enum class numberNotation { normal = 0, fixed, scientific };
  // Constructors/Destructors
  Number() = default;
  template <typename T> explicit Number(T value);
  Number(const Number &other) = default;
  Number &operator=(const Number &other) = default;
  Number(Number &&other) = default;
  Number &operator=(Number &&other) = default;
  ~Number() = default;
  // Is number an int/long/long long/float/double/long double ?
  template <typename T> [[nodiscard]] bool is() const {
    return std::get_if<T>(&yNodeNumber) != nullptr;
  }
  // Return numbers value int/long long/float/double/long double.
  // Note: Can still return an integer value for a floating point.
  template <typename T> [[nodiscard]] T value() const { return getAs<T>(); }
  // Set numbers value to int/long/long long/float/double/long double
  template <typename T> void set(T number) { *this = Number(number); }
  // Return string representation of value
  [[nodiscard]] std::string toString() const { return getAs<std::string>(); }
  // Convert variant to a key
  [[nodiscard]] std::string toKey() const { return getAs<std::string>(); }
  // Set floating point to string conversion parameters
  static void setPrecision(const int precision) { numberPrecision = precision; }
  static void setNotation(const numberNotation notation) {
    numberNotation = notation;
  }

private:
  // Convert string to specific numeric type (returns true on success)
  template <typename T> bool stringToNumber(const std::string_view &number);
  // Number to string
  template <typename T>
  [[nodiscard]] std::string numberToString(const T &number) const;
  // Convert values to another specified type
  template <typename T, typename U> [[nodiscard]] T convertTo(U value) const;
  // Convert values to another specified type
  template <typename T> [[nodiscard]] T getAs() const;
  // Find the smallest type that can represent a number. Please note that if it
  // cannot be held as an integer, then floating point types are tried.
  void convertNumber(const std::string_view &number) {
    [[maybe_unused]] auto ok =
        stringToNumber<int>(number) || stringToNumber<long>(number) ||
        stringToNumber<long long>(number) || stringToNumber<float>(number) ||
        stringToNumber<double>(number) || stringToNumber<long double>(number);
  }
  // Number values (variant)
  Values yNodeNumber;
  // Floating point to string parameters
  inline static int numberPrecision{6};
  inline static auto numberNotation{numberNotation::normal};
};
// Construct Number from value
template <typename T> Number::Number(T value) {
  if constexpr (std::is_same_v<T, std::string>) {
    convertNumber(value);
  } else {
    yNodeNumber = value;
  }
}
// Convert string to specific numeric type (returns true on success)
template <typename T>
bool Number::stringToNumber(const std::string_view &number) {
  T value{};
  // std::from_chars does not accept a leading '+'; strip it if present.
  std::string_view sv = number;
  if (!sv.empty() && sv[0] == '+') {
    sv.remove_prefix(1);
  }
  const char *begin = sv.data();
  const char *end   = sv.data() + sv.size();
  std::from_chars_result result;
  if constexpr (std::is_same_v<T, int> || std::is_same_v<T, long> ||
                std::is_same_v<T, long long>) {
    // NOTE: YAML 1.2 defines octal as "0o<digits>" only; C-style "0NNN"
    // leading-zero octal is NOT valid in YAML 1.2 and must not be treated as
    // base 8 here. The parser converts "0o<digits>" to its decimal string
    // value before constructing Number, so base 10 is always correct unless
    // the token carries an explicit 0x/0X hex prefix.
    if (sv.size() > 2 &&
        sv[0] == '0' && (sv[1] == 'x' || sv[1] == 'X')) {
      // from_chars does not consume the "0x" prefix; skip it manually.
      result = std::from_chars(begin + 2, end, value, 16);
    } else {
      result = std::from_chars(begin, end, value, 10);
    }
  } else {
    // Floating-point: from_chars (GCC 11+ / Clang 12+ / MSVC 16.4+).
    result = std::from_chars(begin, end, value);
  }
  if (result.ec != std::errc{} || result.ptr != end) {
    return false;
  }
  *this = Number(value);
  return true;
}
// Number to string
template <typename T>
std::string Number::numberToString(const T &number) const {
  if constexpr (std::is_floating_point_v<T>) {
    // YAML 1.2 §10.3.2: special float values must stringify to .inf / -.inf /
    // .nan
    if (std::isinf(number)) {
      return number > T{0} ? ".inf" : "-.inf";
    }
    if (std::isnan(number)) {
      return ".nan";
    }
  }
  std::ostringstream os;
  if constexpr (std::is_floating_point_v<T>) {
    switch (numberNotation) {
    case numberNotation::normal:
      os << std::defaultfloat << std::setprecision(numberPrecision) << number;
      break;
    case numberNotation::fixed:
      os << std::fixed << std::setprecision(numberPrecision) << number;
      break;
    case numberNotation::scientific:
      os << std::scientific << std::setprecision(numberPrecision) << number;
      break;
    default:
      os << std::setprecision(numberPrecision) << number;
    }
    if (os.str().find('.') == std::string::npos) {
      return os.str() + ".0";
    }
  } else {
    os << number;
  }
  return os.str();
}
// Convert value to another specified type
template <typename T, typename U> T Number::convertTo(U value) const {
  if constexpr (std::is_same_v<T, std::string>) {
    return numberToString(value);
  } else {
    return static_cast<T>(value);
  }
}
// Convert stored number to another specified type
template <typename T> T Number::getAs() const {
  if (const auto pValue = std::get_if<int>(&yNodeNumber)) {
    return convertTo<T>(*pValue);
  }
  if (const auto pValue = std::get_if<long>(&yNodeNumber)) {
    return convertTo<T>(*pValue);
  }
  if (const auto pValue = std::get_if<long long>(&yNodeNumber)) {
    return convertTo<T>(*pValue);
  }
  if (const auto pValue = std::get_if<float>(&yNodeNumber)) {
    return convertTo<T>(*pValue);
  }
  if (const auto pValue = std::get_if<double>(&yNodeNumber)) {
    return convertTo<T>(*pValue);
  }
  if (const auto pValue = std::get_if<long double>(&yNodeNumber)) {
    return convertTo<T>(*pValue);
  }
  throw std::runtime_error("Could not convert unknown type.");
}
} // namespace YAML_Lib