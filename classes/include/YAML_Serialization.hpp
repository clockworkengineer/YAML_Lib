#pragma once

/**
 * @file YAML_Serialization.hpp
 * @brief Non-intrusive object serialization and deserialization support for YAML_Lib.
 */

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace YAML_Lib {

struct Node;
struct Array;
struct Dictionary;
struct Null;

// ============================================================================
// Primitive Serializers
// ============================================================================

inline void to_yaml(Node& n, const std::string& str);
inline void to_yaml(Node& n, std::string_view sv);
inline void to_yaml(Node& n, const char* str);
inline void to_yaml(Node& n, bool b);

template <typename T>
  requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
void to_yaml(Node& n, T val);

inline void from_yaml(const Node& n, std::string& str);
inline void from_yaml(const Node& n, bool& b);

template <typename T>
  requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
void from_yaml(const Node& n, T& val);

// ============================================================================
// STL Container Serializers
// ============================================================================

template <typename T>
void to_yaml(Node& n, const std::vector<T>& vec);

template <typename T>
void from_yaml(const Node& n, std::vector<T>& vec);

template <typename T>
void to_yaml(Node& n, const std::map<std::string, T>& map);

template <typename T>
void from_yaml(const Node& n, std::map<std::string, T>& map);

template <typename T>
void to_yaml(Node& n, const std::unordered_map<std::string, T>& map);

template <typename T>
void from_yaml(const Node& n, std::unordered_map<std::string, T>& map);

template <typename T>
void to_yaml(Node& n, const std::optional<T>& opt);

template <typename T>
void from_yaml(const Node& n, std::optional<T>& opt);

}  // namespace YAML_Lib

// ============================================================================
// YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE macro
// ============================================================================

#define YAML_LIB_INTERNAL_EXPAND(x) x
#define YAML_LIB_INTERNAL_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,NAME,...) NAME

#define YAML_LIB_INTERNAL_TO_FIELD(n, obj, f) { YAML_Lib::Node child; to_yaml(child, obj.f); n[#f] = std::move(child); }

#define YAML_LIB_INTERNAL_TO_YAML_1(n, obj, f1) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f1)
#define YAML_LIB_INTERNAL_TO_YAML_2(n, obj, f1, f2) YAML_LIB_INTERNAL_TO_YAML_1(n, obj, f1) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f2)
#define YAML_LIB_INTERNAL_TO_YAML_3(n, obj, f1, f2, f3) YAML_LIB_INTERNAL_TO_YAML_2(n, obj, f1, f2) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f3)
#define YAML_LIB_INTERNAL_TO_YAML_4(n, obj, f1, f2, f3, f4) YAML_LIB_INTERNAL_TO_YAML_3(n, obj, f1, f2, f3) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f4)
#define YAML_LIB_INTERNAL_TO_YAML_5(n, obj, f1, f2, f3, f4, f5) YAML_LIB_INTERNAL_TO_YAML_4(n, obj, f1, f2, f3, f4) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f5)
#define YAML_LIB_INTERNAL_TO_YAML_6(n, obj, f1, f2, f3, f4, f5, f6) YAML_LIB_INTERNAL_TO_YAML_5(n, obj, f1, f2, f3, f4, f5) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f6)
#define YAML_LIB_INTERNAL_TO_YAML_7(n, obj, f1, f2, f3, f4, f5, f6, f7) YAML_LIB_INTERNAL_TO_YAML_6(n, obj, f1, f2, f3, f4, f5, f6) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f7)
#define YAML_LIB_INTERNAL_TO_YAML_8(n, obj, f1, f2, f3, f4, f5, f6, f7, f8) YAML_LIB_INTERNAL_TO_YAML_7(n, obj, f1, f2, f3, f4, f5, f6, f7) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f8)
#define YAML_LIB_INTERNAL_TO_YAML_9(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9) YAML_LIB_INTERNAL_TO_YAML_8(n, obj, f1, f2, f3, f4, f5, f6, f7, f8) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f9)
#define YAML_LIB_INTERNAL_TO_YAML_10(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10) YAML_LIB_INTERNAL_TO_YAML_9(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f10)
#define YAML_LIB_INTERNAL_TO_YAML_11(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11) YAML_LIB_INTERNAL_TO_YAML_10(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f11)
#define YAML_LIB_INTERNAL_TO_YAML_12(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12) YAML_LIB_INTERNAL_TO_YAML_11(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11) YAML_LIB_INTERNAL_TO_FIELD(n, obj, f12)

#define YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f) \
  if (YAML_Lib::isA<YAML_Lib::Dictionary>(n) && YAML_Lib::NRef<YAML_Lib::Dictionary>(n).contains(#f)) { \
    from_yaml(n[#f], obj.f); \
  }

#define YAML_LIB_INTERNAL_FROM_YAML_1(n, obj, f1) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f1)
#define YAML_LIB_INTERNAL_FROM_YAML_2(n, obj, f1, f2) YAML_LIB_INTERNAL_FROM_YAML_1(n, obj, f1) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f2)
#define YAML_LIB_INTERNAL_FROM_YAML_3(n, obj, f1, f2, f3) YAML_LIB_INTERNAL_FROM_YAML_2(n, obj, f1, f2) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f3)
#define YAML_LIB_INTERNAL_FROM_YAML_4(n, obj, f1, f2, f3, f4) YAML_LIB_INTERNAL_FROM_YAML_3(n, obj, f1, f2, f3) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f4)
#define YAML_LIB_INTERNAL_FROM_YAML_5(n, obj, f1, f2, f3, f4, f5) YAML_LIB_INTERNAL_FROM_YAML_4(n, obj, f1, f2, f3, f4) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f5)
#define YAML_LIB_INTERNAL_FROM_YAML_6(n, obj, f1, f2, f3, f4, f5, f6) YAML_LIB_INTERNAL_FROM_YAML_5(n, obj, f1, f2, f3, f4, f5) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f6)
#define YAML_LIB_INTERNAL_FROM_YAML_7(n, obj, f1, f2, f3, f4, f5, f6, f7) YAML_LIB_INTERNAL_FROM_YAML_6(n, obj, f1, f2, f3, f4, f5, f6) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f7)
#define YAML_LIB_INTERNAL_FROM_YAML_8(n, obj, f1, f2, f3, f4, f5, f6, f7, f8) YAML_LIB_INTERNAL_FROM_YAML_7(n, obj, f1, f2, f3, f4, f5, f6, f7) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f8)
#define YAML_LIB_INTERNAL_FROM_YAML_9(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9) YAML_LIB_INTERNAL_FROM_YAML_8(n, obj, f1, f2, f3, f4, f5, f6, f7, f8) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f9)
#define YAML_LIB_INTERNAL_FROM_YAML_10(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10) YAML_LIB_INTERNAL_FROM_YAML_9(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f10)
#define YAML_LIB_INTERNAL_FROM_YAML_11(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11) YAML_LIB_INTERNAL_FROM_YAML_10(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f11)
#define YAML_LIB_INTERNAL_FROM_YAML_12(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12) YAML_LIB_INTERNAL_FROM_YAML_11(n, obj, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11) YAML_LIB_INTERNAL_FROM_FIELD(n, obj, f12)

#define YAML_LIB_DEFINE_TYPE_NON_INTRUSIVE(Type, ...) \
  inline void to_yaml(YAML_Lib::Node& n, const Type& obj) { \
    YAML_LIB_INTERNAL_EXPAND(YAML_LIB_INTERNAL_GET_MACRO(__VA_ARGS__, \
      _16,_15,_14,_13, \
      YAML_LIB_INTERNAL_TO_YAML_12, YAML_LIB_INTERNAL_TO_YAML_11, YAML_LIB_INTERNAL_TO_YAML_10, \
      YAML_LIB_INTERNAL_TO_YAML_9, YAML_LIB_INTERNAL_TO_YAML_8, YAML_LIB_INTERNAL_TO_YAML_7, \
      YAML_LIB_INTERNAL_TO_YAML_6, YAML_LIB_INTERNAL_TO_YAML_5, YAML_LIB_INTERNAL_TO_YAML_4, \
      YAML_LIB_INTERNAL_TO_YAML_3, YAML_LIB_INTERNAL_TO_YAML_2, YAML_LIB_INTERNAL_TO_YAML_1)(n, obj, __VA_ARGS__)) \
  } \
  inline void from_yaml(const YAML_Lib::Node& n, Type& obj) { \
    YAML_LIB_INTERNAL_EXPAND(YAML_LIB_INTERNAL_GET_MACRO(__VA_ARGS__, \
      _16,_15,_14,_13, \
      YAML_LIB_INTERNAL_FROM_YAML_12, YAML_LIB_INTERNAL_FROM_YAML_11, YAML_LIB_INTERNAL_FROM_YAML_10, \
      YAML_LIB_INTERNAL_FROM_YAML_9, YAML_LIB_INTERNAL_FROM_YAML_8, YAML_LIB_INTERNAL_FROM_YAML_7, \
      YAML_LIB_INTERNAL_FROM_YAML_6, YAML_LIB_INTERNAL_FROM_YAML_5, YAML_LIB_INTERNAL_FROM_YAML_4, \
      YAML_LIB_INTERNAL_FROM_YAML_3, YAML_LIB_INTERNAL_FROM_YAML_2, YAML_LIB_INTERNAL_FROM_YAML_1)(n, obj, __VA_ARGS__)) \
  }

// ============================================================================
// Implementations (require complete Node, Array, Dictionary types)
// ============================================================================

namespace YAML_Lib {

inline void to_yaml(Node& n, const std::string& str) { n = str; }
inline void to_yaml(Node& n, std::string_view sv) { n = std::string(sv); }
inline void to_yaml(Node& n, const char* str) { n = std::string(str); }
inline void to_yaml(Node& n, bool b) { n = b; }

template <typename T>
  requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
inline void to_yaml(Node& n, T val) {
  n = val;
}

inline void from_yaml(const Node& n, std::string& str) { str = n.as<std::string>(); }
inline void from_yaml(const Node& n, bool& b) { b = n.as<bool>(); }

template <typename T>
  requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
inline void from_yaml(const Node& n, T& val) {
  val = n.as<T>();
}

template <typename T>
inline void to_yaml(Node& n, const std::vector<T>& vec) {
  Node arr = Node::make<Array>();
  for (const auto& item : vec) {
    Node elem;
    to_yaml(elem, item);
    NRef<Array>(arr).add(std::move(elem));
  }
  n = std::move(arr);
}

template <typename T>
inline void from_yaml(const Node& n, std::vector<T>& vec) {
  vec.clear();
  if (isA<Array>(n)) {
    const auto& arr = NRef<Array>(n);
    vec.reserve(arr.size());
    for (std::size_t i = 0; i < arr.size(); ++i) {
      T item{};
      from_yaml(arr[i], item);
      vec.push_back(std::move(item));
    }
  }
}

template <typename T>
inline void to_yaml(Node& n, const std::map<std::string, T>& map) {
  Node dict = Node::make<Dictionary>();
  for (const auto& [k, v] : map) {
    Node elem;
    to_yaml(elem, v);
    dict[k] = std::move(elem);
  }
  n = std::move(dict);
}

template <typename T>
inline void from_yaml(const Node& n, std::map<std::string, T>& map) {
  map.clear();
  if (isA<Dictionary>(n)) {
    const auto& dict = NRef<Dictionary>(n);
    for (const auto& entry : dict.items()) {
      T val{};
      from_yaml(entry.getNode(), val);
      map[std::string(entry.getKey())] = std::move(val);
    }
  }
}

template <typename T>
inline void to_yaml(Node& n, const std::unordered_map<std::string, T>& map) {
  Node dict = Node::make<Dictionary>();
  for (const auto& [k, v] : map) {
    Node elem;
    to_yaml(elem, v);
    dict[k] = std::move(elem);
  }
  n = std::move(dict);
}

template <typename T>
inline void from_yaml(const Node& n, std::unordered_map<std::string, T>& map) {
  map.clear();
  if (isA<Dictionary>(n)) {
    const auto& dict = NRef<Dictionary>(n);
    for (const auto& entry : dict.items()) {
      T val{};
      from_yaml(entry.getNode(), val);
      map[std::string(entry.getKey())] = std::move(val);
    }
  }
}

template <typename T>
inline void to_yaml(Node& n, const std::optional<T>& opt) {
  if (opt.has_value()) {
    to_yaml(n, *opt);
  } else {
    n = Node::make<Null>();
  }
}

template <typename T>
inline void from_yaml(const Node& n, std::optional<T>& opt) {
  if (n.isEmpty() || isA<Null>(n)) {
    opt = std::nullopt;
  } else {
    T val{};
    from_yaml(n, val);
    opt = std::move(val);
  }
}

}  // namespace YAML_Lib
