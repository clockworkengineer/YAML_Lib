#pragma once

namespace YAML_Lib {

// YAML error types
struct Error final : std::runtime_error
{
  explicit Error(const std::string &message) : std::runtime_error("YAML Error: " + message) {}
  explicit Error(const std::pair<unsigned long, unsigned long> &position, const std::string &message = "")
    : std::runtime_error("YAML Error [Line: " + std::to_string(position.first)
                         + " Column: " + std::to_string(position.second) + "]: " + message)
  {}
};
struct SyntaxError final : std::runtime_error
{
  explicit SyntaxError(const std::string &message) : std::runtime_error("YAML Syntax Error: " + message) {}
  explicit SyntaxError(const std::pair<unsigned long, unsigned long> &position, const std::string &message = "")
    : std::runtime_error("YAML Syntax Error [Line: " + std::to_string(position.first)
                         + " Column: " + std::to_string(position.second) + "]: " + message)
  {}
};
}// 