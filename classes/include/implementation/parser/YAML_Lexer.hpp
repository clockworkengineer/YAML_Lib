#pragma once

#include <vector>
#include "interface/ISource.hpp"

namespace YAML_Lib {

/**
 * @brief Handles token character scanning, whitespace skipping, and indentation stack tracking.
 *
 * Implements Single Responsibility Principle (SRP) by extracting lexical scanning rules
 * and indentation level state out of syntactic parser components.
 */
class YAML_Lexer {
public:
  explicit YAML_Lexer(ISource &source) : source_(source) {
    indentStack_.push_back(0);
  }

  [[nodiscard]] char current() const {
    return source_.current();
  }

  void next() {
    source_.next();
  }

  [[nodiscard]] bool more() const {
    return source_.more();
  }

  [[nodiscard]] std::size_t position() {
    return source_.position();
  }

  [[nodiscard]] static bool isWhitespace(char ch) noexcept {
    return ch == ' ' || ch == '\t';
  }

  [[nodiscard]] static bool isLineBreak(char ch) noexcept {
    return ch == '\n' || ch == '\r';
  }

  void skipWhitespace() {
    while (more() && isWhitespace(current())) {
      next();
    }
  }

  void skipComment() {
    if (more() && current() == '#') {
      while (more() && !isLineBreak(current())) {
        next();
      }
    }
  }

  void pushIndent(unsigned long level) {
    indentStack_.push_back(level);
  }

  void popIndent() {
    if (indentStack_.size() > 1) {
      indentStack_.pop_back();
    }
  }

  [[nodiscard]] unsigned long currentIndent() const noexcept {
    return indentStack_.back();
  }

  [[nodiscard]] ISource &source() noexcept {
    return source_;
  }

private:
  ISource &source_;
  std::vector<unsigned long> indentStack_;
};

} // namespace YAML_Lib
