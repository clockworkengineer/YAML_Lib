#pragma once

namespace YAML_Lib {

// YAML error types
struct Error final : std::runtime_error {
    explicit Error(const std::string_view &message)
        : std::runtime_error(std::string("YAML Error: ").append(message)) {
    }

    explicit Error(const std::pair<unsigned long, unsigned long> &position,
                   const std::string_view &message = "")
        : std::runtime_error(
            std::string("YAML Error [Line: ").append(std::to_string(position.first))
            .append(" Column: ").append(std::to_string(position.second))
            .append("]: ").append(message)) {
    }
};

struct SyntaxError final : std::runtime_error {
    explicit SyntaxError(const std::string_view &message)
        : std::runtime_error(std::string("YAML Syntax Error: ").append(message)) {
    }

    explicit SyntaxError(const std::pair<unsigned long, unsigned long> &position,
                         const std::string_view message = "")
        : std::runtime_error(
            std::string("YAML Syntax Error [Line: ").append(std::to_string(position.first))
            .append(" Column: ").append(std::to_string(position.second))
            .append("]: ").append(message)) {
    }
};
} // namespace YAML_Lib