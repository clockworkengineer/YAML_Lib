#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {
namespace stringify_detail {

template <typename DocumentHandler,
          typename NumberHandler,
          typename StringHandler,
          typename BooleanHandler,
          typename NullHandler,
          typename TimestampHandler,
          typename DictionaryHandler,
          typename ArrayHandler>
inline void dispatchStringifyNode(const Node &yNode, IDestination &destination,
                                  const long indent,
                                  DocumentHandler &&documentHandler,
                                  NumberHandler &&numberHandler,
                                  StringHandler &&stringHandler,
                                  BooleanHandler &&booleanHandler,
                                  NullHandler &&nullHandler,
                                  TimestampHandler &&timestampHandler,
                                  DictionaryHandler &&dictionaryHandler,
                                  ArrayHandler &&arrayHandler) {
  if (isA<Document>(yNode)) {
    documentHandler(yNode, destination, indent);
  } else if (isA<Number>(yNode)) {
    numberHandler(yNode, destination);
  } else if (isA<String>(yNode)) {
    stringHandler(yNode, destination);
  } else if (isA<Boolean>(yNode)) {
    booleanHandler(yNode, destination);
  } else if (isA<Null>(yNode)) {
    nullHandler(yNode, destination);
  } else if (isA<Hole>(yNode)) {
    // No output for hole nodes.
  } else if (isA<Timestamp>(yNode)) {
    timestampHandler(yNode, destination);
  } else if (isA<Dictionary>(yNode)) {
    dictionaryHandler(yNode, destination);
  } else if (isA<Array>(yNode)) {
    arrayHandler(yNode, destination);
  } else {
    IStringify::throwUnknownNodeType();
  }
}

template <typename StringifyNodesFn>
inline void stringifyDocument(const Node &yNode, IDestination &destination,
                              const long indent,
                              StringifyNodesFn &&stringifyNodes) {
  stringifyNodes(NRef<Document>(yNode)[0], destination, indent);
}

template <typename WriteFn>
inline void addDelimited(IDestination &destination, const char open,
                         const char close, const std::size_t count,
                         const std::string_view separator,
                         WriteFn &&writeElement) {
  destination.add(open);
  for (std::size_t index = 0; index < count; ++index) {
    writeElement(index);
    if (index + 1 < count) {
      destination.add(separator);
    }
  }
  destination.add(close);
}

inline void addBooleanLiteral(IDestination &destination, const bool value,
                              const std::string_view trueText,
                              const std::string_view falseText) {
  destination.add(value ? trueText : falseText);
}

inline std::string integerToString(const Node &yNode) {
  return std::to_string(NRef<Number>(yNode).value<long long>());
}

} // namespace stringify_detail
} // namespace YAML_Lib
