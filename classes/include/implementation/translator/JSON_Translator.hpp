//
//
// #pragma once
//
#include <unordered_map>
#include <vector>

#include "ITranslator.hpp"
#include "YAML_Converter.hpp"

namespace YAML_Lib {

    // ========================
    // Unicode surrogate ranges
    // ========================

    constexpr char16_t kHighSurrogatesBegin{ 0xD800 };
    constexpr char16_t kHighSurrogatesEnd{ 0xDBFF };
    constexpr char16_t kLowSurrogatesBegin{ 0xDC00 };
    constexpr char16_t kLowSurrogatesEnd{ 0xDFFF };

    class JSON_Translator final : public ITranslator
    {
    public:
        // YAML translator error
        struct Error final : std::runtime_error
        {
            explicit Error(const std::string &message) : std::runtime_error("JSON Translator Error: " + message) {}
        };

        // Pass in user defined converter here
        JSON_Translator() =default;
        // No other constructors supported
        JSON_Translator(const JSON_Translator &other) = delete;
        JSON_Translator &operator=(const JSON_Translator &other) = delete;
        JSON_Translator(JSON_Translator &&other) = delete;
        JSON_Translator &operator=(JSON_Translator &&other) = delete;
        ~JSON_Translator() override = default;

        // Convert to/from YAML escaped characters
        std::string from(const std::string &escapedString) const override{ return escapedString; }
        std::string to(const std::string &rawString) const override{ return rawString; }

    private:
        // To/From escape sequence lookup maps
        mutable std::unordered_map<char, char16_t> fromEscape;
        mutable std::unordered_map<char16_t, char> toEscape;
    };
}// namespace YAML_Lib
